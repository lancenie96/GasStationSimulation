#include "Pump.h"

using namespace std;

void Pump::DisplayPumpFrame()
{
	CMutex PUMP_DOSMutex("PUMP_DOSMutex");

	TEXT_COLOUR(10, 0);

	PUMP_DOSMutex.Wait();

	// Display title of gas station
	MOVE_CURSOR(PUMP_FRAME_X, PUMP_FRAME_Y);
	for (int i = 0; i < WINDOW_WIDTH; i++)
		printf("-");
	fflush(stdout);

	printf("\n");

	MOVE_CURSOR(PUMP_FRAME_X, PUMP_FRAME_Y + 1);
	printf("|");
	fflush(stdout);
	MOVE_CURSOR(PUMP_FRAME_X + WINDOW_WIDTH - 1, PUMP_FRAME_Y + 1);
	printf("|");
	fflush(stdout);

	printf("\n");

	MOVE_CURSOR(PUMP_FRAME_X, PUMP_FRAME_Y + 2);
	printf("|");
	fflush(stdout);
	MOVE_CURSOR(PUMP_FRAME_X + 26, PUMP_FRAME_Y + 2);
	printf("GAS STATION PUMP WINDOW");
	fflush(stdout);
	MOVE_CURSOR(PUMP_FRAME_X + WINDOW_WIDTH - 1, PUMP_FRAME_Y + 2);
	printf("|");
	fflush(stdout);

	printf("\n");

	MOVE_CURSOR(PUMP_FRAME_X, PUMP_FRAME_Y + 3);
	printf("|");
	fflush(stdout);
	MOVE_CURSOR(PUMP_FRAME_X + WINDOW_WIDTH - 1, PUMP_FRAME_Y + 3);
	printf("|");
	fflush(stdout);

	printf("\n");

	MOVE_CURSOR(PUMP_FRAME_X, PUMP_FRAME_Y + 4);
	for (int i = 0; i < WINDOW_WIDTH; i++)
		printf("-");
	fflush(stdout);

	// Display title of pumps
	int pos_x;
	int pos_y;
	int pumpNum;

	for (int i = 0; i < PUMP_NUMBER; i++)
	{
		pumpNum = i + 1;

		// Create communication datapool between pump and GSC
		CDataPool dp(string("PumpDP" + to_string(pumpNum)), sizeof(struct pumpdatapooldata));
		struct pumpdatapooldata *PumpGSCDataPool = (struct pumpdatapooldata *)(dp.LinkDataPool());

		pos_x = PUMP_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		pos_y = PUMP_POS_Y + (pumpNum - 1) / 2 * PUMP_FRAME_INTERVAL;

		MOVE_CURSOR(pos_x, pos_y);
		printf("Pump %d Status: Disabled", pumpNum);
		fflush(stdout);

		MOVE_CURSOR(pos_x, pos_y + 1);
		for (int i = 0; i < 30; i++)
			printf("-");
		fflush(stdout);
	}

	PUMP_DOSMutex.Signal();
}


Pump::Pump(int num) 
{ 
	pumpNum = num; 
};

