#include <stdio.h>
#include <string>
#include <iostream>
#include <list>
#include <conio.h>
#include <ctype.h>
#include "Customer.h"
#include "Pump.h"
#include "InitFile.h"
#include "..\RTLibrary\rt.h"
using namespace std;
#define _CRT_SECURE_NO_WARNINGS

// Define constants for display position on DOS
#define ENTER_PRICE_POS_X 0
#define ENTER_PRICE_POS_y 0
#define FRAME_POS_X 3
#define FRAME_POS_Y 1
#define PUMP_INTERVAL 12
#define PRICE_POS_X 3
#define PRICE_POS_Y 8		// FRAME_POS_Y + 7
#define FUEL_LEVEL_POS_X 30
#define FUEL_LEVEL_POS_Y 8 // FRAME_POS_Y + 7
#define PUMP_INFO_POS_X 3
#define PUMP_INFO_POS_Y 18	// PRICE_POS_Y + 10

#define NEW_PRICE_INFO_POS_Y 67
#define TRAN_INFO_POS_Y 75
#define COMMAND_INFO_Y 53

// DOS mutex
CMutex GSC_DOSMutex("GSC_DOSMutex");

int test[4];

char commandSignal[PUMP_NUMBER];
char commandRefill[4];
char commandChangeCost[2];
char commandDisplayTran[2];
char commandHideTran[2];

int grade;

list<pumpdatapooldata> tranList;

// Display the prices of fuels
void DisplayFrame()
{
	GSC_DOSMutex.Wait();

	CLEAR_SCREEN();

	// Display title of gas station
	MOVE_CURSOR(FRAME_POS_X, FRAME_POS_Y);
	for (int i = 0; i < WINDOW_WIDTH; i++)
		printf("-");
	fflush(stdout);

	printf("\n");

	MOVE_CURSOR(FRAME_POS_X, FRAME_POS_Y + 1);
	printf("|");
	fflush(stdout);
	MOVE_CURSOR(FRAME_POS_X + WINDOW_WIDTH - 1, FRAME_POS_Y + 1);
	printf("|");
	fflush(stdout);

	printf("\n"); 
	
	MOVE_CURSOR(FRAME_POS_X, FRAME_POS_Y + 2);
	printf("|");
	fflush(stdout);
	MOVE_CURSOR(FRAME_POS_X + 26, FRAME_POS_Y + 2);
	printf("CHEVRON GAS STATION");
	fflush(stdout);
	MOVE_CURSOR(FRAME_POS_X + WINDOW_WIDTH - 1, FRAME_POS_Y + 2);
	printf("|");
	fflush(stdout);

	printf("\n");

	MOVE_CURSOR(FRAME_POS_X, FRAME_POS_Y + 3);
	printf("|");
	fflush(stdout);
	MOVE_CURSOR(FRAME_POS_X + WINDOW_WIDTH - 1, FRAME_POS_Y + 3);
	printf("|");
	fflush(stdout);

	printf("\n");

	MOVE_CURSOR(FRAME_POS_X, FRAME_POS_Y + 4);
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

		pos_x = PUMP_INFO_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		pos_y = PUMP_INFO_POS_Y + (pumpNum - 1) / 2 * PUMP_INTERVAL;

		MOVE_CURSOR(pos_x, pos_y);
		printf("Pump %d Status: Disabled", pumpNum);
		fflush(stdout);

		MOVE_CURSOR(pos_x, pos_y + 1);
		for (int i = 0; i < 30; i++)
			printf("-");
		fflush(stdout);
	}

	GSC_DOSMutex.Signal();
}

// Set the prices of fuels
void SetFuelPrice()
{
	// Create fuel price datapool
	CDataPool fpdp("FuelPrice", sizeof(struct fuelprice));
	struct fuelprice *FuelPriceDataPool = (struct fuelprice *)(fpdp.LinkDataPool());

	CMutex fuelPriceMutex("fuelPriceMutex");

	GSC_DOSMutex.Wait();

	CLEAR_SCREEN();

	fuelPriceMutex.Wait();

	MOVE_CURSOR(ENTER_PRICE_POS_X, ENTER_PRICE_POS_y + 0);
	printf("Enter Fuel 87 initial price: ");
	cin >> FuelPriceDataPool->fuel87Price;
	fflush(stdout);

	MOVE_CURSOR(ENTER_PRICE_POS_X, ENTER_PRICE_POS_y + 1);
	printf("Enter Fuel 89 initial price: ");
	cin >> FuelPriceDataPool->fuel89Price;
	fflush(stdout);

	MOVE_CURSOR(ENTER_PRICE_POS_X, ENTER_PRICE_POS_y + 2);
	printf("Enter Fuel 91 initial price: ");
	cin >> FuelPriceDataPool->fuel91Price;
	fflush(stdout);

	MOVE_CURSOR(ENTER_PRICE_POS_X, ENTER_PRICE_POS_y + 3);
	printf("Enter Fuel 93 initial price: ");
	cin >> FuelPriceDataPool->fuel93Price;
	fflush(stdout);

	fuelPriceMutex.Signal();

	GSC_DOSMutex.Signal();
}

