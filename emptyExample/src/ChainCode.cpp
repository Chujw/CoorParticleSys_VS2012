#include "ChainCode.h"


bool ChainCode::IsTrueEdge()
{
	if(edgepixel_count >= EDGE_LENGTH)
		return true;
	else
	{
		edgepixel_count = 0;
		return false;
	}
}

void ChainCode::Edgepixel_count()
{
	edgepixel_count++;
}

bool ChainCode::GetFirstChnPix(ofVec2f* curPixel, ofImage EG_map)
{
	int w = ofGetWidth();
	int h = ofGetHeight();
	ofColor black;
	black.set(0,0,0);
	int x = curPixel->x;
	int y = curPixel->y;
	ofVec2f orgPixel;
	orgPixel.set(*curPixel);
	// Find the first edge pixel
	while(y<h && *curPixel == orgPixel)
	{
		y++;
		x = 0;
		while(x<w && *curPixel == orgPixel)
		{
			if(EG_map.getColor(x,y)==black)
				curPixel->set(x,y);
			x++;
		}
	}
	if(EG_map.getColor(curPixel->x,curPixel->y)==black)
	{
		//cout<<"Found the first edge pixel: "<<curPixel->x<<", "<<curPixel->y<<endl;
		return true;
	}
	else
		return false;
}

void ChainCode::GetNextChnPix(ofVec2f* curPixel, int* dir, ofImage EG_map)
{
	int count = 0;
	while(!SearchThisDir(curPixel,*dir,EG_map) && count<8)	// if not found in the current direction, update the dir
	{
		*dir = *dir+1;		// anti-clockwise direction
		count++;
		if(*dir>7)
			*dir = 0;
	}
	if(count>=7)	// if cannot found
	{
		//cout<<"Cannot find edge pixel around it, it is a single point."<<endl;
		//cout<<"curPixel: "<<curPixel->x<<", "<<curPixel->y<<endl;
	}
}

bool ChainCode::Stop(ofVec2f curPixel, ofVec2f lastPixel, ofVec2f firstpt, ofVec2f secpt)
{
	if(curPixel == secpt && lastPixel == firstpt)
		return true;
	else
		return false;
}

//-----------------------------------------------------------------------------------
//
// Search in the next direction, if found one, update the curPixel and return
//
//-----------------------------------------------------------------------------------
bool ChainCode::SearchThisDir(ofVec2f* curPixel, int dir, ofImage EG_map)
{
	// Search the 3*3 neighborhood of curPixel
	switch(dir)
	{
	case 0:	// East
		if(EG_map.getColor(curPixel->x+1,curPixel->y) == 0)
		{
			curPixel->x++;
			return true;
		}
		else
			return false;
	case 1:	// Northeast
		if(EG_map.getColor(curPixel->x+1,curPixel->y-1) == 0)
		{
			curPixel->x++;
			curPixel->y--;
			return true;
		}
		else
			return false;
	case 2:	// North
		if(EG_map.getColor(curPixel->x,curPixel->y-1) == 0)
		{
			curPixel->y--;
			return true;
		}
		else
			return false;
	case 3: // Northwest
		if(EG_map.getColor(curPixel->x-1,curPixel->y-1) == 0)
		{
			curPixel->x--;
			curPixel->y--;
			return true;
		}
		else
			return false;
	case 4:	// West
		if(EG_map.getColor(curPixel->x-1,curPixel->y) == 0)
		{
			curPixel->x--;
			return true;
		}
		else
			return false;
	case 5:	// Southwest
		if(EG_map.getColor(curPixel->x-1,curPixel->y+1) == 0)
		{
			curPixel->x--;
			curPixel->y++;
			return true;
		}
		else
			return false;
	case 6:	// South
		if(EG_map.getColor(curPixel->x,curPixel->y+1) == 0)
		{
			curPixel->y++;
			return true;
		}
		else
			return false;
	case 7:	// Southeast
		if(EG_map.getColor(curPixel->x+1,curPixel->y+1) == 0)
		{
			curPixel->x++;
			curPixel->y++;
			return true;
		}
		else
			return false;
	default:
		return false;

	}
}


//---------------------------------------------------------
//
// Compute next direction based on current one
//
//---------------------------------------------------------
int ChainCode::NextDir(int dir)
{
	int newdir = -1;
	if(dir%2==0)	// if dir is even
		newdir = (dir+7)%8;
	else			// if dir is odd
		newdir = (dir+6)%8;
	return newdir;
}
