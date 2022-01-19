#pragma once
#include "museInterface.h"

/*
- can be implemented identically to museInterface
- simulates headset's live data with csv reading 
  of a recorded session.
*/
class MuseInterfaceCSV :
	public museInterface
{
public:

	MuseInterfaceCSV();
	~MuseInterfaceCSV() override;

	//load a custom csv file:
	void loadCSV(string filename);

	void start() override;


private:
	string _filename;

	//recieves messages by reading the CSV file according to time intervals 
	void recieveMessage() override;

	std::thread* readThread = nullptr;

	long int getInMillisecondsSinceMidnight(string & timeStampStr);

	bool validNumericalData = false;

};
