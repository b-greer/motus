#include "MuseInterfaceCSV.h"


MuseInterfaceCSV::MuseInterfaceCSV()
{
	_filename = "data/data.csv";
}

MuseInterfaceCSV::~MuseInterfaceCSV() {
}

void MuseInterfaceCSV::loadCSV(string filename)
{
	_filename = filename;
}

void MuseInterfaceCSV::start()
{
	if (!readThread) {
		threadRunning = true;
		readThread = new std::thread(&MuseInterfaceCSV::recieveMessage, this);
	}
}

void MuseInterfaceCSV::recieveMessage()
{
	ifstream csvFile;
	//string strPathCSVFile = "data/data.csv";
	csvFile.open(_filename.c_str(), std::ifstream::in);

	string line;
	getline(csvFile, line); // skip the 1st line containing headers

	long int prevTimeStamp = -1;
	int timerMillis = -1;

	//Just to correctly initialise chrono structs, these particular assignments don't actually get used
	auto end = chrono::high_resolution_clock::now();
	auto begin = chrono::high_resolution_clock::now();
	auto dur = end - begin;

	validNumericalData = false;
	vector <long double> numerical_vals;
	for (int i = 0; i < 37; i++)
	{
		numerical_vals.push_back(0);
	}


	while (getline(csvFile, line))
	{
		if (line.empty()) { continue; }

		istringstream iss(line);
		string lineStream;
		string::size_type sz;

		getline(iss, lineStream, ',');
		string timeStampStr(lineStream);
		long int timeStamp = getInMillisecondsSinceMidnight(timeStampStr);

		for (int i = 0; i < 37; i++)
		{
			getline(iss, lineStream, ',');
			if (!lineStream.empty()) {
				numerical_vals[i] = stold(lineStream, &sz); // convert to double
				validNumericalData = true;
			}
		}

		getline(iss, lineStream, ',');
		string elements(lineStream);

		/* HERE wait for (timer > (timeStamp - prevTimeStamp)), then reset timer and
		 * move the new values into the variables
		 */
		do {
			end = chrono::high_resolution_clock::now();
			dur = end - begin;
			timerMillis = chrono::duration_cast<chrono::milliseconds>(dur).count();
		} while (prevTimeStamp != -1 && timerMillis < (timeStamp - prevTimeStamp));

		prevTimeStamp = timeStamp;
		begin = chrono::high_resolution_clock::now();


		// assign variables with data read from CSV line

	//------DELTA:
		//values for invividual sensors:
		delta_sensor_values[0] = numerical_vals[0];
		delta_sensor_values[1] = numerical_vals[1];
		delta_sensor_values[2] = numerical_vals[2];
		delta_sensor_values[3] = numerical_vals[3];
		delta = (numerical_vals[0] + numerical_vals[1] + numerical_vals[2] + numerical_vals[3]) / 4;
		
		updateBaseline(delta, deltaVals, bl_average_delta, bl_lowest_delta, bl_highest_delta);


	//------THETA:
		theta_sensor_values[0] = numerical_vals[4];
		theta_sensor_values[1] = numerical_vals[5];
		theta_sensor_values[2] = numerical_vals[6];
		theta_sensor_values[3] = numerical_vals[7];
		theta = (numerical_vals[4] + numerical_vals[5] + numerical_vals[6] + numerical_vals[7]) / 4;
		
		updateBaseline(theta, thetaVals, bl_average_theta, bl_lowest_theta, bl_highest_theta);


	//--------ALPHA:
		alpha_sensor_values[0] = numerical_vals[8];
		alpha_sensor_values[1] = numerical_vals[9];
		alpha_sensor_values[2] = numerical_vals[10];
		alpha_sensor_values[3] = numerical_vals[11];
		alpha = (numerical_vals[8] + numerical_vals[9] + numerical_vals[10] + numerical_vals[11]) / 4;
		
		updateBaseline(alpha, alphaVals, bl_average_alpha, bl_lowest_alpha, bl_highest_alpha);


	//---------BETA:
		beta_sensor_values[0] = numerical_vals[12];
		beta_sensor_values[1] = numerical_vals[13];
		beta_sensor_values[2] = numerical_vals[14];
		beta_sensor_values[3] = numerical_vals[15];
		beta = (numerical_vals[12] + numerical_vals[13] + numerical_vals[14] + numerical_vals[15]) / 4;
		
		updateBaseline(beta, betaVals, bl_average_beta, bl_lowest_beta, bl_highest_beta);


	//-------GAMMA:
		gamma_sensor_values[0] = numerical_vals[16];
		gamma_sensor_values[1] = numerical_vals[17];
		gamma_sensor_values[2] = numerical_vals[18];
		gamma_sensor_values[3] = numerical_vals[19];
		gamma = (numerical_vals[16] + numerical_vals[17] + numerical_vals[18] + numerical_vals[19]) / 4;
		
		updateBaseline(gamma, gammaVals, bl_average_gamma, bl_lowest_gamma, bl_highest_gamma);

		//update interpreted values (when new brainwave frequency band is recieved)
		updateInterpretedValues();


		raw[0] = numerical_vals[20];
		raw[1] = numerical_vals[21];
		raw[2] = numerical_vals[22];
		raw[3] = numerical_vals[23];

		accelerometer = glm::vec3(numerical_vals[25], numerical_vals[26], numerical_vals[27]);
		gyroscope = glm::vec3(numerical_vals[28], numerical_vals[29], numerical_vals[30]);

		TouchingForehead = (int)numerical_vals[31];

		signalQuality[0] = numerical_vals[32];
		signalQuality[1] = numerical_vals[33];
		signalQuality[2] = numerical_vals[34];
		signalQuality[3] = numerical_vals[35];

	}

	csvFile.close();
	validNumericalData = false;
	threadRunning = false;

}

long int MuseInterfaceCSV::getInMillisecondsSinceMidnight(string& timeStampStr) {
	long y, m, d, h, min, sec, microsec;
	char ampm[3];
	sscanf(timeStampStr.c_str(), "%ld-%ld-%ld %ld:%ld:%ld.%ld %s", &y, &m, &d, &h, &min, &sec, &microsec, ampm);
	if (strcmp("pm", ampm) == 0) {
		h += 12;
	}
	return h*3600000 + min*60000 + sec*1000 + microsec;
}