int Pump::main(void) 
{
	// 100 customers, 4 pumps, 1 station computer process
	CRendezvous r("GetReady", rNum);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between pumps and customers
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Create communication pipeline between pump and customer
	CTypedPipe<struct customerdata> pipe(string("pumpPipe" + to_string(pumpNum)), 2048);

	// Create mutex of pipeline between pump and customer
	CMutex pipeMutex(string("pipeMutex" + to_string(pumpNum)));

	// Semaphores to manage the interaction between customer and pump
	CSemaphore PumpAvailable(string("PumpAvailable" + to_string(pumpNum)), 0, 1);
	CSemaphore CustomerSetup(string("CustomerSetup" + to_string(pumpNum)), 0, 1);
	CSemaphore PumpReceivedInfo(string("PumpReceivedInfo" + to_string(pumpNum)), 0, 1);
	CSemaphore CustomerSelectFuel(string("CustomerSelectFuel" + to_string(pumpNum)), 0, 1);
	CSemaphore FuelDispensed(string("FuelDispensed" + to_string(pumpNum)), 0, 1);
	CSemaphore PumpDone(string("PumpDone" + to_string(pumpNum)), 0, 1);
	CSemaphore CustomerExit(string("CustomerExit" + to_string(pumpNum)), 0, 1);

	CMutex PUMP_DOSMutex("PUMP_DOSMutex");

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between pumps and GSC
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Create datapool
	CDataPool dp(string("PumpDP" + to_string(pumpNum)), sizeof(struct pumpdatapooldata));
	struct pumpdatapooldata *PumpGSCDataPool = (struct pumpdatapooldata *)(dp.LinkDataPool());
	
	// Create producer & consumer semaphore
	CSemaphore pumpPS(string("pumpPS" + to_string(pumpNum)), 0, 1);
	CSemaphore pumpCS(string("pumpCS" + to_string(pumpNum)), 1, 1);
	CSemaphore pumpUpdatePS(string("pumpUpdatePS" + to_string(pumpNum)), 0, 1);
	CSemaphore pumpUpdateCS(string("pumpUpdateCS" + to_string(pumpNum)), 1, 1);

	CSemaphore PumpRequest(string("PumpRequest" + to_string(pumpNum)), 0, 1);
	CSemaphore PumpResponse(string("PumpResponse" + to_string(pumpNum)), 0, 1);
	CSemaphore FuelDispensedToPrint(string("FuelDispensedToPrint" + to_string(pumpNum)), 0, 1);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between pumps and fuel tanks
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Create fuel tank datapool
	CDataPool fdp("FuelTank", sizeof(struct fueltank));
	struct fueltank *FuelTankDataPool = (struct fueltank *)(fdp.LinkDataPool());

	CMutex fuelTankMutex("fuelTankMutex");

	// Create fuel price datapool
	CDataPool fpdp("FuelPrice", sizeof(struct fuelprice));
	struct fuelprice *FuelPriceDataPool = (struct fuelprice *)(fpdp.LinkDataPool());

	CMutex fuelPriceMutex("fuelPriceMutex");

	TEXT_COLOUR(10, 0);

	r.Wait(); // Rendezvous

	PUMP_DOSMutex.Wait();

	DisplayPumpFrame();

	PUMP_DOSMutex.Signal();

	while (true)
	{
		// Display setting
		int info_pos_x = PUMP_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		int info_pos_y = PUMP_POS_Y + (pumpNum - 1) / 2 * PUMP_FRAME_INTERVAL + 2;
		int pos_x = PUMP_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		int pos_y = PUMP_POS_Y + (pumpNum - 1) / 2 * PUMP_FRAME_INTERVAL;

		PumpAvailable.Signal(); // Let in a customer

		CustomerSetup.Wait(); // wait until the customer completes entering information

		// Pump receives data from customer
		PUMP_DOSMutex.Wait();

		MOVE_CURSOR(info_pos_x, info_pos_y + 2);
		printf("                                "); // clear line
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 2);
		TEXT_COLOUR(10, 0);
		printf("Receiving data from customer\n");
		fflush(stdout);

		PUMP_DOSMutex.Signal();

		pipeMutex.Wait();
		struct customerdata CustomerStruct;

		pipe.Read(&CustomerStruct);

		PUMP_DOSMutex.Wait();

		MOVE_CURSOR(info_pos_x, info_pos_y + 3);
		printf("                            "); // clear line
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 3);
		TEXT_COLOUR(10, 0);
		printf("Name: %s %s", names[CustomerStruct.firstNameHash].c_str(), 
			names[CustomerStruct.lastNameHash].c_str());
		fflush(stdout);

		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 "); // clear line
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		TEXT_COLOUR(10, 0);
		printf("Credit card: %d-%d-%d-%d", CustomerStruct.first4Digit, CustomerStruct.second4Digit, 
			CustomerStruct.third4Digit, CustomerStruct.fourth4Digit);
		fflush(stdout);

		PUMP_DOSMutex.Signal();

		pipeMutex.Signal();

		PumpReceivedInfo.Signal();

		CustomerSelectFuel.Wait(); // Wait for customer to select fuel grade

		PUMP_DOSMutex.Wait();

		MOVE_CURSOR(info_pos_x, info_pos_y + 7);
		printf("                     "); // clear line
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 7);
		TEXT_COLOUR(10, 0);
		printf("Fuel grade: %d", CustomerStruct.fuelGrade);
		fflush(stdout);

		PUMP_DOSMutex.Signal();

		// Pump sends data to GSC 
		pumpCS.Wait();

		// Customer name
		PumpGSCDataPool->lastNameHash	= CustomerStruct.lastNameHash;
		PumpGSCDataPool->firstNameHash	= CustomerStruct.firstNameHash;

		// Customer credit card
		PumpGSCDataPool->first4Digit	= CustomerStruct.first4Digit;
		PumpGSCDataPool->second4Digit	= CustomerStruct.second4Digit;
		PumpGSCDataPool->third4Digit	= CustomerStruct.third4Digit;
		PumpGSCDataPool->fourth4Digit	= CustomerStruct.fourth4Digit;

		// Fuel and pump information
		PumpGSCDataPool->fuelGrade		= CustomerStruct.fuelGrade;
		PumpGSCDataPool->pumpNum		= pumpNum;
		PumpGSCDataPool->fuelCost		= 0.0;
		double costTemp = PumpGSCDataPool->fuelCost;
		PumpGSCDataPool->fuelDispensed	= 0.0;
		double fuelDispensedTemp = PumpGSCDataPool->fuelDispensed;
		PumpGSCDataPool->pumpdone[pumpNum - 1] = 0;

		pumpPS.Signal();

		PumpRequest.Signal(); // Pump sends request for dispensing fuel to GSC

		// Wait for attendant to signal the pump
		PumpResponse.Wait();

		PUMP_DOSMutex.Wait();

		MOVE_CURSOR(pos_x, pos_y);
		printf("                        ");
		fflush(stdout);
		MOVE_CURSOR(pos_x, pos_y);
		TEXT_COLOUR(10, 0);
		printf("Pump %d Status: Enabled", pumpNum);
		fflush(stdout);

		MOVE_CURSOR(info_pos_x, info_pos_y + 8);
		printf("                     "); // clear line
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 8);
		TEXT_COLOUR(10, 0);
		printf("Dispensing fuel...\n");
		fflush(stdout);

		PUMP_DOSMutex.Signal();

		// Check fuel amount
		double fuelAmount;
		fuelTankMutex.Wait();
		switch (CustomerStruct.fuelGrade)
		{
			case 87: fuelAmount = FuelTankDataPool->fuel87level;
				break;
			case 89: fuelAmount = FuelTankDataPool->fuel89level;
				break;
			case 91: fuelAmount = FuelTankDataPool->fuel91level;
				break;
			case 93: fuelAmount = FuelTankDataPool->fuel93level;
				break;
		}
		fuelTankMutex.Signal();

		double cost = 0.0;
		fuelPriceMutex.Wait();
		switch (CustomerStruct.fuelGrade)
		{
			case 87: cost = FuelPriceDataPool->fuel87Price;
				break;
			case 89: cost = FuelPriceDataPool->fuel89Price;
				break;
			case 91: cost = FuelPriceDataPool->fuel91Price;
				break;
			case 93: cost = FuelPriceDataPool->fuel93Price;
				break;
		}
		fuelPriceMutex.Signal();


		double delta;
		while (fuelAmount > 0.0 && fuelDispensedTemp < min(70.0, CustomerStruct.fuelNeeded))
		{
			delta = min(70.0, CustomerStruct.fuelNeeded) - fuelDispensedTemp;

			if (fuelAmount >= delta && delta < 0.5 && fuelAmount > 0.0) // last one
			{
				fuelTankMutex.Wait();
				switch (CustomerStruct.fuelGrade)
				{
					case 87: FuelTankDataPool->fuel87level = FuelTankDataPool->fuel87level - delta;
						break;
					case 89: FuelTankDataPool->fuel89level = FuelTankDataPool->fuel89level - delta;
						break;
					case 91: FuelTankDataPool->fuel91level = FuelTankDataPool->fuel91level - delta;
						break;
					case 93: FuelTankDataPool->fuel93level = FuelTankDataPool->fuel93level - delta;
						break;
				}
				fuelTankMutex.Signal();

				pumpUpdateCS.Wait();
				PumpGSCDataPool->fuelDispensed = PumpGSCDataPool->fuelDispensed + delta;
				fuelDispensedTemp = PumpGSCDataPool->fuelDispensed;
				PumpGSCDataPool->fuelCost = PumpGSCDataPool->fuelCost + cost * delta;
				costTemp = PumpGSCDataPool->fuelCost;
				PumpGSCDataPool->pumpdone[pumpNum - 1] = 1;
				pumpUpdatePS.Signal();
			}
			else if (fuelAmount >= delta && delta >= 0.5 && fuelAmount > 0.0)
			{
				fuelTankMutex.Wait();
				switch (CustomerStruct.fuelGrade)
				{
					case 87: FuelTankDataPool->fuel87level = FuelTankDataPool->fuel87level - 0.5;
						break;
					case 89: FuelTankDataPool->fuel89level = FuelTankDataPool->fuel89level - 0.5;
						break;
					case 91: FuelTankDataPool->fuel91level = FuelTankDataPool->fuel91level - 0.5;
						break;
					case 93: FuelTankDataPool->fuel93level = FuelTankDataPool->fuel93level - 0.5;
						break;
				}
				fuelTankMutex.Signal();

				pumpUpdateCS.Wait();
				PumpGSCDataPool->fuelDispensed = PumpGSCDataPool->fuelDispensed + 0.5;
				fuelDispensedTemp = PumpGSCDataPool->fuelDispensed;
				PumpGSCDataPool->fuelCost = PumpGSCDataPool->fuelCost + cost * 0.5;
				costTemp = PumpGSCDataPool->fuelCost;
				pumpUpdatePS.Signal();
			}
			else if (fuelAmount > 0.0) // in this case, fuelAmount < delta, pump can only dispense 'fuelAmount' amount of fuel. last one
			{
				fuelTankMutex.Wait();
				switch (CustomerStruct.fuelGrade)
				{
					case 87: FuelTankDataPool->fuel87level = FuelTankDataPool->fuel87level - fuelAmount;
						break;
					case 89: FuelTankDataPool->fuel89level = FuelTankDataPool->fuel89level - fuelAmount;
						break;
					case 91: FuelTankDataPool->fuel91level = FuelTankDataPool->fuel91level - fuelAmount;
						break;
					case 93: FuelTankDataPool->fuel93level = FuelTankDataPool->fuel93level - fuelAmount;
						break;
				}
				fuelTankMutex.Signal();

				pumpUpdateCS.Wait();
				PumpGSCDataPool->fuelDispensed = PumpGSCDataPool->fuelDispensed + fuelAmount;
				fuelDispensedTemp = PumpGSCDataPool->fuelDispensed;
				PumpGSCDataPool->fuelCost = PumpGSCDataPool->fuelCost + cost * fuelAmount;
				costTemp = PumpGSCDataPool->fuelCost;
				PumpGSCDataPool->pumpdone[pumpNum - 1] = 1;
				pumpUpdatePS.Signal();
			}
			else
				break;

			PUMP_DOSMutex.Wait();

			MOVE_CURSOR(info_pos_x, info_pos_y + 9);
			printf("                                  ");
			fflush(stdout);
			MOVE_CURSOR(info_pos_x, info_pos_y + 9);
			TEXT_COLOUR(10, 0);
			printf("Fuel dispensed: %.3f Liters", fuelDispensedTemp);
			fflush(stdout);

			MOVE_CURSOR(info_pos_x, info_pos_y + 10);
			printf("                     "); // clear line
			fflush(stdout);
			MOVE_CURSOR(info_pos_x, info_pos_y + 10);
			TEXT_COLOUR(10, 0);
			printf("Cost: $ %.2f\n", costTemp);
			fflush(stdout);

			PUMP_DOSMutex.Signal();

			SLEEP(HALF_LITER_PER_X_SECOND);
		}

		FuelDispensed.Signal(); 

		PumpDone.Signal(); // pump complete dispensing fuel

		CustomerExit.Wait(); // wait for customer to leave

		PUMP_DOSMutex.Wait();

		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 "); 
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 "); 
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 "); 
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 "); 
		fflush(stdout);

		MOVE_CURSOR(pos_x, pos_y);
		printf("                        ");
		fflush(stdout);
		MOVE_CURSOR(pos_x, pos_y);
		TEXT_COLOUR(10, 0);
		printf("Pump %d Status: Disabled", pumpNum);
		fflush(stdout);

		PUMP_DOSMutex.Signal();
	}

	return 0;
};
