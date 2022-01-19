#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "museInterface.h"
#include "MuseInterfaceCSV.h"

using namespace glm;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void sphereInit(ofMesh& myMesh, float total);
		void sphereUpdate(ofMesh& myMesh, float total);

		float supershape(float theta, float m, float n1, float n2, float n3);

		float a, b, r, r1, r2, m, n1, n2, n3, lon, lat, x, y, z;

		float mChange, mOff;

		int total, t;

		int index;
		
		vec3 globe[401][401];
		int indies[320800];

		ofEasyCam myCam;
		ofMesh myMesh;
		ofLight light;
		
		ofxPanel panel;

		ofxFloatSlider aSlide; 
		ofxFloatSlider bSlide;
		ofxFloatSlider mSlide;
		ofxFloatSlider n1Slide;
		ofxFloatSlider n2Slide;
		ofxFloatSlider n3Slide;

		ofxFloatSlider ambRed;
		ofxFloatSlider ambGreen;
		ofxFloatSlider ambBlue;


		MuseInterfaceCSV headset;
		//live headset:
		//museInterface headset;

		vec3 headRotation;


		//phong shader stuff
		ofShader geoShader;

		ofFloatColor matAmbient;
		ofFloatColor matDiffuse;
		ofFloatColor matSpecular;
		float matShininess;
		vec4 lightPosition;
		ofFloatColor lightColor;

		ofImage environmentMap;
		float matReflectivity;
		float matRefractiveIndex;

};
