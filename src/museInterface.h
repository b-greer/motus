#pragma once

#include <time.h>
#include "ofMain.h"

#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")

#include "ofxOsc.h"

#define PORT 7000


/*
	Recieves messages sent from the Mind Monitor App via OSC 
	(The Mind Monitor app recieves data from the Muse headset via blutooth 
	and does some processing of the RAW EEG data to produce the brainwave bands: delta, theta, alpha, beta and gamma) 
*/
class museInterface{
public:
	museInterface();
	virtual ~museInterface();

	ofxOscReceiver reciever;

	/*creates thread for revieving OSC messages from Mind monitor*/
	virtual void start();

	/*stops thread that reieves messages*/
	void stop();


	vector<float> getRaw();

	
	float getDelta();
	float getTheta();
	float getAlpha();
	float getBeta();
	float getGamma();

	float getDelta_relative();
	float getTheta_relative();
	float getAlpha_relative();
	float getBeta_relative();
	float getGamma_relative();

	glm::vec3 getGyroscope();     
	glm::vec3 getAccelerometer();

	/*! blink detection is unreliable: */
	bool getBlink();
	bool getJawClench();

//----interpreted values stuff:

	/*
	returns standard deviation of EEG brainwaves
	between 0 and 1
	A low value indicates meditation/concentration 
	A higher value indicates an unfocused/busy mind:
	*/
	float getEntropy();

	/*returns mood as a scale from -1. - 1.
	-1 = negative mood
	 1 = positive mood
	 0 = neutral
	*/
	float getMood();

	/*
	> (currently not working well) <
	returns between -1 and 1
	*/
	float getMeditation();

	/*
	returns returns between -1 and 1
	*/
	float getAttention();

	//stores hisotry of interpereted values:
	deque<float> moodVals, attentionVals, meditationVals;
//----------


	/*returns signal quality of each electrode sensor as a vector
	[0] = TP9, [1] = AF7, [2] = AF8, [3] = TP10
	*/
	vector<float> getSignalQuality();

	/*
	draws signal quality of each electrode and whether headset is toutching forehead
	(similar to horshoe used by Muse)
	- middle black circle appears when headset is touching forehead
	- coloured circles either side correlate with electrode placement 
	which appear big when signal is best and smaller when signal not perfect
	- circles dissapear completely when there is no signal
	*/
	void drawHorshoe(glm::vec3 position);


//------baseline stuff:
	bool usingBaseLine;

	float bl_average_delta, bl_average_theta, bl_average_alpha, bl_average_beta, bl_average_gamma;
	float bl_lowest_delta = 100, bl_lowest_theta = 100, bl_lowest_alpha = 100, bl_lowest_beta = 100, bl_lowest_gamma = 100;
	float bl_highest_delta = -100, bl_highest_theta = -100, bl_highest_alpha = -100, bl_highest_beta = -100, bl_highest_gamma = -100;

	/*Stores history of past brainwave values:*/
	deque<float> deltaVals, thetaVals, alphaVals, betaVals, gammaVals;

//------------------------------


protected:

	vector<float> raw;

	/*vectors for storing individual values from each sensor:
	[0] = TP9, [1] = AF7, [2] = AF8, [3] = TP10
	*/
	vector<float> delta_sensor_values, theta_sensor_values, alpha_sensor_values, beta_sensor_values, gamma_sensor_values;

	//average value of the sensors:
	float delta, theta, alpha, beta, gamma;

	float blink;
	float jawClench;

//interpreted values:
	float attention, meditation, mood, entropy;

//calculations for interpreted values:
	float updateAttention();
	float updateMeditation();
	float updateMood();
	float updateEnropy();

	//used to update all the above at once:
	void updateInterpretedValues();


	glm::vec3 gyroscope;
	glm::vec3 accelerometer;

	vector<float> signalQuality;
	bool TouchingForehead;


	/*Updates the baseline for an individual brainwave band
	vals = it's history of values e.g. deltaVals
	keeps a moving average up to a defined "history length" by baseline_history_length:
	*/
	void updateBaseline(float newVal, deque<float> &vals, float &baseLineAverage, float &baseLineLowest, float &baseLineHighest);
	float baseline_history_length = 500;


	//multithreading stuff:
	//so that messages can always be recieved without holding up or being help up by other processes

	virtual void recieveMessage();

	std::thread* readThread = nullptr;

	bool threadRunning;

};

