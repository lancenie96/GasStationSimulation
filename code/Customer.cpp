
#include "Customer.h"

Customer::Customer(int num)
{
	customerNum = num; 

	srand(customerNum); // random number generator

	// Generate name
	firstNameHash = rand() % NAME_SPACE;
	lastNameHash = rand() % NAME_SPACE;
	name = names[firstNameHash] + " " + names[lastNameHash];

	// Generate credit card number
	first4Digit = rand() % 8999 + 1000;
	second4Digit = rand() % 8999 + 1000;
	third4Digit = rand() % 8999 + 1000;
	fourth4Digit = rand() % 8999 + 1000;
	CreditCardNum = to_string(first4Digit) + to_string(second4Digit) + to_string(third4Digit) + to_string(fourth4Digit);

	// Fuel grade wanted
	int iFuelGrade = rand() % 4;
	switch (iFuelGrade)
	{
		case 0: fuelGrade = 87;
			break;
		case 1: fuelGrade = 89;
			break;
		case 2: fuelGrade = 91;
			break;
		case 3: fuelGrade = 93;
			break;
		default: fuelGrade = 87;
			break;
	}

	// Amount of fuel needed
	double fuelDecimal = double(rand() % 71);
	double fuelFraction = double(rand() % 1000) / 1000.0;
	fuelNeeded = fuelDecimal + fuelFraction;
};

int Customer::main(void)
{
	// 100 customers, 4 pumps, 1 station computer process
	CRendezvous r("GetReady", rNum);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between pumps and customers
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Customer randomly selects pump 1-4
	srand(customerNum);
	int pumpNum = rand() % PUMP_NUMBER + 1;
	
	// Create communication pipeline between pump and customer
	CTypedPipe<struct customerdata> pipe(string("pumpPipe" + to_string(pumpNum)), 2048);

	// Create mutex of pipeline between pump and customer
	CMutex pipeMutex(string("pipeMutex" + to_string(pumpNum)));
	
	// Semaphores to manage the interaction between customer and pump
	CSemaphore PumpAvailable(string("PumpAvailable" + to_string(pumpNum)), 0, 1);
	CSemaphore CustomerSetup(string("CustomerSetup" + to_string(pumpNum)), 0, 1);
	CSemaphore CustomerSelectFuel(string("CustomerSelectFuel" + to_string(pumpNum)), 0, 1);
	CSemaphore FuelDispensed(string("FuelDispensed" + to_string(pumpNum)), 0, 1);
	CSemaphore PumpDone(string("PumpDone" + to_string(pumpNum)), 0, 1);
	CSemaphore CustomerExit(string("CustomerExit" + to_string(pumpNum)), 0, 1);
	CSemaphore PumpReceivedInfo(string("PumpReceivedInfo" + to_string(pumpNum)), 0, 1);

	CMutex PUMP_DOSMutex("PUMP_DOSMutex");

	TEXT_COLOUR(14, 0);

	r.Wait(); // Rendezvous
	
	while (true)
	{
		// Display setting
		int info_pos_x = CPUMP_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		int info_pos_y = CPUMP_POS_Y + (pumpNum - 1) / 2 * CPUMP_FRAME_INTERVAL + 2;
		int pos_x = CPUMP_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		int pos_y = CPUMP_POS_Y + (pumpNum - 1) / 2 * CPUMP_FRAME_INTERVAL;

		PumpAvailable.Wait(); // wait until pump available

		PUMP_DOSMutex.Wait();
		MOVE_CURSOR(info_pos_x, info_pos_y + 0);
		printf("                             "); 
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 0);
		TEXT_COLOUR(14, 0);
		printf("Customer arrives\n");
		fflush(stdout);
		PUMP_DOSMutex.Signal();

		SLEEP(500);

		PUMP_DOSMutex.Wait();
		MOVE_CURSOR(info_pos_x, info_pos_y + 1);
		printf("                             "); 
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 1);
		TEXT_COLOUR(14, 0);
		printf("Customer entering info...\n");
		fflush(stdout);
		PUMP_DOSMutex.Signal();

		SLEEP(500); // customer entering information

		// Customer data sent to pump, then to GSC
		pipeMutex.Wait();
		struct customerdata CustomerStruct = { lastNameHash, firstNameHash, first4Digit, second4Digit, third4Digit, fourth4Digit,
			fuelGrade, fuelNeeded, pumpNum };
		pipe.Write(&CustomerStruct);
		pipeMutex.Signal();

		CustomerSetup.Signal(); // Customer finish entering information

		PumpReceivedInfo.Wait();

		SLEEP(500);

		PUMP_DOSMutex.Wait();
		MOVE_CURSOR(info_pos_x, info_pos_y + 5);
		printf("                                 "); 
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 5);
		TEXT_COLOUR(14, 0);
		printf("Customer removes gas hose\n"); 
		fflush(stdout);
		PUMP_DOSMutex.Signal();

		SLEEP(500);

		PUMP_DOSMutex.Wait();
		MOVE_CURSOR(info_pos_x, info_pos_y + 6);
		printf("                                 "); 
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 6);
		TEXT_COLOUR(14, 0);
		printf("Customer selects fuel grade...");
		fflush(stdout);
		PUMP_DOSMutex.Signal();

		CustomerSelectFuel.Signal(); // Customer selects fuel grade

		FuelDispensed.Wait(); // wait for the pump to be signaled

		PumpDone.Wait(); // wait until pump completes dispensing fuel

		SLEEP(500);

		PUMP_DOSMutex.Wait();
		MOVE_CURSOR(info_pos_x, info_pos_y + 11);
		printf("                     ");
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 11);
		TEXT_COLOUR(14, 0);
		printf("Customer leaves");
		fflush(stdout);
		PUMP_DOSMutex.Signal();

		SLEEP(700);

		CustomerExit.Signal(); // Customer leaves

		PUMP_DOSMutex.Wait();
		for (int i = 0; i < 12; i++)
		{
			MOVE_CURSOR(info_pos_x, info_pos_y + i);
			printf("                              ");
			fflush(stdout);
		}
		PUMP_DOSMutex.Signal();
	}

	return 0;
};