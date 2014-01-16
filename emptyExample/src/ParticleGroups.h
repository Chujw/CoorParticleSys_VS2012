#pragma once

#include "ofMain.h"
#include "Particle.h"
#include "common.h"
#include "EdgeMap.h"
#include "SpacingMap.h"
//#include "Quadtree.h"
#include "GridCollisionDetection.h"


//#define DEFAULT_NUM			70
//#define DEFAULT_SPACING		50
#define BRH_OFFSET		1.41/**DEFAULT_SPACING*/	 	// stepsize for birth control	141% of spacing (1.41%)
#define DTH_OFFSET		0.58/**DEFAULT_SPACING*/		// stepsize for death control	58% of spacing (60%)
#define BRH_MIN_NUM			1	// every BRH_BLOCKSIZE should at least have BRH_MIN_NUM of particles (default mode, could change according to intensity)
#define DTH_MAX_NUM			5	// every DTH_BLOCKSIZE could at most have DTH_MAX_NUM of particles (default mode : same as above)
#define TILT_LIMIT			rand()%60 +20
#define COLOR			0/*ofColor(rand()%255, rand()%255,rand()%255)*/



class ParticleGroups{
private:
	vector<Particle> particle;
	bool* Foreground_map;
	int head_par;
	int rear_par;
	int numpt;	// number of init particles
	int tilt_control;
	bool canstop;
	bool FillGrids;
	int special_thred_type;
	//Quadtree quad;
	GridCollisionDetection grid;
	long int beacon_num;
	bool Is_Foreground;// is this par group works for open area?
	//int drawbeacon_count;

public:
	void Setup();
	void Setup_parlist_bkg(ofVec2f* parlist, int closepar_numpt, SpacingMap m_spacing);
	void Setup_parlist_forg(ofVec2f* parlist,int openpar_numpt,SpacingMap m_spacing);
	void Setup_Background_map(ofVec2f* AllPixelsInChain, int allpixels_num);	// setup the map of open area via parlist
	void Setup_Foreground_map(ofVec2f* AllPixelsInChain, int allpixels_num);	// setup the map of open area via parlist
	void Setup_2ndForeground_map(ofVec2f* AllPixelsInChain, int allpixels_num);
	void Simulate(SpacingMap m_spacing, ofImage* m_canvas);

	void DrawAll(ofImage* m_canvas);	
	void Birth_Death(SpacingMap m_spacing);
	void birth(Particle* endpointA, Particle* endpointB, SpacingMap m_spacing);
	void death(Particle* endpointA, Particle* endpointB);
	void TiltUpdate(Particle* thispar);
	void TiltControl(Particle* thispar);	
	bool CanStop();
	bool GetStopSignal();

	void BoundaryControl();
	void BoundaryControl_Window();
	void BoundaryControl_Foreground();

	bool OutOfFeatureMap(Particle thispar);

	void kill(int indexID);
	void SetAsRegular(Particle* A, Particle* B, float birth_dist);
	void FixDyingFlags(Particle* A);

	void CreateGrids();
	void UpdateGrids();
	void GridsCollisionKill();

	Particle FindPar(float x, float y);
};