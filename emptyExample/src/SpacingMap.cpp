#include "SpacingMap.h"

void SpacingMap::PixelSpaceViables(float* grayscale_pixels, float min, float max)
{
	//-------Init map------------
	height = ofGetHeight();
	width = ofGetWidth();
	spaceViables_map = new float [height*width];
	//for(int y = 0; y<height; y++)
	//{
	//	for(int x = 0; x<width/3; x++)
	//		spaceViables_map[y*width+x] = LEFT_VIABLE;
	//	for(int x = width/3; x<2*width/3; x++)
	//		spaceViables_map[y*width+x] = MID_VIABLE;
	//	for(int x = 2*width/3; x<width; x++)
	//		spaceViables_map[y*width+x] = RIGHT_VIABLE;
	//}
	for(int y = 0; y<ofGetHeight(); y++)
	{
		for(int x=0; x<ofGetWidth(); x++)
		{
			int index = y*ofGetWidth()+x;
			spaceViables_map[index] = grayscale_pixels[index]*(max-min)/255 + min;
			float tempcheckviable = spaceViables_map[index];
			if(spaceViables_map[index]>max)
				spaceViables_map[index] = max;
			else if(spaceViables_map[index]<min)
				spaceViables_map[index] = min;
		}
	}

}

//void SpacingMap::RescaleGrayValues(float* grayscale_pixels, float min, float max)
//{
//
//}

void SpacingMap::PixelThresholds()	
{
	height = ofGetHeight();
	width = ofGetWidth();
	spaceThresholds_map = new float [height*width];
	for(int i = 0; i<height*width; i++)
		spaceThresholds_map[i] = BASE_SPACING;
}

void SpacingMap::SetupSpacingMaps(float* grayscale_pixels)
{
	PixelSpaceViables(grayscale_pixels,MIN_SP_VIABLE,MAX_SP_VIABLE);
	PixelThresholds();
	height = ofGetHeight();
	width = ofGetWidth();
	space_map = new float [height*width];
	for(int i = 0; i<ofGetHeight()*ofGetWidth(); i++)
		space_map[i] = spaceViables_map[i] * spaceThresholds_map[i]; 
}

float SpacingMap::GetBaseThreshold(int x, int y)
{
	int index = y*width+x;
	if(index>=0 && index<ofGetWidth()*ofGetHeight())
		return space_map[index];
	else
		return space_map[0];
}

float SpacingMap::GetDefaultSpacing()
{
	return BASE_SPACING;
}