// Display the prices of fuels
void DisplayFuelPrice()
{
	// Create fuel price datapool
	CDataPool fpdp("FuelPrice", sizeof(struct fuelprice));
	struct fuelprice *FuelPriceDataPool = (struct fuelprice *)(fpdp.LinkDataPool());

	CMutex fuelPriceMutex("fuelPriceMutex");

	GSC_DOSMutex.Wait();

	fuelPriceMutex.Wait();

	MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y);
	printf("Fuel 87 Price: $%.2f", FuelPriceDataPool->fuel87Price);
	fflush(stdout);

	MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y + 1);
	printf("Fuel 89 Price: $%.2f", FuelPriceDataPool->fuel89Price);
	fflush(stdout);

	MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y + 2);
	printf("Fuel 91 Price: $%.2f", FuelPriceDataPool->fuel91Price);
	fflush(stdout);

	MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y + 3);
	printf("Fuel 93 Price: $%.2f", FuelPriceDataPool->fuel93Price);
	fflush(stdout);

	fuelPriceMutex.Signal();

	GSC_DOSMutex.Signal();
}

// Display the fuel levels in the tank 
void DisplayFuelLevel()
{
	// Create fuel tank datapool
	CDataPool fdp("FuelTank", sizeof(struct fueltank));
	struct fueltank *FuelTankDataPool = (struct fueltank *)(fdp.LinkDataPool());

	CMutex fuelTankMutex("fuelTankMutex");

	GSC_DOSMutex.Wait();

	fuelTankMutex.Wait();

	MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y);
	printf("Fuel 87 level: %.3f Liters", FuelTankDataPool->fuel87level);
	fflush(stdout);

	MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 1);
	printf("Fuel 89 level: %.3f Liters", FuelTankDataPool->fuel89level);
	fflush(stdout);

	MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 2);
	printf("Fuel 91 level: %.3f Liters", FuelTankDataPool->fuel91level);
	fflush(stdout);

	MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 3);
	printf("Fuel 93 level: %.3f Liters", FuelTankDataPool->fuel93level);
	fflush(stdout);

	fuelTankMutex.Signal();

	GSC_DOSMutex.Signal();
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Thread
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Thread for communication between GSC and pump 4
UINT __stdcall PumpCommunication(void *args)
{
	// To protect thread creations
	CSemaphore GSCThreadMutex("GSCThreadMutex", 1);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between pumps and GSC
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Pump number
	int pumpNum = *(int *)(args);

	// Create communication datapool between pump and GSC
	CDataPool dp(string("PumpDP" + to_string(pumpNum)), sizeof(struct pumpdatapooldata));
	struct pumpdatapooldata *PumpGSCDataPool = (struct pumpdatapooldata *)(dp.LinkDataPool());

	// Create producer & consumer semaphores
	CSemaphore pumpPS(string("pumpPS" + to_string(pumpNum)), 0, 1);
	CSemaphore pumpCS(string("pumpCS" + to_string(pumpNum)), 1, 1);
	CSemaphore pumpUpdatePS(string("pumpUpdatePS" + to_string(pumpNum)), 0, 1);
	CSemaphore pumpUpdateCS(string("pumpUpdateCS" + to_string(pumpNum)), 1, 1);

	CSemaphore PumpRequest(string("PumpRequest" + to_string(pumpNum)), 0, 1);
	CSemaphore PumpResponse(string("PumpResponse" + to_string(pumpNum)), 0, 1);
	CSemaphore FuelDispensedToPrint(string("FuelDispensedToPrint" + to_string(pumpNum)), 0, 1);

	int pumpDoneFlag;

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between GSC and fuel tanks
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Create fuel tank datapool
	CDataPool fdp("FuelTank", sizeof(struct fueltank));
	struct fueltank *FuelTankDataPool = (struct fueltank *)(fdp.LinkDataPool());

	CMutex fuelTankMutex("fuelTankMutex");

	// Create fuel price datapool
	CDataPool fpdp("FuelPrice", sizeof(struct fuelprice));
	struct fuelprice *FuelPriceDataPool = (struct fuelprice *)(fpdp.LinkDataPool());

	CMutex fuelPriceMutex("fuelPriceMutex");

	CSemaphore Refill87Done("Refill87Done", 0, 1);
	CSemaphore Refill89Done("Refill89Done", 0, 1);
	CSemaphore Refill91Done("Refill91Done", 0, 1);
	CSemaphore Refill93Done("Refill93Done", 0, 1);

	CSemaphore PumpSignalMutex(string("PumpSignalMutex" + to_string(pumpNum)), 0, 1);

	// Indicate that this thread has been created
	GSCThreadMutex.Signal();

	while (true)
	{
		// Display setting
		int info_pos_x = PUMP_INFO_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		int info_pos_y = PUMP_INFO_POS_Y + (pumpNum - 1) / 2 * PUMP_INTERVAL + 2;

		// GSC receives fuel dispensing information from pump
		PumpRequest.Wait(); // wait for the request from pump

		GSC_DOSMutex.Wait();

		pumpPS.Wait();

		// Print customer name 
		MOVE_CURSOR(info_pos_x, info_pos_y + 0);
		printf("                                 "); // clear line
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 0);
		printf("Name: %s %s", names[PumpGSCDataPool->firstNameHash].c_str(), names[PumpGSCDataPool->lastNameHash].c_str());
		fflush(stdout);

		// Print customer credit card number
		MOVE_CURSOR(info_pos_x, info_pos_y + 1);
		printf("Credit Card #: %d-%d-%d-%d", PumpGSCDataPool->first4Digit, PumpGSCDataPool->second4Digit,
			PumpGSCDataPool->third4Digit, PumpGSCDataPool->fourth4Digit);
		fflush(stdout);

		// Print fuel grade selected by customer
		MOVE_CURSOR(info_pos_x, info_pos_y + 2);
		printf("Fuel Grade: %d", PumpGSCDataPool->fuelGrade);
		fflush(stdout);
		grade = PumpGSCDataPool->fuelGrade;

		// Print amount of fuel that has been dispensed
		MOVE_CURSOR(info_pos_x, info_pos_y + 3);
		printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 3);
		printf("Amount dispensed: %.3f liters", PumpGSCDataPool->fuelDispensed);
		fflush(stdout);

		// Print running cost
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 ");
		fflush(stdout);
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("Cost: $ %.2f", PumpGSCDataPool->fuelCost);
		fflush(stdout);

		// Read pumpdone flag
		pumpDoneFlag = PumpGSCDataPool->pumpdone[pumpNum - 1];

		pumpCS.Signal();

		TEXT_COLOUR(2, 0);
		MOVE_CURSOR(0, COMMAND_INFO_Y + 4);
		printf("To refill fuel 87 tank, press o + 1 + ENTER + ENTER.");
		fflush(stdout);
		MOVE_CURSOR(0, COMMAND_INFO_Y + 5);
		printf("To refill fuel 89 tank, press p + 2 + ENTER + ENTER.");
		fflush(stdout);
		MOVE_CURSOR(0, COMMAND_INFO_Y + 6);
		printf("To refill fuel 91 tank, press q + 3 + ENTER + ENTER.");
		fflush(stdout);
		MOVE_CURSOR(0, COMMAND_INFO_Y + 7);
		printf("To refill fuel 93 tank, press r + 4 + ENTER + ENTER.");
		fflush(stdout);

		MOVE_CURSOR(0, COMMAND_INFO_Y + 8);
		printf("To display transaction history, press d + t + ENTER + ENTER.");
		fflush(stdout);
		MOVE_CURSOR(0, COMMAND_INFO_Y + 9);
		printf("To hide transaction history, press h + t + ENTER + ENTER.");
		fflush(stdout);
		MOVE_CURSOR(0, COMMAND_INFO_Y + 10);
		printf("To change prices, press c + c + ENTER + ENTER.");
		fflush(stdout);

		MOVE_CURSOR(0, COMMAND_INFO_Y + 12);
		printf("The command entered is:");
		fflush(stdout);
		TEXT_COLOUR(15, 0);

		GSC_DOSMutex.Signal();
		
		// Check fuel amount
		double fuelAmount;

		fuelTankMutex.Wait();
		switch (grade)
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

		if (fuelAmount < 200.0) // if fuel amount less than 200 liters
		{
			int gradeHash;
			switch (grade)
			{
				case 87: gradeHash = 1;
					break;
				case 89: gradeHash = 2;
					break;
				case 91: gradeHash = 3;
					break;
				case 93: gradeHash = 4;
					break;
			}

			GSC_DOSMutex.Wait();

			MOVE_CURSOR(info_pos_x, info_pos_y + 7);
			TEXT_COLOUR(12, 0);
			printf("WARNING: Fuel %d tank almost empty.", grade);
			fflush(stdout);
			MOVE_CURSOR(info_pos_x, info_pos_y + 8);
			TEXT_COLOUR(12, 0);
			printf("Request denied. %c+%d to refill.", commandRefill[gradeHash - 1], gradeHash);
			fflush(stdout);
			TEXT_COLOUR(15, 0);

			GSC_DOSMutex.Signal();

			while (true) // break when tank refilled
			{
				if (grade == 87)
				{
					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[0] = 1;
					fuelTankMutex.Signal();

					Refill87Done.Wait();

					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[0] = 0;
					fuelTankMutex.Signal();

					// Update fuel tank information
					GSC_DOSMutex.Wait();

					fuelTankMutex.Wait();

					MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 0);
					printf("Fuel 87 level: %.3f Liters", FuelTankDataPool->fuel87level);
					fflush(stdout);

					fuelTankMutex.Signal();

					GSC_DOSMutex.Signal();

					break;
				}
				else if (grade == 89)
				{
					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[1] = 1;
					fuelTankMutex.Signal();

					Refill89Done.Wait();

					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[1] = 0;
					fuelTankMutex.Signal();

					// Update fuel tank information
					GSC_DOSMutex.Wait();

					fuelTankMutex.Wait();

					MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 1);
					printf("Fuel 89 level: %.3f Liters", FuelTankDataPool->fuel89level);
					fflush(stdout);

					fuelTankMutex.Signal();

					GSC_DOSMutex.Signal();

					break;
				}
				else if (grade == 91)
				{
					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[2] = 1;
					fuelTankMutex.Signal();

					Refill91Done.Wait();

					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[2] = 0;
					fuelTankMutex.Signal();

					// Update fuel tank information
					GSC_DOSMutex.Wait();

					fuelTankMutex.Wait();

					MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 2);
					printf("Fuel 91 level: %.3f Liters", FuelTankDataPool->fuel91level);
					fflush(stdout);

					fuelTankMutex.Signal();

					GSC_DOSMutex.Signal();

					break;
				}
				else if (grade == 93)
				{
					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[3] = 1;
					fuelTankMutex.Signal();

					Refill93Done.Wait();

					fuelTankMutex.Wait();
					FuelTankDataPool->refillRequest[3] = 0;
					fuelTankMutex.Signal();

					// Update fuel tank information
					GSC_DOSMutex.Wait();

					fuelTankMutex.Wait();

					MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 3);
					printf("Fuel 93 level: %.3f Liters", FuelTankDataPool->fuel93level);
					fflush(stdout);

					fuelTankMutex.Signal();

					GSC_DOSMutex.Signal();

					break;
				}
			}

			GSC_DOSMutex.Wait();

			MOVE_CURSOR(info_pos_x, info_pos_y + 7);
			TEXT_COLOUR(12, 0);
			printf("                                   ");
			fflush(stdout);
			MOVE_CURSOR(info_pos_x, info_pos_y + 8);
			TEXT_COLOUR(12, 0);
			printf("                              ");
			fflush(stdout);
			TEXT_COLOUR(15, 0);

			GSC_DOSMutex.Signal();
		}

		GSC_DOSMutex.Wait();

		MOVE_CURSOR(info_pos_x, info_pos_y + 6);
		TEXT_COLOUR(6, 0);
		printf("Press %c+%d to signal pump.", commandSignal[pumpNum - 1], pumpNum);
		fflush(stdout);
		TEXT_COLOUR(15, 0);

		GSC_DOSMutex.Signal();

		PumpSignalMutex.Wait();

		int pos_x = PUMP_INFO_POS_X + (pumpNum % 2 == 0 ? 35 : 0);
		int pos_y = PUMP_INFO_POS_Y + (pumpNum - 1) / 2 * PUMP_INTERVAL;

		GSC_DOSMutex.Wait();
		MOVE_CURSOR(pos_x, pos_y);
		printf("                        ");
		fflush(stdout);
		MOVE_CURSOR(pos_x, pos_y);
		printf("Pump %d Status: Enabled", pumpNum);
		fflush(stdout);
		GSC_DOSMutex.Signal();

		while (pumpDoneFlag != 1)
		{
			GSC_DOSMutex.Wait();

			// Update fuel tank information
			fuelTankMutex.Wait();

			MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 0);
			printf("Fuel 87 level: %.3f Liters", FuelTankDataPool->fuel87level);
			fflush(stdout);

			MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 1);
			printf("Fuel 89 level: %.3f Liters", FuelTankDataPool->fuel89level);
			fflush(stdout);

			MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 2);
			printf("Fuel 91 level: %.3f Liters", FuelTankDataPool->fuel91level);
			fflush(stdout);

			MOVE_CURSOR(FUEL_LEVEL_POS_X, FUEL_LEVEL_POS_Y + 3);
			printf("Fuel 93 level: %.3f Liters", FuelTankDataPool->fuel93level);
			fflush(stdout);

			fuelTankMutex.Signal();

			// Update pump information
			pumpUpdatePS.Wait();

			// Print amount of fuel that has been dispensed
			MOVE_CURSOR(info_pos_x, info_pos_y + 3);
			printf("                                 "); // clear line
			fflush(stdout);
			MOVE_CURSOR(info_pos_x, info_pos_y + 3);
			printf("Amount dispensed: %.3f liters", PumpGSCDataPool->fuelDispensed);
			fflush(stdout);

			// Print running cost
			MOVE_CURSOR(info_pos_x, info_pos_y + 4);
			printf("                                 "); // clear line
			fflush(stdout);
			MOVE_CURSOR(info_pos_x, info_pos_y + 4);
			printf("Cost: $ %.2f", PumpGSCDataPool->fuelCost);
			fflush(stdout);

			// Read pumpdone flag
			pumpDoneFlag = PumpGSCDataPool->pumpdone[pumpNum - 1];

			pumpUpdateCS.Signal();

			GSC_DOSMutex.Signal();
		}

		GSC_DOSMutex.Wait();
		MOVE_CURSOR(pos_x, pos_y);
		printf("                        ");
		fflush(stdout);
		MOVE_CURSOR(pos_x, pos_y);
		printf("Pump %d Status: Disabled", pumpNum);
		fflush(stdout);
		GSC_DOSMutex.Signal();

		tranList.push_back(*PumpGSCDataPool);

		GSC_DOSMutex.Wait();

		// Print customer name 
		MOVE_CURSOR(info_pos_x, info_pos_y + 0);
		printf("                                 "); 
		fflush(stdout);
		// Print customer credit card number
		MOVE_CURSOR(info_pos_x, info_pos_y + 1);
		printf("                                  ");
		fflush(stdout);
		// Print fuel grade selected by customer
		MOVE_CURSOR(info_pos_x, info_pos_y + 2);
		printf("                                 ");
		fflush(stdout);
		// Print amount of fuel that has been dispensed
		MOVE_CURSOR(info_pos_x, info_pos_y + 3);
		printf("                                 ");
		fflush(stdout);
		// Print running cost
		MOVE_CURSOR(info_pos_x, info_pos_y + 4);
		printf("                                 ");
		fflush(stdout);

		GSC_DOSMutex.Signal();
	}

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Thread
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Thread for communication between GSC and pump 4
UINT __stdcall FuelLevelCheck(void *args)
{
	int num = *(int *)(args);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between GSC and fuel tanks
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Create fuel tank datapool
	CDataPool fdp("FuelTank", sizeof(struct fueltank));
	struct fueltank *FuelTankDataPool = (struct fueltank *)(fdp.LinkDataPool());

	CMutex fuelTankMutex("fuelTankMutex");

	// do something here
	int warningFlag[4];
	for (int i = 0; i < 4; i++)
		warningFlag[i] = 0;

	double sleeptime = 99999999;

	double level87;
	double level89;
	double level91;
	double level93;

	while (true)
	{
		fuelTankMutex.Wait();
		level87 = FuelTankDataPool->fuel87level;
		level89 = FuelTankDataPool->fuel89level;
		level91 = FuelTankDataPool->fuel91level;
		level93 = FuelTankDataPool->fuel93level;
		fuelTankMutex.Signal();

		if (num == 0)
		{
			if (level87 < 200.0) {
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 0);
				TEXT_COLOUR(0, 12);
				printf(" ");
				TEXT_COLOUR(15, 0);
				fflush(stdout);
				GSC_DOSMutex.Signal();

				for (double i = 0.0; i < sleeptime; i++) {}

				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 0);
				TEXT_COLOUR(15, 0);
				printf(" ");
				fflush(stdout);
				GSC_DOSMutex.Signal();

				for (double i = 0.0; i < sleeptime; i++) {}
				TEXT_COLOUR(15, 0);
				warningFlag[0] = 1;
			}
			else if (warningFlag[0] == 1)
			{
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 0);
				printf(" ");
				fflush(stdout);
				warningFlag[0] = 0;
				GSC_DOSMutex.Signal();				
			}
		}
		else if (num == 1)
		{
			if (level89 < 200.0) {
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 1);
				TEXT_COLOUR(0, 12);
				printf(" ");
				TEXT_COLOUR(15, 0);
				fflush(stdout);
				GSC_DOSMutex.Signal();

				for (double i = 0.0; i < sleeptime; i++) {}

				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 1);
				TEXT_COLOUR(15, 0);
				printf(" ");
				fflush(stdout);
				GSC_DOSMutex.Signal();

				for (double i = 0.0; i < sleeptime; i++) {}
				TEXT_COLOUR(15, 0);
				warningFlag[1] = 1;
			}
			else if (warningFlag[1] == 1)
			{
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 1);
				printf(" ");
				fflush(stdout);
				warningFlag[1] = 0;
				GSC_DOSMutex.Signal();
			}
		}
		else if (num == 2)
		{
			if (level91 < 200.0) {
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 2);
				TEXT_COLOUR(0, 12);
				printf(" ");
				TEXT_COLOUR(15, 0);
				fflush(stdout);
				GSC_DOSMutex.Signal();

				for (double i = 0.0; i < sleeptime; i++) {}

				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 2);
				TEXT_COLOUR(15, 0);
				printf(" ");
				fflush(stdout);
				GSC_DOSMutex.Signal();

				for (double i = 0.0; i < sleeptime; i++) {}
				TEXT_COLOUR(15, 0);
				warningFlag[2] = 1;
			}
			else if (warningFlag[2] == 1)
			{	
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 2);
				printf(" ");
				fflush(stdout);
				warningFlag[2] = 0;
				GSC_DOSMutex.Signal();
			}
		}
		else
		{
			if (level93 < 200.0) {
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 3);
				TEXT_COLOUR(0, 12);
				printf(" ");
				TEXT_COLOUR(15, 0);
				fflush(stdout);
				GSC_DOSMutex.Signal();

				for (double i = 0.0; i < sleeptime; i++) {}

				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 3);
				TEXT_COLOUR(15, 0);
				printf(" ");
				fflush(stdout);
				GSC_DOSMutex.Signal();
				
				for (double i = 0.0; i < sleeptime; i++) {}
				TEXT_COLOUR(15, 0);
				warningFlag[3] = 1;
			}
			else if (warningFlag[3] == 1)
			{
				GSC_DOSMutex.Wait();
				MOVE_CURSOR(FUEL_LEVEL_POS_X - 2, FUEL_LEVEL_POS_Y + 3);
				printf(" ");
				fflush(stdout);
				warningFlag[3] = 0;
				GSC_DOSMutex.Signal();
			}
		}
	}

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Thread
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Thread to display transaction
UINT __stdcall Transaction(void *args)
{
	CSemaphore DisplayTran("DisplayTran", 0, 1);
	CSemaphore HideTran("HideTran", 0, 1);

	while (1)
	{
		DisplayTran.Wait();

		GSC_DOSMutex.Wait();
		MOVE_CURSOR(0, TRAN_INFO_POS_Y - 4);
		printf("Transaction History");
		fflush(stdout);
		MOVE_CURSOR(0, TRAN_INFO_POS_Y -3);
		printf("---------------------");
		fflush(stdout);
		GSC_DOSMutex.Signal();

		int i = 0;
		for (list<pumpdatapooldata>::iterator it = tranList.begin(); it != tranList.end(); ++it)
		{
			GSC_DOSMutex.Wait();

			// Name
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 0);
			printf("Name: %s %s", names[it->firstNameHash].c_str(), names[it->lastNameHash].c_str());
			fflush(stdout);

			// Print customer credit card number
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 1);
			printf("Credit Card #: %d-%d-%d-%d", it->first4Digit, it->second4Digit,
				it->third4Digit, it->fourth4Digit);
			fflush(stdout);

			// Print fuel grade selected by customer
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 2);
			printf("Fuel Grade: %d", it->fuelGrade);
			fflush(stdout);

			// Print amount of fuel that has been dispensed
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 3);
			printf("Amount dispensed: %.3f liters", it->fuelDispensed);
			fflush(stdout);

			// Print running cost
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 4);
			printf("Cost: $ %.2f", it->fuelCost);
			fflush(stdout);

			GSC_DOSMutex.Signal();

			i++;
		}

		HideTran.Wait();

		GSC_DOSMutex.Wait();
		MOVE_CURSOR(0, TRAN_INFO_POS_Y - 4);
		printf("                         ");
		fflush(stdout);
		MOVE_CURSOR(0, TRAN_INFO_POS_Y - 3);
		printf("                           ");
		fflush(stdout);
		GSC_DOSMutex.Signal();

		i = 0;
		for (list<pumpdatapooldata>::iterator it = tranList.begin(); it != tranList.end(); ++it)
		{
			GSC_DOSMutex.Wait();

			// Name
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 0);
			printf("                                   ");
			fflush(stdout);

			// Print customer credit card number
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 1);
			printf("                                    ");
			fflush(stdout);

			// Print fuel grade selected by customer
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 2);
			printf("                                   ");
			fflush(stdout);

			// Print amount of fuel that has been dispensed
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 3);
			printf("                                   ");
			fflush(stdout);

			// Print running cost
			MOVE_CURSOR(0, TRAN_INFO_POS_Y + i * 6 + 4);
			printf("                                   ");
			fflush(stdout);

			GSC_DOSMutex.Signal();
			i++;
		}
	}

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Thread
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Thread to read keyboard command
UINT __stdcall KeyCommand(void *args)
{
	CSemaphore DisplayTran("DisplayTran", 0, 1);
	CSemaphore HideTran("HideTran", 0, 1); 
	CSemaphore ChangePrices("ChangePrices", 0, 1);

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between GSC and pumps
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	CSemaphore Pump1Response("PumpResponse1", 0, 1);
	CSemaphore Pump2Response("PumpResponse2", 0, 1);
	CSemaphore Pump3Response("PumpResponse3", 0, 1);
	CSemaphore Pump4Response("PumpResponse4", 0, 1);

	CSemaphore Pump1SignalMutex("PumpSignalMutex1", 0, 1);
	CSemaphore Pump2SignalMutex("PumpSignalMutex2", 0, 1);
	CSemaphore Pump3SignalMutex("PumpSignalMutex3", 0, 1);
	CSemaphore Pump4SignalMutex("PumpSignalMutex4", 0, 1);

	// Create fuel price datapool
	CDataPool fpdp("FuelPrice", sizeof(struct fuelprice));
	struct fuelprice *FuelPriceDataPool = (struct fuelprice *)(fpdp.LinkDataPool());

	CMutex fuelPriceMutex("fuelPriceMutex");

	//+++++++++++++++++++++++++++++++++++++++++++++++++++
	// Communication between GSC and fuel tanks
	//+++++++++++++++++++++++++++++++++++++++++++++++++++

	// Create fuel tank datapool
	CDataPool fdp("FuelTank", sizeof(struct fueltank));
	struct fueltank *FuelTankDataPool = (struct fueltank *)(fdp.LinkDataPool());

	CMutex fuelTankMutex("fuelTankMutex");

	CSemaphore Refill87Done("Refill87Done", 0, 1);
	CSemaphore Refill89Done("Refill89Done", 0, 1);
	CSemaphore Refill91Done("Refill91Done", 0, 1);
	CSemaphore Refill93Done("Refill93Done", 0, 1);

	char command1 = '\0';
	char command2 = '\0';
	int refill87Req;
	int refill89Req;
	int refill91Req;
	int refill93Req;
	char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	while (1)
	{
		fuelTankMutex.Wait();
		refill87Req = FuelTankDataPool->refillRequest[0];
		refill89Req = FuelTankDataPool->refillRequest[1];
		refill91Req = FuelTankDataPool->refillRequest[2];
		refill93Req = FuelTankDataPool->refillRequest[3];
		fuelTankMutex.Signal();

		if (TEST_FOR_KEYBOARD() != 0)
		{
			command1 = command2;

			GSC_DOSMutex.Wait();

			TEXT_COLOUR(2, 0);
			MOVE_CURSOR(24, COMMAND_INFO_Y + 12);
			command2 = getchar();
			TEXT_COLOUR(15, 0);
			GSC_DOSMutex.Signal();

			// Signal pumps
			
			if (command1 == commandSignal[1 - 1] && command2 == digits[1])
			{
				Pump1Response.Signal();
				Pump1SignalMutex.Signal();
			}
			if (command1 == commandSignal[2 - 1] && command2 == digits[2])
			{
				Pump2Response.Signal();
				Pump2SignalMutex.Signal();
			}
			if (command1 == commandSignal[3 - 1] && command2 == digits[3])
			{
				Pump3Response.Signal();
				Pump3SignalMutex.Signal();
			}
			if (command1 == commandSignal[4 - 1] && command2 == digits[4])
			{
				Pump4Response.Signal();
				Pump4SignalMutex.Signal();
			}

			// Refill tankss
			if (command1 == commandRefill[1 - 1] && command2 == digits[1])
			{
				fuelTankMutex.Wait();
				FuelTankDataPool->fuel87level = INITIAL_FUEL_AMOUNT;
				fuelTankMutex.Signal();

				Refill87Done.Signal();
			}

			if (command1 == commandRefill[2 - 1] && command2 == digits[2])
			{
				fuelTankMutex.Wait();
				FuelTankDataPool->fuel89level = INITIAL_FUEL_AMOUNT;
				fuelTankMutex.Signal();

				Refill89Done.Signal();
			}

			if (command1 == commandRefill[3 - 1] && command2 == digits[3])
			{
				fuelTankMutex.Wait();
				FuelTankDataPool->fuel91level = INITIAL_FUEL_AMOUNT;
				fuelTankMutex.Signal();

				Refill91Done.Signal();
			}

			if (command1 == commandRefill[4 - 1] && command2 == digits[4])
			{
				fuelTankMutex.Wait();
				FuelTankDataPool->fuel93level = INITIAL_FUEL_AMOUNT;
				fuelTankMutex.Signal();

				Refill93Done.Signal();
			}

			// Transaction
			if (command1 == commandDisplayTran[0] && command2 == commandDisplayTran[1])
			{
				DisplayTran.Signal();
			}
			if (command1 == commandHideTran[0] && command2 == commandHideTran[1])
			{
				HideTran.Signal();
			}

			// Change price
			if (command1 == commandChangeCost[0] && command2 == commandChangeCost[1])
			{
				// enter new prices
				GSC_DOSMutex.Wait();

				fuelPriceMutex.Wait();

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 0);
				printf("Enter new Fuel 87 price: ");
				cin >> FuelPriceDataPool->fuel87Price;
				fflush(stdout);

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 1);
				printf("Enter new Fuel 89 price: ");
				cin >> FuelPriceDataPool->fuel89Price;
				fflush(stdout);

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 2);
				printf("Enter new Fuel 91 price: ");
				cin >> FuelPriceDataPool->fuel91Price;
				fflush(stdout);

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 3);
				printf("Enter new Fuel 93 price: ");
				cin >> FuelPriceDataPool->fuel93Price;
				fflush(stdout);

				fuelPriceMutex.Signal();

				GSC_DOSMutex.Signal();

				// display new prices
				GSC_DOSMutex.Wait();

				fuelPriceMutex.Wait();

				MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y);
				printf("Fuel 87 Price: $%.2f", FuelPriceDataPool->fuel87Price);
				fflush(stdout);

				MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y + 1);
				printf("Fuel 89 Price: $%.2f", FuelPriceDataPool->fuel89Price);
				fflush(stdout);

				MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y + 2);
				printf("Fuel 91 Price: $%.2f", FuelPriceDataPool->fuel91Price);
				fflush(stdout);

				MOVE_CURSOR(PRICE_POS_X, PRICE_POS_Y + 3);
				printf("Fuel 93 Price: $%.2f", FuelPriceDataPool->fuel93Price);
				fflush(stdout);

				fuelPriceMutex.Signal();

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 0);
				printf("                                ");
				fflush(stdout);

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 1);
				printf("                                ");
				fflush(stdout);

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 2);
				printf("                                ");
				fflush(stdout);

				MOVE_CURSOR(0, NEW_PRICE_INFO_POS_Y + 3);
				printf("                                ");
				fflush(stdout);

				GSC_DOSMutex.Signal();
			}
		}
	}

	return 0;
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Main Function
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

