#ifndef _Customer_
#define _Customer_

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include "Pump.h"
#include "..\rt.h"
#include "InitFile.h"
#include <time.h>

int CPUMP_FRAME_INTERVAL = 16;
int CPUMP_POS_X = 3;
int CPUMP_POS_Y = PUMP_FRAME_Y + 7;

class Customer :public ActiveClass
{
private:
	// Customer ID
	int customerNum;
	string name;
	int lastNameHash;
	int firstNameHash;

	// Credit card
	string CreditCardNum;
	int first4Digit;
	int second4Digit;
	int third4Digit;
	int fourth4Digit;

	// Fuel information
	int fuelGrade;
	double fuelNeeded;

public:
	Customer(int num);

private:
	int main(void);
};

#endif