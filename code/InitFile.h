#ifndef _InitFile_
#define _InitFile_

#include <stdio.h>
#include <string>
#include <iostream>
#include "Customer.h"
#include "Pump.h"
#include "InitFile.h"
#include "..\rt.h"

//***************************************************************
//						INSTRUCTION								|
//***************************************************************
//																|
//	1. Build GSC project;										|
//	2. Change the GSCProcessDir to the directory of GSC.exe;	|
//	3. Play with different number of customers and pumps;		|
//			- Change CUSTOMER_NUMBER or PUMP_NUMBER				|
//	4. Rebuild GSC project;										|
//	5. Run the program;											|
//	6. Enter the initial prices for all grades of fuel,			|
//		and then hit ENTER;										|
//	7. Follow the instructions on GSC display window;			|
//	8. Have fun.												|
//																|
//***************************************************************

//--------------------
// Setup here
//--------------------

#define CUSTOMER_NUMBER 100
#define PUMP_NUMBER 4				// default is 4
#define HALF_LITER_PER_X_SECOND 100 // default is 1000, meaning dispense fuel at the rate of 0.5 liter per second
#define INITIAL_FUEL_AMOUNT 210		// Initial fuel amount, default is 500
string GSCProcessDir = "Z:\\CPEN333\\Assignment1\\GasStation\\Debug\\GSC.exe";

//--------------------------
// Please do NOT change
//--------------------------

#define NAME_SPACE 50
#define WINDOW_WIDTH 72
#define PUMP_FRAME_X 3
#define PUMP_FRAME_Y 1

string names[NAME_SPACE] = { "Lian","Noah","Will","James","Logan","Ben","Elijah","Oliver","Jacob","Lucas",
							"Michael","Alex","Ethan","Daniel","Matthew","Aiden","Henry","Joseph","Jackson","David",
							"Luke","Gabriel","Anthony","Isaac","Grayson","Jack","Jlian","Levi","Joshua","Andrew",
							"Lincoln","Mateo","Ryan","Jaxon","Nathan","Aaron","Isaiah","Thomas","Charles","Caleb",
							"Josiah","Christian","Hunter","Eli","Jon","Connor","Adrian","Asher","Cameron","Leo" };

int rNum = CUSTOMER_NUMBER + PUMP_NUMBER + 1; // Rendezvous wait number

// Struct for datapool between pumps and GSC
struct pumpdatapooldata {
	// Customer name
	int lastNameHash;
	int firstNameHash;
	// Customer credit card
	int first4Digit;
	int second4Digit;
	int third4Digit;
	int fourth4Digit;
	// Fuel
	int fuelGrade;
	// Pump
	int pumpNum;
	int PumpSignal;
	double fuelCost;
	double fuelDispensed;
	int pumpdone[PUMP_NUMBER];
};

// Struct for fuel tank datapool
struct fueltank {
	double fuel87level;
	double fuel89level;
	double fuel91level;
	double fuel93level;
	int refillRequest[4];
};

// Struct for pipe between pumps and customer
struct customerdata {
	// Customer name
	int lastNameHash;
	int firstNameHash;
	// Customer credit card
	int first4Digit;
	int second4Digit;
	int third4Digit;
	int fourth4Digit;
	// Fuel
	int fuelGrade;
	double fuelNeeded;
	// Pump
	int pumpNum;
};

// Struct for fuel price datapool
struct fuelprice {
	double fuel87Price;
	double fuel89Price;
	double fuel91Price;
	double fuel93Price;
};

#endif