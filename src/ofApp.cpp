#include "ofApp.h"
#include <iostream>
//--------------------------------------------------------------
void ofApp::setup(){

	ofSetBackgroundColor(0);

	ofEnableNormalizedTexCoords();
	ofDisableArbTex();

//shader settings:
	geoShader.load("vert.glsl", "frag.glsl", "geom.glsl");


	matAmbient = ofFloatColor(0.1, 0.1, 0.2);
	matDiffuse = ofFloatColor(0.2, 0.5, 0.7);
	matSpecular = ofFloatColor(3.5, 4.0, 5.0);
	matShininess = 500.0f;
	lightPosition = vec4(10.0, 10.0, 20.0, 1.0);
	lightColor = ofFloatColor(1.0, 1.0, 0.8);

	matReflectivity = 0.7f;
	matRefractiveIndex = 1.333f;

	environmentMap.load("hdri_hub_environmentMap.jpg");
//


	ofEnableAlphaBlending();
	ofEnableSmoothing();
	
	ofSetSmoothLighting(true);


//ofLight settings:
	ofSetGlobalAmbientColor(ofColor(100));
	//light.setAmbientColor(ofFloatColor(0.3, 0.3, 0.3));
	light.setDiffuseColor(ofColor(255.0, 255.0, 255.0));
	light.setPointLight();
	light.setPosition(vec3(100.0, 300.0, 300.0));
	light.lookAt(vec3(0, 0, 0));
//

//supershape settings:
	r = 100;
	total = 200;
	t = 0;
	index = -1;

	mOff = 0;
	
	
//GUI:
	panel.setup(); 

	panel.add( aSlide.setup("a", 1, 0, 2));;
	panel.add( bSlide.setup("b", 1, 0, 2));;
	panel.add( mSlide.setup("m", 5, 0, 10));;
	panel.add( n1Slide.setup("n1", 0.2, 0.2, 50));;
	panel.add( n2Slide.setup("n2", 1.7, 0.2, 50));;
	panel.add( n3Slide.setup("n3", 1.7, 0.2, 20));;
//

	//val, min, max
	panel.add(ambRed.setup("r", 0.1, 0, 1));;
	panel.add(ambGreen.setup("g", 0.2, 0, 1));;
	panel.add(ambBlue.setup("b", 0.2, 0, 1));;


	//initialize mesh:
	myMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	sphereInit(myMesh, total);


	//start muse headset communication:
	headset.start();

	headRotation = vec3(0, 0, 0);

}


void ofApp::sphereInit(ofMesh& myMesh, float total)
{
	for (int i = 0; i < total + 1; ++i)
	{
			lat = ofMap(i, 0, total, -HALF_PI, HALF_PI);
			r2 = supershape(lat, mSlide , n1Slide, n2Slide, n3Slide);

			//r2 = supershape(lat, 7, 0.2, 1.7, 1.7);

		for (int j = 0; j < total + 1; ++j)
		{
			++t;

			lon = ofMap(j, 0, total, -PI, PI);
			r1 = supershape(lon, mSlide, n1Slide, n2Slide, n3Slide);

			x = r * r1 * cos(lon) * r2 * cos(lat);
			y = r * r1 * sin(lon) * r2 * cos(lat);
			z = r * r2 * sin(lat);


			globe[i][j] = vec3(x,y,z);

		}
	}


	for (int i = 0; i < total + 1; ++i)
	{
		for (int j = 0; j < total; ++j)
		{
			++index;

			vec3 n = globe[i][j];
			myMesh.addVertex(n);
			myMesh.addTexCoord(vec2(i / total, j / total));
			n = normalize(cross(globe[i][j + 1] - globe[i][j], globe[i + 1][j] - globe[i][j]));
			myMesh.addNormal(n);
			myMesh.addIndex(index);
			
			++index;
			vec3 n1 = globe[i + 1][j];
			myMesh.addVertex(n1);
			myMesh.addTexCoord(vec2((i + 1) / total, j / total));
			n1 = normalize(-cross(globe[i][j + 1] - globe[i + 1][j + 1], globe[i + 1][j] - globe[i + 1][j + 1]));
			myMesh.addNormal(n1);
			myMesh.addIndex(index);

		}
	}

	int numvert = myMesh.getNumVertices();
	std::cout << numvert << endl;

	int numind = myMesh.getNumIndices();
	std::cout << numind << endl;

}

