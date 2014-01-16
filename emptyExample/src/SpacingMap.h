#pragma once

#include "ofMain.h"
#define		BASE_SPACING	4.5
//#define		MID_BASE_SPACING	20
//#define		LEFT_VIABLE		1
//#define		MID_VIABLE	1
//#define		RIGHT_VIABLE		1
#define		MIN_SP_VIABLE	2
#define		MAX_SP_VIABLE	9



class SpacingMap{
private:
	float* spaceViables_map;
	float* spaceThresholds_map;
	float* space_map;
	int height;
	int width;

public:
		//--------------------Try to control the spacing---------------------
	void PixelSpaceViables(float* grayscale_pixels, float min, float max);	// Create viables for each pixel
	//void RescaleGrayValues(float* grayscale_pixels, float min, float max);
	void PixelThresholds();
	//void GetSpaceViableFromImage();
	float GetBaseThreshold(int x, int y);
	void SetupSpacingMaps(float* grayscale_pixels);
	float GetDefaultSpacing();
};