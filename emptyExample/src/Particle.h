#pragma once
#include "ofMain.h"
#include "common.h"
#include "SpacingMap.h"

#define		BIRTH	1
//#define NUM 3
#define DEFAULT_SPEED		0.5 
#define DEFAULT_LINEWIDTH	1
#define LINEMAX		10
#define	LINEMIN		0
//#define	BEARING_LIMIT	

class Particle {

public:

	float angle;
	float bearing;	// the rotate angle in clockwise direction from the north line
	ofVec2f bearing_vec;	// the vector of bearing
	ofVec2f pos;
	ofVec2f last_pos;
	float speed;
	int id;
	int beacon_id;
	Particle* prev;
	Particle* next;
	int parentbeacon;
	int childbeacon;
	bool Is_released;
	bool Was_released;
	bool Is_newborn;
	bool Is_dying;
	float last_tilt;
	int tilt_control;
	int tilt_limit;
	ofColor c;
	float linewidth;
	float last_spacing_threshold;
	int grid_id;

	Particle(float theta=0, float sp=0.5);
	void Move2Middle(bool IsOpenMode, int head_par, int rear_par);
	float GetSigmoidBearing(bool IsOpenMode, int head_par, int rear_par, Particle* thispar);
	float GetAverageBearing(bool IsOpenMode, int head_par, int rear_par, Particle* thispar);
	//int BirthOrDeath(int b_stepsize, int d_stepsize, int b_min_num, int d_max_num);
	void Update();
	void advance();	// add some random direction factors
	bool OutOfBoudaryKill();
	bool PushBackOrFwd(int head, int rear,bool IsOpenMode);
	bool UpdateSpacingTd(float currentTd);// is the local last_spacing_threshold same as the threshold in the current pixel
	float UpdateLineWidth(float currentTd, float minTd, float maxTd);
};