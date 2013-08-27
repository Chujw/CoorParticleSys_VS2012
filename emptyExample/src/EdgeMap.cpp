#include "EdgeMap.h"


//--------------------------------------------------
// small handy functions
//--------------------------------------------------
//float dist(ofVec2f A, ofVec2f B)
//{
//	return sqrt((A.x-B.x)*(A.x-B.x)+(A.y-B.y)*(A.y-B.y));
//}
//
//Particle midpAB(Particle* A, Particle* B)
//{
//	Particle midpoint;
//	midpoint.pos.x = (A->pos.x+B->pos.x)/2;
//	midpoint.pos.y = (A->pos.y+B->pos.y)/2;
//	return midpoint;
//}

//-----------------------------------------------------------------

void EdgeMap::InitEdgeMap(int this_id)
{
	bkgchain_num = 0;
	openchain_num = 0;
	AllPixels_num = 0;
	id = this_id;
	//ofSetColor(255);
	EG_map.allocate(ofGetWidth(),ofGetHeight(),OF_IMAGE_GRAYSCALE);
	//close_edgechain.resize(CHAIN_NUM);
	//open_edgechain.resize(CHAIN_NUM);
	bkg_edgechain = new ofVec2f[CHAIN_NUM];
	open_edgechain = new ofVec2f[CHAIN_NUM];
	AllPixelsInChain = new ofVec2f[CHAIN_NUM];
	start_edgepx.set(-1,-1);
	grayscale_pixels = new float[ofGetWidth()*ofGetHeight()];
}

//-----------------------------------------------------------------
//
// Load source images and compute the gray scale image and edge map
//
//-----------------------------------------------------------------
void EdgeMap::MakeImages()
{
	canvas.loadImage("TheScream.jpg");
	canvas.resize(ofGetWidth(), ofGetHeight());
	canvas.setImageType(OF_IMAGE_GRAYSCALE);
	SetupGrayValue();	// store the gray value of each pixel
	cout<<"Computing Gaussian Map..."<<endl;	// get edge map
	EG_map = gau.GaussianBlur(canvas,SIGMA);
	cout<<"Computing Sobel Map..."<<endl;
	EG_map = sob.outputImage(EG_map);
	cout<<endl;
}


//---------------------------------------------------------
//
// Use chain code algorithm to store edge data
//	
//	East: 0		Northeast: 1		North: 2
//	West: 4		Northwest: 3		South: 6
//	Southeast: 7	Southwest: 5
//
//---------------------------------------------------------
ofVec2f* EdgeMap::GetCloseEdgeChain(int* par_index, float DefaultSpacing)
{	
			
	ofVec2f lastPixel;
	ofVec2f firstpt;
	ofVec2f secpt;
	float dis_count = 0;
	//reset paras
	//close_edgechain.clear();
	//close_edgechain.resize(CHAIN_NUM);

	*par_index = 0;
	AllPixels_num = 0;
	cout<<"Start searching edge chains..."<<endl;

	if(chain.GetFirstChnPix(&start_edgepx,EG_map))	// find the first chain pixel
	{
		ofVec2f curPixel;
		curPixel.set(start_edgepx);
		AllPixelsInChain[AllPixels_num++].set(start_edgepx);
		int dir = 7;	// init dir
		firstpt = curPixel;	// store the first pixel (once only)
		dir = chain.NextDir(dir);	// update direction
		lastPixel = curPixel;	// store as the last chain pixel found
		chain.GetNextChnPix(&curPixel,&dir,EG_map);	// get the second chain pixel
		//cout<<"Found the second edge pixel: "<<curPixel.x<<", "<<curPixel.y<<endl;
		secpt = curPixel;	// store the second pixel (once only)
		AllPixelsInChain[AllPixels_num++].set(curPixel);
		//cout<<"Finding other edge pixels..."<<endl;
							////for(int i=0; i<AllPixels_num; i++)
							////{
							////	cout<<i<<" : "<<AllPixelsInChain[i]<<endl;
							////}
		do
		{
			if(dis_count>=DefaultSpacing)			// store the position to release particles every 12 steps
			{
				dis_count = 0;
				//close_edgechain[*par_index].pos = curPixel;	// start from the second particle in array
				bkg_edgechain[*par_index] = curPixel;	// start from the second particle in array
				//ofVec2f par_dir = (curPixel-lastPixel).getPerpendicular();
				//par_dir = par_dir.normalize();
				//close_edgechain[*par_index].bearing = acos(par_dir.dot(ofVec2f(0,1)));
				*par_index = *par_index+1;
				//cout<<"still adding......"<<endl;
			}
			else
				dis_count+=curPixel.distance(lastPixel);
			chain.Edgepixel_count();	// count the number of all edge pixels
			if(start_edgepx.y<curPixel.y)
				start_edgepx = curPixel;
			lastPixel = curPixel;
			dir = chain.NextDir(dir);
			chain.GetNextChnPix(&curPixel,&dir,EG_map);	// find a next pixel
			AllPixelsInChain[AllPixels_num++].set(curPixel);

								////cout<<AllPixels_num-1<<" : "<<AllPixelsInChain[AllPixels_num-1]<<endl;
						//cout<<"searching next chain pixel"<<endl;
		}
		while(!chain.Stop(curPixel,lastPixel,firstpt,secpt));
		cout<<"Finished searching edges."<<endl;
		cout<<endl;
	}
	bkgchain_num = *par_index;	// store the number of chain pixels
	ReverseEdgeChain(bkg_edgechain, *par_index);
	return bkg_edgechain;
}

