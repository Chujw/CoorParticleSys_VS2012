#pragma once
#include "ofMain.h"

class Gaussian{

public:

	ofImage GaussianBlur(ofImage inputImage, double sigma){
	//create a temp image
	int width = inputImage.getWidth();
	int height = inputImage.getHeight();
	unsigned char* temp = new unsigned char[width*height];
	unsigned char* input = inputImage.getPixels();
	ofImage outputImage = inputImage;
	unsigned char* newpixels = outputImage.getPixels();

	if(!temp)
	{
		return false;
	}

	//kernelsize --> 5
	double gkernel = 3.0;
	int kernelRadius = sigma * gkernel;
	int kernelSize=kernelRadius*2+1;

	double * kernel=new double[kernelSize];

	for(int x=-kernelRadius; x<=kernelRadius; ++x)
		kernel[x+kernelRadius]=exp(-(x*x)/(2*sigma*sigma))/(sqrt(2*PI)*sigma);

	//normalize
	double kernelTotal=0.0f;

	for(int i=0; i<kernelSize; ++i)
		kernelTotal+=kernel[i];

	for(int i=0; i<kernelSize; ++i)
		kernel[i]/=kernelTotal;

	//x direction
	for(int y=0; y<height; ++y)
	{
		for(int x=0; x<width; ++x)
		{
				double newValue=0.0;
				for(int kx=-kernelRadius; kx<=kernelRadius; ++kx)
				{
					int pixelColumn=x+kx;
					if(pixelColumn<0)
						pixelColumn=-pixelColumn-1;

					if(pixelColumn>=width)
						pixelColumn=width-(pixelColumn-width)-1;

					newValue+=kernel[kx+kernelRadius]*input[y*width+pixelColumn];
				}
				temp[y*width+x]=newValue;
		}
	}

	//y direction
	for(int y=0; y<height; ++y)
	{
		for(int x=0; x<width; ++x)
		{
			double newValue=0.0;
			for(int kx=-kernelRadius; kx<=kernelRadius; ++kx)
			{
				int pixelRow=y+kx;
				if(pixelRow<0)
					pixelRow=-pixelRow-1;

				if(pixelRow>=height)
					pixelRow=height-(pixelRow-height)-1;
				newValue+=kernel[kx+kernelRadius]*temp[pixelRow*width+x];
			}
			outputImage.getPixelsRef()[y*width+x]=(unsigned char)newValue;
		}
	}
	return outputImage;
	};
};