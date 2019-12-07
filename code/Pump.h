#ifndef _Pump_
#define _Pump_

#include <stdio.h>
#include <string>
#include <iostream>
#include <algorithm>
#include "Customer.h"
#include "Pump.h"
#include "InitFile.h"
#include "..\RTLibrary\rt.h"

using namespace std;

#define PUMP_FRAME_INTERVAL 15

int PUMP_POS_X = 3;
int PUMP_POS_Y = PUMP_FRAME_Y + 7;

class Pump : public ActiveClass
{
private:
	int pumpNum;
	void DisplayPumpFrame();

public:
	Pump(int num);

private:
	int main(void);
};

#endif