ofVec2f* EdgeMap::MakeBkgEdgeChains(ofVec2f StartpA, ofVec2f EndpB, int* chain_num, float DefaultSpacing)
{
	cout<<"Distributing particles for backgroud..."<<endl;
	int size = 2*StartpA.distance(EndpB);
	int allpixel_num = 0;
	bkg_edgechain = new ofVec2f[size];
	ofVec2f* LinePixel = new ofVec2f[size];
	if(StartpA.x < EndpB.x)	// when it is an A--B line
	{
		float k = (EndpB.y-StartpA.y)/(EndpB.x-StartpA.x);	// compute the slope value
		float dx = 1;
		for(float x=StartpA.x; x<=EndpB.x; x+=dx)
		{
			float y = k * (x - StartpA.x) + StartpA.y; 
			LinePixel[allpixel_num++] = ofVec2f(floor(x+0.5),floor(y+0.5));
			//*chain_num = openchain_num;
		}
	}
	else if(StartpA.x == EndpB.x)	// if it is a verticle thread line
	{
		float k;
		float dy;
		if(StartpA.y < EndpB.y)	// A above B
			dy = 1;
		else if(StartpA.y > EndpB.y)
			dy = -1;
		for(float y=StartpA.y; y<=EndpB.y; y+=dy)
		{
			float x = StartpA.x; 
			LinePixel[allpixel_num++] = ofVec2f(floor(x+0.5),floor(y+0.5));
		}
	}
	else	// when it is a B--A line
	{
		float k = (EndpB.y-StartpA.y)/(EndpB.x-StartpA.x);	// compute the slope value
		float dx = 1;
		for(float x=StartpA.x; x>=EndpB.x; x-=dx)
		{
			float y = k * (x - StartpA.x) + StartpA.y; 
			LinePixel[allpixel_num++] = ofVec2f(floor(x+0.5),floor(y+0.5));
		}
	}

	//--------------------------------------------------------------
	// store pixels into the openchain array every default distance
	//--------------------------------------------------------------
	float dis_count = 0;
	bkgchain_num = 0;
	for(int i=0; i<allpixel_num-1; i++)
	{
		dis_count+=LinePixel[i].distance(LinePixel[i+1]);
		if(dis_count>=DefaultSpacing)
		{
			dis_count = 0;
			bkg_edgechain[bkgchain_num++] = LinePixel[i];
			*chain_num = bkgchain_num;
		}
	}
	return bkg_edgechain;
}

