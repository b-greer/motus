#include "museInterface.h"

museInterface::museInterface() : signalQuality(4), raw(4),
								 delta_sensor_values(4), theta_sensor_values(4), alpha_sensor_values(4), beta_sensor_values(4), gamma_sensor_values(4)
{
	//give port number to the OSC reciever:
	reciever.setup(PORT);

	threadRunning = false;

	usingBaseLine = false;

}

museInterface::~museInterface()
{
	stop();
}

void museInterface::start()
{
	if (!readThread) {
		threadRunning = true;
		readThread = new std::thread(&museInterface::recieveMessage, this);
	}
	
}

void museInterface::stop()
{
	if (readThread) {
		threadRunning = false;
		readThread->join();
		delete readThread;
	}
}


float museInterface::getMood()
{
	return mood;
}

float museInterface::getMeditation()
{
	return meditation;
}

float museInterface::getAttention()
{
	return attention;
}

vector<float> museInterface::getSignalQuality()
{
	return signalQuality;
}

vector<float> museInterface::getRaw()
{
	return raw;
}

float museInterface::getDelta()
{
	return delta;
}

float museInterface::getTheta()
{
	return theta;
}

float museInterface::getAlpha()
{
	return alpha;
}

float museInterface::getBeta()
{
	return beta;
}

float museInterface::getGamma()
{
	return gamma;
}

float museInterface::getDelta_relative()
{
	return ( pow(10, delta) / (pow(10, delta) + pow(10, theta) + pow(10, alpha) + pow(10,beta) + pow(10, gamma) ) );
}

float museInterface::getTheta_relative()
{
	return (pow(10, theta) / (pow(10, delta) + pow(10, theta) + pow(10, alpha) + pow(10, beta) + pow(10, gamma)));
}

float museInterface::getAlpha_relative()
{
	return (pow(10, alpha) / (pow(10, delta) + pow(10, theta) + pow(10, alpha) + pow(10, beta) + pow(10, gamma)));
}

float museInterface::getBeta_relative()
{
	return (pow(10, beta) / (pow(10, delta) + pow(10, theta) + pow(10, alpha) + pow(10, beta) + pow(10, gamma)));
}

float museInterface::getGamma_relative()
{
	return (pow(10, gamma) / (pow(10, delta) + pow(10, theta) + pow(10, alpha) + pow(10, beta) + pow(10, gamma)));
}

glm::vec3 museInterface::getGyroscope()
{
	return gyroscope;
}

glm::vec3 museInterface::getAccelerometer()
{
	return accelerometer;
}

bool museInterface::getBlink()
{
	return (ofGetElapsedTimef() - blink) < 0.1;
}

bool museInterface::getJawClench()
{
	return (ofGetElapsedTimef() - jawClench) < 0.1;
}

float museInterface::getEntropy()
{
	//standard deviation of eeg values:
	return entropy;
}

void museInterface::drawHorshoe(glm::vec3 position)
{
	ofPushStyle();

	float radius = 10;
	ofSetColor(255, 150, 0, ofMap(signalQuality[0], 1, 4, 255, 0));
	ofFill();
	if (signalQuality[0] == 1) radius *= 1.5;
	ofDrawCircle(position, radius);

	radius = 10;
	ofSetColor(0, 200, 120, ofMap(signalQuality[1], 1, 4, 255, 0));
	ofFill();
	if (signalQuality[1] == 1) radius *= 1.5;
	ofDrawCircle(position + glm::vec3(40, 0, 0), radius);

	radius = 15;
	ofSetColor(0, 0, 0, ofMap(TouchingForehead, 0, 1, 0, 255));
	ofFill();
	ofDrawCircle(position + glm::vec3(80, 0, 0), radius);
	
	
	radius = 10;
	ofSetColor(170, 0, 255, ofMap(signalQuality[2], 1, 4, 255, 0));
	ofFill();
	if (signalQuality[2] == 1) radius *= 1.5;
	ofDrawCircle(position + glm::vec3(120, 0, 0), radius);

	radius = 10;
	ofSetColor(255, 0, 190, ofMap(signalQuality[3], 1, 4, 255, 0));
	ofFill();
	if (signalQuality[3] == 1) radius *= 1.5;
	ofDrawCircle(position + glm::vec3(160, 0, 0), radius);

	ofPopStyle();
}
	

