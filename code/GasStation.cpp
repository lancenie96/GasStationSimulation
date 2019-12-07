#include "Customer.h"
#include "Pump.h"
#include "InitFile.h"
#include "..\rt.h"

int main()
{
	// rNum = # of customers + # of pumps + 1 station computer process
	CRendezvous r("GetReady", rNum);

	TEXT_COLOUR(15, 0);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Initialize fuel tank
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	// Create fuel tank datapool
	CDataPool fdp("FuelTank", sizeof(struct fueltank));
	struct fueltank *FuelTankDataPool = (struct fueltank *)(fdp.LinkDataPool());

	FuelTankDataPool->fuel87level = INITIAL_FUEL_AMOUNT;
	FuelTankDataPool->fuel89level = INITIAL_FUEL_AMOUNT;
	FuelTankDataPool->fuel91level = INITIAL_FUEL_AMOUNT;
	FuelTankDataPool->fuel93level = INITIAL_FUEL_AMOUNT;
	for (int i = 0; i < 4; i++)
		FuelTankDataPool->refillRequest[i] = 0;

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Create pumps
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	Pump *pump[PUMP_NUMBER];
	for (int i = 0; i < PUMP_NUMBER; i++)
	{
		pump[i] = new Pump(i + 1);
		pump[i]->Resume();
	}

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Create the process of gas station computer (GSC)
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	CProcess GSC(
		GSCProcessDir,
		NORMAL_PRIORITY_CLASS,
		OWN_WINDOW,
		ACTIVE
	);

	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	// Create customers
	//++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	Customer *customer[CUSTOMER_NUMBER];
	for (int i = 0; i < CUSTOMER_NUMBER; i++)
	{
		customer[i] = new Customer(i);
		customer[i]->Resume();
	}

	// Wait for process(es) and thread(s) to terminate
	GSC.WaitForProcess();

	for (int i = 0; i < CUSTOMER_NUMBER; i++)
		customer[i]->WaitForThread();

	for (int i = 0; i < PUMP_NUMBER; i++)
		pump[i]->WaitForThread();

	system("PAUSE");
	return 0;
}