//--------------------------------------------------------------------
// before reverse, the edge chain stored as from right to left,after 
//reverse, it goes as clockwise, so that it suits for Move_2_Middle();
//--------------------------------------------------------------------
void EdgeMap::ReverseEdgeChain(ofVec2f* edgechain, int numpt)	
{
	ofVec2f* rev_edgechain;
	rev_edgechain = new ofVec2f[CHAIN_NUM];
	int j = 0;
	for(int i = 0; i<numpt; i++)	// copy chain array
	{
		rev_edgechain[i] = edgechain[i];
	}
	for(int j = 0; j<numpt; j++)	// reverse the chain array
	{
		edgechain[j] = rev_edgechain[numpt-j-1];
	}
}



//--------------------------------------------------------------------
// Given two points, find all pixels between the points, store them 
// as the open edge chain pixels
//--------------------------------------------------------------------
bool EdgeMap::MakeOpenEdgeChainsFromThreadEdge(ofVec2f StartpA, ofVec2f EndpB, int* chain_num, float DefaultSpacing)
{
	cout<<"Making the edge chains from given tread points..."<<endl;
	//if(StartpA.y > EndpB.y)	// switch the position of A and B to make sure the A is above B
	//{
	//	ofVec2f temp = StartpA;
	//	StartpA = EndpB;
	//	EndpB = temp;
	//}
	//else if(StartpA.y == EndpB.y)
	//{
	//	if(StartpA.x < EndpB.x)	// switch the position to make sure A is at the right side of B 
	//	{
	//		ofVec2f temp = StartpA;
	//		StartpA = EndpB;
	//		EndpB = temp;
	//	}
	//}
	//-------------------------------------------
	// get all the pixels on the thread line
	//-------------------------------------------
	int size = 2*StartpA.distance(EndpB);
	int allpixel_num = 0;
	open_edgechain = new ofVec2f[size];
	ofVec2f* LinePixel = new ofVec2f[size];
	if(StartpA.x < EndpB.x)	// when it is an A--B line
	{
		float k = (EndpB.y-StartpA.y)/(EndpB.x-StartpA.x);	// compute the slope value
		float dx = 1;
		for(float x=StartpA.x; x<=EndpB.x; x+=dx)
		{
			float y = k * (x - StartpA.x) + StartpA.y; 
			LinePixel[allpixel_num++] = ofVec2f(floor(x+0.5),floor(y+0.5));
			//*chain_num = openchain_num;
		}
	}
	else if(StartpA.x == EndpB.x)	// if it is a verticle thread line
	{
		float k;
		float dy;
		if(StartpA.y < EndpB.y)	// A above B
			dy = 1;
		else if(StartpA.y > EndpB.y)
			dy = -1;
		for(float y=StartpA.y; y<=EndpB.y; y+=dy)
		{
			float x = StartpA.x; 
			LinePixel[allpixel_num++] = ofVec2f(floor(x+0.5),floor(y+0.5));
			//*chain_num = openchain_num;
		}
	}
	else	// when it is a B--A line
	{
		float k = (EndpB.y-StartpA.y)/(EndpB.x-StartpA.x);	// compute the slope value
		float dx = 1;
		for(float x=StartpA.x; x>=EndpB.x; x-=dx)
		{
			float y = k * (x - StartpA.x) + StartpA.y; 
			LinePixel[allpixel_num++] = ofVec2f(floor(x+0.5),floor(y+0.5));
			//*chain_num = openchain_num;
		}
	}

	//--------------------------------------------------------------
	// store pixels into the openchain array every default distance
	//--------------------------------------------------------------
	float dis_count = 0;
	openchain_num = 0;
	for(int i=0; i<allpixel_num-1; i++)
	{
		dis_count+=LinePixel[i].distance(LinePixel[i+1]);
		if(dis_count>=DefaultSpacing)
		{
			dis_count = 0;
			open_edgechain[openchain_num++] = LinePixel[i];
			*chain_num = openchain_num;
		}
	}



	if(openchain_num >0)
		return true;
	else
		return false;
}


