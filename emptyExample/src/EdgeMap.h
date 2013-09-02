#pragma once

#include "ofMain.h"
//#include "Thread.h"
#include "Particle.h"
#include "ChainCode.h"
#include "common.h"
#include "Gaussian.h"
#include "Sob.h"
//#include "ParticleGroups.h"

#define BLACK				0
#define GRIDSIZE			4	// 2 by 2 grid
#define SIGMA				0.8
//#define DEFAULT_SPACING		12
//#define DEFAULT_SPEED		0.3 
#define DEFAULT_BEARING		radians(20)
#define XD					ofGetWidth()
#define YD					ofGetHeight()
#define RADIUS				YD/14
#define CENTER				ofVec2f(XD/5, YD/2)
#define START_POINT			ofVec2f(XD/4-RADIUS, YD/3)
#define CHAIN_NUM			7000
#define DIRECTION_OPEN_AREA		90

class EdgeMap{
private:
	ofImage canvas;
	ofImage EG_map;
	ChainCode chain;
	int gridsize;
	int id;
	//ofVec2f* close_edgechain;
	ofVec2f* bkg_edgechain;
	ofVec2f* forg_edgechain;
	ofVec2f* AllPixelsInChain;
	float* grayscale_pixels;
	ofVec2f start_edgepx;
	Gaussian gau;
	Sob sob;
	int bkgchain_num;
	int openchain_num;
	int AllPixels_num;

public:
	void MakeImages();
	void InitEdgeMap(int this_id);
	ofVec2f* GetCloseEdgeChain(int* par_index, float DefaultSpacing);	// use chain code to recognize images
	void ReverseEdgeChain(ofVec2f* edgechain, int numpt);
	//bool MakeOpenEdgeChains(ofVec2f posA, ofVec2f posB, int* chain_num);	
	ofVec2f* MakeBkgEdgeChains(ofVec2f StartpA, ofVec2f EndpB, int* chain_num, float DefaultSpacing);
	bool MakeOpenEdgeChainsFromThreadEdge(ofVec2f posA, ofVec2f posB, int* chain_num, float DefaultSpacing);
	void GetCircleFromDrawing();
	void GetOrgDistribution(vector<Particle> par_list,int par_index,ofVec2f pos, float bearing);
	
	bool NeedToFindAgain();
	void SetupGrayValue();
	float* GetGrayscaleMap();
	int FindChainPoint(ofVec2f thispos);
	ofVec2f* GetForgEdgeChain();
	ofImage GetsrcImage();
	ofImage GetEdgeMap();
	ofVec2f* GetAllPixelsInChain(int* allpixels_num);
};