int main(void)
{
	// 100 customers, 4 pumps, 1 station computer process
	CRendezvous r("GetReady", rNum);

	TEXT_COLOUR(15, 0);

	// To protect thread creations
	CSemaphore   GSCThreadMutex("GSCThreadMutex", 1);
	
	// Create pump threads
	CThread *Pumps[PUMP_NUMBER];
	int pumpNum;
	for (int i = 0; i < PUMP_NUMBER; i++)
	{
		GSCThreadMutex.Wait(); // Wait here until the previous thread has been completely created
		pumpNum = i + 1;
		Pumps[i] = new CThread(PumpCommunication, ACTIVE, &pumpNum);
	}

	// Create FuelLevelCheck threads
	CThread *LevelCheck[4];
	for (int i = 0; i < 4; i++)
	{
		LevelCheck[i] = new CThread(FuelLevelCheck, ACTIVE, &i);
	}

	// Create KeyCommand threads
	CThread *KeyCmd = new CThread(KeyCommand, ACTIVE, &pumpNum);

	// Create Transaction threads
	CThread *Tran = new CThread(Transaction, ACTIVE, &pumpNum);

	SetFuelPrice();
	DisplayFrame();
	DisplayFuelPrice();
	DisplayFuelLevel();

	for (int i = 0; i < PUMP_NUMBER; i++)
		commandSignal[i] = 'a' + i;

	for (int i = 0; i < 4; i++)
		commandRefill[i] = 'o' + i;

	commandChangeCost[0] = 'c';
	commandChangeCost[1] = 'c';

	commandDisplayTran[0] = 'd';
	commandDisplayTran[1] = 't';

	commandHideTran[0] = 'h';
	commandHideTran[1] = 't';

	r.Wait(); // Rendezvous

	// Wait for thread(s) to terminate
	for (int i = 0; i < PUMP_NUMBER; i++)
		Pumps[i]->WaitForThread();

	for (int i = 0; i < 4; i++)
		LevelCheck[i]->WaitForThread();

	KeyCmd->WaitForThread();
	Tran->WaitForThread();

	getchar();
	getchar();

	return 0;
}