//---------------------------------------------------------------------------
//
// Open edge chain starts from the nearest close edge chain pixel from posA to posB
//
//-------------------------------------------------------------------------
//bool EdgeMap::MakeOpenEdgeChains(ofVec2f posA, ofVec2f posB, int* chain_num)
//{
//	cout<<"Making the edge chains for the open area..."<<endl;
//	int start_point = FindChainPoint(posA);
//	int end_point = FindChainPoint(posB);
//	if(start_point < 0 || end_point<0)
//		return false;
//	if(start_point<end_point)
//	{
//		for(int i=start_point; i<=end_point; i++)
//		{
//			// start from the start_point, store chain points in open_edgechain
//			open_edgechain[openchain_num] = close_edgechain[i];
//			//cout<<"open_edgechain["<<openchain_num<<"] = "<<open_edgechain[openchain_num]<<endl;
//			openchain_num++;
//			*chain_num = openchain_num;		// update the chain_num
//		}
//		//return true;
//	}
//	else
//	{
//		for(int i=start_point; i<closechain_num; i++)
//		{
//			open_edgechain[openchain_num] = close_edgechain[i];
//			//cout<<"open_edgechain["<<openchain_num<<"] = "<<open_edgechain[openchain_num]<<endl;
//			openchain_num++;
//			*chain_num = openchain_num;	
//		}
//		for(int i=0; i<=end_point; i++)
//		{
//			open_edgechain[openchain_num] = close_edgechain[i];
//			//cout<<"open_edgechain["<<openchain_num<<"] = "<<open_edgechain[openchain_num]<<endl;
//			openchain_num++;
//			*chain_num = openchain_num;	
//		}
//		//return true;
//	}
//	// if the chain pixel index from the bottome to top (because the pars move to right, so 
//	// only a clock-wise A-B-C direction suits the function Move2Middel()), we need to reverse 
//	// the oder for the chain pixels.
//	if(open_edgechain[0].y>open_edgechain[openchain_num-1].y)
//	{
//		ReverseEdgeChain(open_edgechain,openchain_num);
//	}
//	return true;
//}


//-------------------------------------------------------------------
// find chain point in close chain list (close chain list have stored 
// all chain pixels while open one only has a part of them)
//-------------------------------------------------------------------
int EdgeMap::FindChainPoint(ofVec2f thispos)
{
	float dist = bkg_edgechain[0].distance(thispos);
	int index = -1;
	for(int i=1; i<bkgchain_num; i++)
	{
		float new_dist = bkg_edgechain[i].distance(thispos);
		if(dist>new_dist)
		{
			dist = new_dist;	// find the chain pixel closest to the given position
			index = i;
		}
	}
	if(index<0)
		cout<<"Warning: Cannot find a chain pixel at the given position!"<<endl;
	return index;
}


ofImage EdgeMap::GetsrcImage()
{
	return canvas;	// gray source image
}

ofImage EdgeMap::GetEdgeMap()
{
	return EG_map;
}

ofVec2f* EdgeMap::GetOpenEdgeChain()
{
	return open_edgechain;
}

ofVec2f* EdgeMap::GetAllPixelsInChain(int* allpixels_num)
{
	*allpixels_num = AllPixels_num;
	return AllPixelsInChain;
}

bool EdgeMap::NeedToFindAgain()
{
	if(chain.IsTrueEdge())
		return false;
	else
	{
		// need to find again from the last lowest chain pixel found
		// find the lowest point
		//for(int i =0; i<edgechain.size();i++)
		//{
		//	if(edgechain[i].pos.y<start_edgepx.y)
		//		start_edgepx = edgechain[i].pos;
		//}
		return true;
	}
}

void EdgeMap::SetupGrayValue()
{
	unsigned char* EG_pixels = canvas.getPixels();
	for(int i=0; i<ofGetWidth()*ofGetHeight(); i++)
	{
		grayscale_pixels[i] = EG_pixels[i];
	}
}

float* EdgeMap::GetGrayscaleMap()
{
	return grayscale_pixels;
}


///////////////////////////////////////////////
//////////////EDGE TYPE///////////////////////
//--------------------------------------------
//			0				1			2
//		circle			straight
//--------------------------------------------
