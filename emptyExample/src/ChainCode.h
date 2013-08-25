#pragma once

#include"ofMain.h"

#define	 EDGE_LENGTH	200

class ChainCode{

private:
	int edgepixel_count;

public:
	ChainCode(){edgepixel_count =0;};
	bool GetFirstChnPix(ofVec2f* curPixel, ofImage EG_map);	// get the first chain pixel
	void GetNextChnPix(ofVec2f* curPixel, int* dir, ofImage EG_map);	// search the rest
	bool SearchThisDir(ofVec2f* curPixel,int dir, ofImage EG_map);
	bool Stop(ofVec2f curPixel, ofVec2f lastPixel, ofVec2f firstpt, ofVec2f secpt);
	int NextDir(int dir);
	bool IsTrueEdge();
	void Edgepixel_count();



};