void museInterface::recieveMessage()
{
	while (threadRunning) {

		while (reciever.hasWaitingMessages()) {

			ofxOscMessage m;
			reciever.getNextMessage(m);

			if (m.getAddress() == "/muse/elements/horseshoe") {
				signalQuality[0] = m.getArgAsFloat(0);
				signalQuality[1] = m.getArgAsFloat(1);
				signalQuality[2] = m.getArgAsFloat(2);
				signalQuality[3] = m.getArgAsFloat(3);
			}

			if (m.getAddress() == "/muse/elements/touching_forehead") {
				TouchingForehead = m.getArgAsBool(0);
			}

			if (m.getAddress() == "/muse/eeg") {
				raw[0] = m.getArgAsFloat(0);
				raw[1] = m.getArgAsFloat(1);
				raw[2] = m.getArgAsFloat(2);
				raw[3] = m.getArgAsFloat(3);
			}

			

			if (m.getAddress() == "/muse/elements/delta_absolute") {

				//values for invividual sensors:
				delta_sensor_values[0] = m.getArgAsFloat(0);
				delta_sensor_values[1] = m.getArgAsFloat(1);
				delta_sensor_values[2] = m.getArgAsFloat(2);
				delta_sensor_values[3] = m.getArgAsFloat(3);

				//average:
				if (TouchingForehead) {
					delta = (delta_sensor_values[0] + delta_sensor_values[1] + delta_sensor_values[2] + delta_sensor_values[3]) / 4;

					updateBaseline(delta, deltaVals, bl_average_delta, bl_lowest_delta, bl_highest_delta);
				}
				else {
					delta = 0;
				}
				
				
			}
			if (m.getAddress() == "/muse/elements/theta_absolute") {
				
				//values for invividual sensors:
				theta_sensor_values[0] = m.getArgAsFloat(0);
				theta_sensor_values[1] = m.getArgAsFloat(1);
				theta_sensor_values[2] = m.getArgAsFloat(2);
				theta_sensor_values[3] = m.getArgAsFloat(3);

				if (TouchingForehead) {
					//average:
					theta = (theta_sensor_values[0] + theta_sensor_values[1] + theta_sensor_values[2] + theta_sensor_values[3]) / 4;

					updateBaseline(theta, thetaVals, bl_average_theta, bl_lowest_theta, bl_highest_theta);
				}
				else {
					theta = 0;
				}

			}
			if (m.getAddress() == "/muse/elements/beta_absolute") {

				//values for invividual sensors:
				beta_sensor_values[0] = m.getArgAsFloat(0);
				beta_sensor_values[1] = m.getArgAsFloat(1);
				beta_sensor_values[2] = m.getArgAsFloat(2);
				beta_sensor_values[3] = m.getArgAsFloat(3);

				if (TouchingForehead) {
					//average:
					beta = (beta_sensor_values[0] + beta_sensor_values[1] + beta_sensor_values[2] + beta_sensor_values[3]) / 4;

					updateBaseline(beta, betaVals, bl_average_beta, bl_lowest_beta, bl_highest_beta);
				}
				else {
					beta = 0;
				}

			}
			if (m.getAddress() == "/muse/elements/alpha_absolute") {

				//values for invividual sensors:
				alpha_sensor_values[0] = m.getArgAsFloat(0);
				alpha_sensor_values[1] = m.getArgAsFloat(1);
				alpha_sensor_values[2] = m.getArgAsFloat(2);
				alpha_sensor_values[3] = m.getArgAsFloat(3);

				if (TouchingForehead) {
					//average:
					alpha = (alpha_sensor_values[0] + alpha_sensor_values[1] + alpha_sensor_values[2] + alpha_sensor_values[3]) / 4;

					updateBaseline(alpha, alphaVals, bl_average_alpha, bl_lowest_alpha, bl_highest_alpha);
				}
				else {
					alpha = 0;
				}

			}
			if (m.getAddress() == "/muse/elements/gamma_absolute") {

				//values for invividual sensors:
				gamma_sensor_values[0] = m.getArgAsFloat(0);
				gamma_sensor_values[1] = m.getArgAsFloat(1);
				gamma_sensor_values[2] = m.getArgAsFloat(2);
				gamma_sensor_values[3] = m.getArgAsFloat(3);

				if (TouchingForehead) {
					//average:
					gamma = (gamma_sensor_values[0] + gamma_sensor_values[1] + gamma_sensor_values[2] + gamma_sensor_values[3]) / 4;

					updateBaseline(gamma, gammaVals, bl_average_gamma, bl_lowest_gamma, bl_highest_gamma);

					//update interpreted values (when new brainwave frequency band is recieved)
					updateInterpretedValues();
			
				}
				else {
					gamma = 0;
				}
				
			}

			
			if (m.getAddress() == "/muse/elements/blink") {
				if (m.getArgAsFloat(0) == 1) {
					blink = ofGetElapsedTimef();
				}
			}

			if (m.getAddress() == "/muse/elements/jaw_clench") {
				jawClench = ofGetElapsedTimef();
			}


			if (m.getAddress() == "/muse/gyro") {
				gyroscope = glm::vec3(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
			}

			if (m.getAddress() == "/muse/acc") {
				accelerometer = glm::vec3(m.getArgAsFloat(0), m.getArgAsFloat(1), m.getArgAsFloat(2));
			}
		}
	}
}


//--------------------------------------------------------------
float museInterface::updateAttention()
{
//relative to baseline
	/*
	float relative_gamma = ofMap(gamma, bl_lowest_gamma, bl_highest_gamma, -1, 1, true);
	float relative_beta = ofMap(beta, bl_lowest_beta, bl_highest_beta, -1, 1, true);
	float relative_alpha = ofMap(alpha, bl_lowest_alpha, bl_highest_alpha, 1, -1, true);
	//*/
//----
	
	//*
	//relative to all other frequency bands:
	//seems to work better than above
	float relative_beta = ofMap(getBeta_relative(), 0.f, 0.2f, -1, 1);
	float relative_gamma = ofMap(getGamma_relative(), 0.f, 0.2f, -1, 1);
	float relative_alpha = ofMap(getAlpha_relative(), 0.f, 0.2f, 1, -1);
	//*/

	float gamma_weight = 0.8;
	float beta_weight = 0.4;
	float alpha_weight = 0.2; //take negative alpha
	float my_attention = (relative_gamma * gamma_weight) + (relative_beta * beta_weight) + (relative_alpha * alpha_weight);
	//float attention = relative_gamma;
	//cout << "attention: " << my_attention << endl; // ", relative_gamma: " << relative_gamma << ", relative_beta: " << relative_beta << endl;
	return my_attention;
}

//--------------------------------------------------------------
float museInterface::updateMeditation()
{
	//*
	//relative to baseline

	float relative_alpha = ofMap(getAlpha_relative(), 0.f, 0.4f, -1, 1);
	float relative_theta = ofMap(getTheta_relative(), 0.f, 0.2f, -1, 1);

	//float relative_alpha = ofMap(alpha, bl_lowest_alpha, bl_highest_alpha, -1, 1, true);
	//float relative_theta = ofMap(theta, bl_lowest_theta, bl_highest_theta, -1, 1, true);
	float inverse_attention = ofMap(attention, -1.0, 1.0, 1, -1, true);
	float inverse_entropy = ofMap(entropy, 0.0, 0.5, 1, -1, true);
	
	//*/
	//cout << "relative alpha: " << getAlpha_relative() << "relative theta: " << getTheta_relative() << endl;

	//relative to all frequency bands
	//relative_alpha = ofMap(getAlpha_relative(), 0.f, 0.3f, -1, 1, true);
	//relative_alpha = getAlpha_relative();

	float my_meditation = (inverse_attention* 0.0) + (relative_alpha * 0.25) + (relative_theta * 0.25) + (inverse_entropy*0.5);
	
	//cout << "meditation: " << my_meditation << ", entropy: " << entropy << endl;

	return my_meditation;
}

//--------------------------------------------------------------
float museInterface::updateMood()
{
	float left_alpha = alpha_sensor_values[1];
	float right_alpha = alpha_sensor_values[2];

	/*frontal Asymmetry:
		ln(10^right_alpha / 10^left_alpha)
		= ln(10^(right_alpha - left_alpha)
		= (right_alpha - left_alpha) * ln(10)
		ln(10) = 2.302585
	*/
	float f_a = (right_alpha - left_alpha)*2.302585;
	//map from min and max to -1 and 1:
	//ln(100) = 4.605170186;
	//ln(1/100) = -4.605170186;
	//left > right = positive mood = 1
	//right > left = negative mood = -1
	f_a = ofMap(f_a, -4.605170186, 4.605170186, 1, -1, true);
	//cout << "mood: " << f_a << endl;
	return f_a;
}

//--------------------------------------------------------------
float museInterface::updateEnropy()
{
	//standard deviation of eeg values:
	vector<float> values;
	values.push_back(delta);
	values.push_back(theta);
	values.push_back(alpha);
	values.push_back(beta);
	values.push_back(gamma);

	float standardDeviation = 0.0;
	float sum = 0.0;

	for (int i = 0; i < values.size(); ++i)
	{
		sum += values[i];
	}

	float mean = sum / values.size();

	for (int i = 0; i < values.size(); ++i) {
		standardDeviation += pow(values[i] - mean, 2);
	}

	return sqrt(standardDeviation / values.size());
}

//--------------------------------------------------------------
void museInterface::updateInterpretedValues()
{
	if (signalQuality[1] != 4 || signalQuality[2] != 4) {
		//only update mood if signal is good
		mood = updateMood();
	}
	attention = updateAttention();
	meditation = updateMeditation();
	entropy = updateEnropy();

	//update vectors storing past interpreted values:
	moodVals.push_back(mood);
	if (moodVals.size() >= baseline_history_length) moodVals.pop_front();
	

	attentionVals.push_back(attention);
	if (attentionVals.size() >= baseline_history_length) attentionVals.pop_front();


	meditationVals.push_back(meditation);
	if (meditationVals.size() >= baseline_history_length) meditationVals.pop_front();

}


//--------------------------------------------------------------
void museInterface::updateBaseline(float newVal, deque<float> &vals, float &baseLineAverage, float &baseLineLowest, float &baseLineHighest)
{
	vals.push_back(newVal);

	if (vals.size() >= baseline_history_length) vals.pop_front();
	/*
	brainWave_count++;

	if (brainWave_count == 1) {
		//first value
		baseLineAverage = newValue;
	}
	else {
		//moving average:
		baseLineAverage = baseLineAverage + ((newValue - baseLineAverage) / brainWave_count);
	}
	*/
	

	if (vals.size() < baseline_history_length) {
		float sum = 0;
		for (float val : vals) {
			sum += val;
		}
		baseLineAverage = sum / (float)vals.size();
	}
	else {
		//cout << "reached history length" << endl;
		float sum = 0;
		for (auto val = vals.rbegin(); val != vals.rbegin() + (baseline_history_length); val++) {
			sum += *val;
		}
		baseLineAverage = sum / baseline_history_length;
	}

	//update lowest and highest:
	//find lowest:
	if (vals.back() < baseLineLowest) {
		baseLineLowest = vals.back();
	}
	//find highest:
	if (vals.back() > baseLineHighest) {
		baseLineHighest = vals.back();
	}
	
}