void ofApp::sphereUpdate(ofMesh& myMesh, float total)
{
	index = -1;

//update sliders for supershape variables:

	mSlide = ofMap(headset.getAttention(), -1, 1, 10, 3, true);

	n1Slide = ofMap(headset.getEntropy(), 0, 0.8, 5, 0.8, true);

	n2Slide = 3;

	n3Slide = ofMap(n1Slide, 1, 5, 2, 5);
//


	for (int i = 0; i < total + 1; ++i)
	{
		lat = ofMap(i, 0, total, -HALF_PI, HALF_PI);
		r2 = supershape(lat, mSlide, n1Slide, n2Slide, n3Slide);

		for (int j = 0; j < total + 1; ++j)
		{
			++t;

			lon = ofMap(j, 0, total, -PI, PI);
			r1 = supershape(lon, mSlide, n1Slide, n2Slide, n3Slide);

			x = r * r1 * cos(lon) * r2 * cos(lat);
			y = r * r1 * sin(lon) * r2 * cos(lat);
			z = r * r2 * sin(lat);

			globe[i][j] = vec3(x, y, z);

		}
	}

	for (int i = 0; i < total + 1; ++i)
	{
		for (int j = 0; j < total; ++j)
		{
			++index;
			vec3 n = globe[i][j];
			myMesh.setVertex(index, n);
			myMesh.setTexCoord(index, vec2(i/total, j / total));
			n = normalize(cross(globe[i][j+1] - globe[i][j], globe[i+1][j] - globe[i][j]));
			myMesh.setNormal(index, n);

			++index;
			vec3 n1 = globe[i + 1][j];
			myMesh.setVertex(index, n1);
			myMesh.setTexCoord(index, vec2((i+1) / total, j / (total+1)));
			n1 = normalize(-cross(globe[i][j+1] - globe[i+1][j+1], globe[i + 1][j] - globe[i+1][j+1]));
			myMesh.setNormal(index, n1);
		}
	}

	index = -1;
}


//--------------------------------------------------------------
float ofApp::supershape(float theta, float m, float n1, float n2, float n3) {

	a = aSlide;
	b = bSlide;

	float t1 = abs((1 / a) * cos(m * theta / 4));
	t1 = pow(t1, n2);
	
	float t2 = abs((1 / b * sin(m * theta / 4)));
	t2 = pow(t2, n3);

	float t3 = t1 + t2; 
	t3 = pow(t3, -1 / n1); 

	float value = t3;
	return value;
}

//--------------------------------------------------------------
void ofApp::update(){
	
	sphereUpdate(myMesh, total);


//update head position:
	vec3 gyro(headset.getGyroscope());

	float threshold = 4;
	headRotation.x += abs(gyro.y) > threshold ? gyro.y*-0.002 : 0;
	headRotation.y += abs(gyro.z) > threshold ? gyro.z*0.002 : 0;
	headRotation.z += abs(gyro.x) > threshold ? gyro.x*-0.002 : 0;

	//add some "friction" to keep it in place
	headRotation -= 0.02 * headRotation;
//

	//matAmbient = ofFloatColor(ambRed, ambGreen, ambBlue);

	//change colour based on mood
	ambRed = ofMap(headset.getMood(), -1, 1, 0.0, 0.775);
	ambGreen = ofMap(headset.getMood(), -1, 1, 0.07, 0.5);
	ambBlue = 0.135;
	matAmbient = ofFloatColor(ambRed, ambGreen, ambBlue);
}

//--------------------------------------------------------------
void ofApp::draw(){
	
	ofEnableDepthTest();
	ofBackgroundGradient(ofColor(255), ofColor(128));


	myCam.begin();

	/*
	ofPushMatrix();
	ofDrawAxis(5000);
	ofPopMatrix();
	*/

		geoShader.begin();
		
		//these uniforms for PHONG
		geoShader.setUniform4f("matAmbient", matAmbient);
		geoShader.setUniform4f("matDiffuse", matDiffuse);
		geoShader.setUniform4f("matSpecular", matSpecular);
		geoShader.setUniform1f("matShininess", matShininess);
		geoShader.setUniform4f("lightPosition", lightPosition);
		geoShader.setUniform4f("lightColor", lightColor);
		geoShader.setUniform1f("matReflectivity", matReflectivity);
		geoShader.setUniformTexture("environmentMap", environmentMap.getTexture(), 1);
		geoShader.setUniform1f("matRefractiveIndex", matRefractiveIndex);


		//these for the geom/frag combo move
		geoShader.setUniformTexture("tex0", environmentMap.getTexture(), 1);
		//change veroni effect based on attention:
		geoShader.setUniform1f("time", ofMap(headset.getAttention(), -1, 1, 0, 0.01));
		geoShader.setUniform1f("offset", ofMap(headset.getAttention(), -1, 1, 0.01, 0.05));

		ofRotateXRad(headRotation.x);
		ofRotateYRad(headRotation.y);
		ofRotateZRad(PI / 2 + headRotation.z);

			
			myMesh.draw();

		
		geoShader.end();


	myCam.end();
	

	//disable depth test and lighting so GUI panel and headset can be drawn correctly:
	ofDisableDepthTest();

	//GUI:
	//panel.draw();

	//headset connection quality:
	headset.drawHorshoe( vec3(ofGetWidth() * 0.45, ofGetHeight() *0.9, 0));

}