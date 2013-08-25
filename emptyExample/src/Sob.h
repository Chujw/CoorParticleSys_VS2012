#pragma once
#include "ofMain.h"

#define THRESHOLD 20	// higher, vaguer; lower, clearer

class Sob{
public:
	
	int **GX;
	int **GY;
	float *Gradx;
	float *Grady;
	bool GradGernerlized;

Sob()
{
	GradGernerlized = false;
};

void setupmask()
{
	GX = new int*[3]; 
	for (int i = 0; i < 3; i++) 
	{
		GX[i] = new int[3]; 
	}
	GX[0][0] = 1; GX[0][1] = 2; GX[0][2] = 1;
	GX[1][0] = 0; GX[1][1] = 0; GX[1][2] = 0;
	GX[2][0] = -1; GX[2][1] = -2; GX[2][2] = -1;

	GY = new int*[3]; 
	for (int i = 0; i < 3; i++) 
	{
		GY[i] = new int[3]; 
	}
	GY[0][0] =  1; GY[0][1] =  0; GY[0][2] = -1;
	GY[1][0] =  2; GY[1][1] =  0; GY[1][2] = -2;
	GY[2][0] =  1; GY[2][1] =  0; GY[2][2] = -1;	
}; 

void initGrad(int w, int h)
{
	Gradx = new float[h*w];
	Grady = new float[h*w];
	//for(int i=0; i<h;i++)
	//{
	//	Grad[i] = new ofVec2f[w];
	//}

	for(int y=0;y<h;y++)
	{
		for(int x=0; x<w;x++)
		{
			Gradx[y*w+x] = -1;
			Grady[y*w+x] = -1;
		}
	}
};

//ofVec2f* getGradient(ofImage inputImage)
//{
//	if(GradGernerlized)
//	{
//		//int x = Grad[512+3].x;
//		return Grad;
//	}
//		
//	else
//	{
//		cout<<"Cannot get gradient vector field."<<endl;
//		return false;
//	}
//};


ofImage outputImage(ofImage inputImage)
{
	setupmask();
	ofImage outputImage = inputImage;
	unsigned char* OldPixels = inputImage.getPixels();
	unsigned char* NewPixels = outputImage.getPixels();
	int h = inputImage.getHeight();
	int w = inputImage.getWidth();
	int sumG;
	initGrad(w,h);
	for(int y=0; y <h; y++)
	{
		for(int x=0; x<w; x++)
		{
			int sumGX = 0;
			int sumGY = 0;
			if(y==0 || y==h-1)			// image boundaries 
				sumG = 0;
			else if(x==0 || x==w-1)
				sumG = 0;

			else
			{
				for(int i=-1; i<=1; i++)					// X GRADIENT APPROXIMATION
				{
					for(int j=-1; j<=1; j++)				
					{
						sumGX = sumGX + (OldPixels[x + j + (y + i)*w]) * GX[j+1][i+1];
					}
				}

				for(int i=-1; i<=1; i++)					// Y GRADIENT APPROXIMATION
				{
					for(int j=-1; j<=1; j++)				
					{
						sumGY = sumGY + (OldPixels[x + j + (y + i)*w]) * GY[j+1][i+1];
					}
				}
				//sum = abs(sumGX) + abs(sumGY);				// GRADIENT MAGNITUDE APPROXIMATION
				sumG = sqrt((double)(sumGX*sumGX+sumGY*sumGY));
			}
			Gradx[y*w+x] = sumGX;
			Grady[y*w+x] = sumGY;	// GRADIENT VECTOR
			//Grad[y*w+x].normalize();
			GradGernerlized = true;
			if(sumG > THRESHOLD) // when sumG = max, it's an edge pixel
				sumG = 255;
			if(sumG < THRESHOLD) 
				sumG = 0;
			NewPixels[x+y*w] = 255 - (unsigned char)(sumG);	// reverse the color value, show the edge as black
		}
	}
	return outputImage;
};

};
