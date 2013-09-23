#include "ParticleGroups.h"


//--------------------------------------------------
// small handy functions
//--------------------------------------------------
float distAB(Particle* A, Particle* B)
{
	return sqrt((A->pos.x-B->pos.x)*(A->pos.x-B->pos.x)+(A->pos.y-B->pos.y)*(A->pos.y-B->pos.y));
}

Particle midpAB(Particle* A, Particle* B)
{
	Particle midpoint;
	midpoint.pos.x = (A->pos.x+B->pos.x)/2;
	midpoint.pos.y = (A->pos.y+B->pos.y)/2;
	return midpoint;
}

//----------------------------------------------------------------------------------------------------

void ParticleGroups::Setup()
{
	canstop = false;
	Is_Foreground = false;
	FillGrids = false;
	numpt = 0;	// num of released regular particles
	particle.resize(CHAIN_NUM);
	tilt_control = 0;
	head_par = -1;
	rear_par = -1;
}



//---------------------------------------------
//
// Setup particle list for close contour area
//
//---------------------------------------------
void ParticleGroups::Setup_parlist_bkg(ofVec2f* closeEdgechain, int closepar_numpt, SpacingMap m_spacing)
{
	Is_Foreground = false; // this par group will work for close area
	numpt = closepar_numpt;
	beacon_num = numpt;
	for(int i=0; i<numpt; i++)
	{
	/*	if(i == 203)
			cout<<endl;*/
		particle[i].id = i;
		particle[i].beacon_id = i;
		particle[i].pos = closeEdgechain[i];
		particle[i].last_pos = particle[i].pos;
		ofVec2f AB;		// tangent direction of this par
		if(i==numpt-1)
			AB = closeEdgechain[i]-closeEdgechain[(i-1+numpt)%numpt];
		else
			AB = closeEdgechain[i+1] - closeEdgechain[i];
		AB.normalize();
		ofVec2f DIR = -AB.getPerpendicular();	// direction of this par
		float this_bearing = acos(DIR.dot(ofVec2f(0,1)));
		if(DIR.x<0)
			this_bearing = 2*PI-this_bearing;
		particle[i].bearing_vec = DIR;
		particle[i].bearing = this_bearing;
		particle[i].angle = degrees(particle[i].bearing);
		particle[i].speed = DEFAULT_SPEED;
		particle[i].last_tilt = particle[i].bearing;
		particle[i].tilt_limit = TILT_LIMIT;
		particle[i].tilt_control = 0;
		particle[i].Is_released = true;
		particle[i].Was_released = true;
		particle[i].Is_newborn = false;
		particle[i].Is_dying = false;
		particle[i].c = COLOR;
		if(i==0)	// for head particle
		{
			particle[0].prev = &particle[numpt-1];	// last par
			particle[0].next = &particle[(i+1)%numpt];	// next par
		}
		else
		{
			particle[i].prev = &particle[(i-1+numpt)%numpt];
			particle[i].next = &particle[(i+1)%numpt];
			particle[i].prev->next = &particle[i];
		}
		particle[i].last_spacing_threshold = m_spacing.GetBaseThreshold(particle[i].pos.x,particle[i].pos.y);	// set up base threshold for each par
		particle[i].linewidth = particle[i].UpdateLineWidth(particle[i].last_spacing_threshold,MIN_SP_VIABLE,MAX_SP_VIABLE);
	}
	head_par = 0;
	rear_par = numpt-1;
}


//-----------------------------------------------------------
//
// Setup particle list for open area
//	in open area, pars moves to a main predefined direction
//-----------------------------------------------------------
void ParticleGroups::Setup_parlist_forg(ofVec2f* openEdgechain, int openpar_numpt, SpacingMap m_spacing)
{
	cout<<"Setting up the parlist for open areas..."<<endl;
	Is_Foreground = true;	// this par group will work for open area
	numpt = openpar_numpt;
	beacon_num = numpt;
	for(int i=0; i<numpt; i++)
	{
		particle[i].id = i;
		particle[i].beacon_id = i;
		particle[i].pos = openEdgechain[i];
		particle[i].last_pos = particle[i].pos;
		// tangent direction of this par
		ofVec2f AB;		
		if(i==numpt-1)
			AB = openEdgechain[i]-openEdgechain[(i-1+numpt)%numpt];
		else
			AB = openEdgechain[i+1] - openEdgechain[i];
		AB.normalize();
		ofVec2f DIR = -AB.getPerpendicular();	// direction of this par
		float this_bearing = acos(DIR.dot(ofVec2f(0,1)));
		if(DIR.x<0)
			this_bearing = 2*PI-this_bearing;
		particle[i].bearing_vec = DIR;
		particle[i].bearing = this_bearing;
		particle[i].angle = degrees(particle[i].bearing);

		//particle[i].bearing = radians(DIRECTION_OPEN_AREA);
		//particle[i].bearing_vec = ofVec2f(sin(particle[i].bearing),cos(particle[i].bearing));
		//if(particle[i].bearing_vec.x<0)
		//	particle[i].bearing = 2*PI-particle[i].bearing;
		//particle[i].angle = degrees(particle[i].bearing);	// All particles move to right. (90 degrees)
		particle[i].speed = DEFAULT_SPEED;
		particle[i].last_tilt = particle[i].bearing;
		particle[i].tilt_limit = TILT_LIMIT;
		particle[i].tilt_control = 0;
		particle[i].Is_released = true;
		particle[i].Was_released = true;
		particle[i].Is_newborn = false;
		particle[i].Is_dying = false;
		particle[i].c = COLOR;

		if(i==0)	// for head particle
		{
			particle[0].prev = &particle[numpt-1];	// last par
			particle[0].next = &particle[(i+1)%numpt];	// next par
		}
		else
		{
			particle[i].prev = &particle[(i-1+numpt)%numpt];
			particle[i].next = &particle[(i+1)%numpt];
			particle[i].prev->next = &particle[i];
		}
		particle[i].last_spacing_threshold = m_spacing.GetBaseThreshold(particle[i].pos.x,particle[i].pos.y);	// set up base threshold for each par
		particle[i].linewidth = particle[i].UpdateLineWidth(particle[i].last_spacing_threshold,MIN_SP_VIABLE,MAX_SP_VIABLE);
	}
	head_par = 0;
	rear_par = numpt-1;
}


//--------------------------------------------------------------
//
// Setup the map of open area with the parlist
//	walk through the parlist of close area, mark
//	inner pixels as true, outside as false. For now
//  the map is imported by a preprocessed thresholding map
//--------------------------------------------------------------
void ParticleGroups::Setup_Foreground_map(ofVec2f* AllPixelsInChain, int allpixels_num)
{
	ofImage testImage;
	testImage.loadImage("chair_threshold.png");
	testImage.resize(ofGetWidth(),ofGetHeight());
	testImage.setImageType(OF_IMAGE_GRAYSCALE);
	unsigned char* testedgepixel = testImage.getPixels();

	Foreground_map = new bool[ofGetWidth()*ofGetHeight()];
	for(int i =0; i<ofGetWidth(); i++)
	{
		for(int j = 0; j<ofGetHeight(); j++)
		{
			if(testedgepixel[i+j*ofGetWidth()] ==0)
				Foreground_map[i+j*ofGetWidth()] = true;
			else // all pixels with color (not neccessary to be black) is included in the map
				Foreground_map[i+j*ofGetWidth()] = false;
		}
	}
	//make all the edge pixels as true;
	for(int i = 0; i<allpixels_num; i++)
	{
		int index = AllPixelsInChain[i].x + AllPixelsInChain[i].y*ofGetWidth();
		Foreground_map[index] = true;
	}
	cout<<"--Finished the map of foreground objects"<<endl;
}

void ParticleGroups::Setup_2ndForeground_map(ofVec2f* AllPixelsInChain, int allpixels_num)
{
	ofImage testImage;
	testImage.loadImage("lion.png");
	testImage.resize(ofGetWidth(),ofGetHeight());
	testImage.setImageType(OF_IMAGE_GRAYSCALE);
	unsigned char* testedgepixel = testImage.getPixels();

	Foreground_map = new bool[ofGetWidth()*ofGetHeight()];
	for(int i =0; i<ofGetWidth(); i++)
	{
		for(int j = 0; j<ofGetHeight(); j++)
		{
			if(testedgepixel[i+j*ofGetWidth()] !=0 && testedgepixel[i+j*ofGetWidth()] != 255)
				Foreground_map[i+j*ofGetWidth()] = true;
			else // all pixels with color (not neccessary to be black) is included in the map
				Foreground_map[i+j*ofGetWidth()] = false;
		}
	}
	//make all the edge pixels as true;
	//for(int i = 0; i<allpixels_num; i++)
	//{
	//	int index = AllPixelsInChain[i].x + AllPixelsInChain[i].y*ofGetWidth();
	//	Foreground_map[index] = true;
	//}
	cout<<"--Finished the 2nd map of foreground objects"<<endl;
}



//---------------------------------------------
//
// Display all particle traces
//
//---------------------------------------------
void ParticleGroups::DrawAll(ofImage* m_canvas)
{
	unsigned char* canvas_pixel = m_canvas->getPixels();
	for(int i=0; i<numpt; i++)
	{
		if(particle[i].Is_released)
		{
			//// If we want Particle Groups not crossing with each other, uncomment it.
			//if(!Is_Foreground && OutOfForeground(particle[i])|| Is_Foreground)	// if the background particles move inside foreground area, do not draw
			//{
			//	for(float x=particle[i].pos.x; x<particle[i].pos.x+particle[i].linewidth && x<ofGetWidth(); x++)
			//	{
			//		for(float y=particle[i].pos.y; y<particle[i].pos.y+particle[i].linewidth && y<ofGetHeight();y++)
			//		{
			//			//m_canvas->setColor(int(floor(x+0.5)),int(floor(y+0.5)),particle[i].c);
			//			int index = int(floor(x+0.5))+int(floor(y+0.5))*ofGetWidth();
			//			if(index>=0 && index<ofGetHeight()*ofGetWidth())
			//				canvas_pixel[int(floor(x+0.5))+int(floor(y+0.5))*ofGetWidth()] = 0;
			//		}
			//	}
			//}

			//If Particle Groups need to cross with each other to have heavier tone, uncomment it.
			for(float x=particle[i].pos.x; x<particle[i].pos.x+particle[i].linewidth && x<ofGetWidth(); x++)
			{
				for(float y=particle[i].pos.y; y<particle[i].pos.y+particle[i].linewidth && y<ofGetHeight();y++)
				{
					//m_canvas->setColor(int(floor(x+0.5)),int(floor(y+0.5)),particle[i].c);
					int index = int(floor(x+0.5))+int(floor(y+0.5))*ofGetWidth();
					if(index>=0 && index<ofGetHeight()*ofGetWidth())
						canvas_pixel[int(floor(x+0.5))+int(floor(y+0.5))*ofGetWidth()] = 0;
				}
			}
		}
	}
}

//---------------------------------------------
//
// Main loop of the simulation
//
//---------------------------------------------
void ParticleGroups::Simulate(SpacingMap m_spacing, ofImage* m_canvas)
{
	if(!canstop)
	{
		BoundaryControl();
		Birth_Death(m_spacing);
		GridsCollisionKill();
		UpdateGrids();
		DrawAll(m_canvas);
		Particle* indexPar = &particle[head_par];
		int iter = 0;
		while(iter<numpt)	// start from the first particle till the last one
		{
			int thisparID = indexPar->id;	// mark this par's ID as the current one
			indexPar = indexPar->next;		// let the index pointer point to the next one
			TiltUpdate(&particle[thisparID]);	
			particle[thisparID].Update();	// update position, speed
			particle[thisparID].PushBackOrFwd(head_par, rear_par,Is_Foreground);
			if(!particle[thisparID].Is_dying)
			{
				particle[thisparID].advance();	// add random factors to direction
				particle[thisparID].Move2Middle(Is_Foreground,head_par,rear_par);	// coordinate movement
				TiltControl(&particle[thisparID]);
			}
			iter++;
		}
	// after the first update, create the grids
		if(!FillGrids)	
		{
			CreateGrids();
			FillGrids = true;
		}
	}
}

void ParticleGroups::TiltUpdate(Particle* thispar)
{
	thispar->tilt_control++;
	if(thispar->tilt_control>=thispar->tilt_limit)	// change the tilt direction every ... steps
	{
		thispar->last_tilt = -thispar->last_tilt;
	}
}

//-----------------------------------------------------
//
// If the tlit goes beyond the limit, reset it as zero
//
//-----------------------------------------------------
void ParticleGroups::TiltControl(Particle* thispar)
{
	if(thispar->tilt_control>=thispar->tilt_limit)
	{
		thispar->tilt_control = 0;
		thispar->tilt_limit = TILT_LIMIT;
	}
}


//---------------------------------------------
//
// Detect birth and death cases
//
//---------------------------------------------

//---------------------------------------------
void ParticleGroups::Birth_Death(SpacingMap m_spacing)
{
	bool continueLoop = true;
	Particle *index_par = &particle[head_par];// start from the first particle
	Particle *endpointA = index_par;	// mark the endpoint A as the first par
	do
	{
		Particle *endpointB = endpointA->next;	// mark the endpoint B as the next par
		Particle tempA = *endpointA;
		//----------------------------------------------------------
		// Compute the threshold and spacing viables for this pixel
		//----------------------------------------------------------
		// the average threshold of A and B's local thresholds
		float aver_threshold = 0.5 * (endpointA->last_spacing_threshold+ endpointB->last_spacing_threshold);
		float Apixel_threshold = endpointA->last_spacing_threshold;	// the threshold stored in pixels
		float Bpixel_threshold = endpointB->last_spacing_threshold;	
		if(endpointA->Is_released && endpointB->Is_released)
		{	// get the threshold of pixels at points A and B's positions; update the last_spacing stored in particles later
			Apixel_threshold = m_spacing.GetBaseThreshold(endpointA->pos.x,endpointA->pos.y); // the threshold at pixel A
			Bpixel_threshold = m_spacing.GetBaseThreshold(endpointB->pos.x,endpointB->pos.y); // the threshold at pixel B
		}

		// update the pars' info about Is_newborn before checking birth and death
		SetAsRegular(endpointA, endpointB, BRH_OFFSET*aver_threshold);	// use the average threshold computing with the OLD, NOT-UPDATED spacing_threshold
		//float tempab;
		//----------------------------------------------------------
		//	start checking the Birth and Death
		//----------------------------------------------------------
		if(!endpointA->Is_newborn && !endpointB->Is_newborn)
		{
			endpointA->UpdateSpacingTd(Apixel_threshold);// update its last_spacing_threshold
			aver_threshold = 0.5 * (endpointA->last_spacing_threshold + endpointB->last_spacing_threshold);	// update the average threshold
			float AB = distAB(endpointA,endpointB);
			if(AB<DTH_OFFSET*aver_threshold && (endpointA->Is_dying == endpointB->Is_dying))	
			{	// death
				if(!endpointA->Is_dying)	// mark them as dying...
				{
					endpointA->Is_dying = true;
				}
				if(!endpointB->Is_dying)
				{
					endpointB->Is_dying = true;
				}
				if(tempA.id == numpt-1)	// if A is the last one (A will be deleted)
				{
					tempA = *endpointB;
					death(endpointA, endpointB);
					endpointA = &tempA;
				}
				else
					death(endpointA, endpointB);
			}
			// if the distance goes beyond a limit, then release a new particle
			else if(AB>BRH_OFFSET*aver_threshold && !endpointA->Is_dying && !endpointB->Is_dying && endpointA->Is_released && endpointB->Is_released)
			{	// birth
				birth(endpointA, endpointB, m_spacing);	
			}
		}
		endpointA = endpointA->next;// move the Apointer to the next par
		//if(!Is_Foreground && !(endpointA->id==head_par && endpointA->prev->id==rear_par))
		//	continueLoop = true;	// for the contour area case
		//else 
		if(/*Is_Foreground &&*/endpointA->id!=rear_par)
			continueLoop = true;
		else
			continueLoop = false;
	} 
	while(continueLoop); // run until the continueLoop marked as false
}



//---------------------------------------------
//
// birth control
//
//---------------------------------------------
void ParticleGroups::birth(Particle* endpointA, Particle* endpointB, SpacingMap m_spacing)
{
	//if(numpt==CHAIN_NUM-1)
	//	cout<<"Warning: numpt is "<<numpt<<" now."<<endl;
	particle[numpt].pos = endpointA->pos;	// create from the A's position
	particle[numpt].last_pos = particle[numpt].pos;
	particle[numpt].id = numpt;
	particle[numpt].beacon_id = beacon_num;

	// SHOULD THE NEW DIR POINTS DIFFERENT FOT PARS IN OPEN AREAS???????????????????
	//---------------------------------------------------------------------------------
	particle[numpt].bearing_vec = (endpointA->bearing_vec + endpointB->bearing_vec)/2;	// the new direction is the average one
	particle[numpt].bearing_vec.normalize();
	particle[numpt].bearing = acos(particle[numpt].bearing_vec.dot(ofVec2f(0,1)));
	if(particle[numpt].bearing_vec.x<0)
		particle[numpt].bearing = 2*PI-particle[numpt].bearing;
	particle[numpt].angle = degrees(particle[numpt].bearing);
					
	//----------------------------------------------------------------------------------

	particle[numpt].speed = DEFAULT_SPEED;
	particle[numpt].prev = endpointA;
	particle[numpt].next = endpointB;
	particle[numpt].prev->next = &particle[numpt];
	particle[numpt].next->prev = &particle[numpt];
	particle[numpt].parentbeacon = endpointA->beacon_id;
	particle[numpt].prev->childbeacon = beacon_num;
	particle[numpt].last_tilt = particle[numpt].bearing;
	particle[numpt].tilt_limit = TILT_LIMIT;
	particle[numpt].tilt_control = 0;
	particle[numpt].Is_newborn = true;
	if(particle[numpt].pos.x < 0 || particle[numpt].pos.x >= ofGetWidth() || particle[numpt].pos.y <0 || particle[numpt].pos.y >= ofGetHeight())
		particle[numpt].Is_released = false;
	else if(Is_Foreground && OutOfForeground(particle[numpt]))	// check for open area
		particle[numpt].Is_released = false;
	else
		particle[numpt].Is_released = true;
	particle[numpt].Was_released = particle[numpt].Is_released;
	particle[numpt].Is_dying = false;
	particle[numpt].c = COLOR;

	// if a par is born between head and rear, mark this par as a rear par
	// Note: donot create new pars between head and rear if it is in an open-area mode
	//---------------------------------------------------------------------------------
	if(particle[numpt].prev->id == rear_par && particle[numpt].next->id == head_par)	
		rear_par = numpt;
	//---------------------------------------------------------------------------------

	if(particle[numpt].Is_released)
	{
		// the new spacing threshold will be the average of A, B and the threshold stored in current pixel
		particle[numpt].last_spacing_threshold = (endpointA->last_spacing_threshold + endpointB->last_spacing_threshold + m_spacing.GetBaseThreshold(particle[numpt].pos.x,particle[numpt].pos.y))/3;
		particle[numpt].linewidth = particle[numpt].UpdateLineWidth(m_spacing.GetBaseThreshold(particle[numpt].pos.x,particle[numpt].pos.y),MIN_SP_VIABLE,MAX_SP_VIABLE);
	}
	else
	{
		// the new spacing threshold will be the average of A and B only
		particle[numpt].last_spacing_threshold = (endpointA->last_spacing_threshold + endpointB->last_spacing_threshold)/2;
		particle[numpt].linewidth = particle[numpt].UpdateLineWidth((endpointA->linewidth + endpointB->linewidth)/2,MIN_SP_VIABLE,MAX_SP_VIABLE);
	}
		// insert this particle into the grid
	if(FillGrids)
		grid.insertPar(&particle[numpt]);
	numpt++;
	beacon_num++;
}


//---------------------------------------------
//
// death control
//
//---------------------------------------------
void ParticleGroups::death(Particle* endpointA, Particle* endpointB)
{
	// move A to the middle
	endpointA->pos.x = (endpointA->pos.x+endpointB->pos.x)/2;
	endpointA->pos.y = (endpointA->pos.y+endpointB->pos.y)/2;
	endpointA->bearing_vec = endpointA->bearing_vec + endpointB->bearing_vec;	// A now will point to the average direction
	endpointA->bearing_vec.normalize();
	endpointA->bearing = acos(endpointA->bearing_vec.dot(ofVec2f(0,1)));
	if(endpointA->bearing_vec.x<0)
		endpointA->bearing = 2*PI-endpointA->bearing;
	endpointA->angle = degrees(endpointA->bearing);
	endpointA->speed = DEFAULT_SPEED;
	kill(endpointB->id);
	endpointA->Is_dying = false;
	endpointB->Is_dying = false;
}


//---------------------------------------------
//
// If particles hit boundary, remove them from
// link and set them as rest.
//
//---------------------------------------------
void ParticleGroups::BoundaryControl()
{
	if(Is_Foreground)
		BoundaryControl_Foreground();
	else
		BoundaryControl_Window();
}



//--------------------------------------------------
//
// Check if particles move outside of the open area
//
//--------------------------------------------------
void ParticleGroups::BoundaryControl_Foreground()
{
	if(Is_Foreground)
	{
		Particle* indexPar = &particle[head_par];
		int iter = 0;
		while(iter<numpt)
		{
			int indexID = indexPar->id;
			indexPar = indexPar->next;
			// if this par moves out of open areas, do not kill, instead make them move in backstage
			// delete it from grids
			if(OutOfForeground(particle[indexID]))
			{
				if(particle[indexID].Is_released)
				{
					particle[indexID].Was_released = particle[indexID].Is_released;
					particle[indexID].Is_released = false;	// if false, don't draw it, just deactive the birth and death
				}
				if(FillGrids && particle[indexID].Was_released &&!particle[indexID].Is_released)// if the par just moves out of the open area
					grid.deletePar(&particle[indexID]);// delete it from the quadtree
			}
			// if the particle moves back to open area
			// set it as released again and insert it back to grid again
			else
			{
				if(!particle[indexID].Is_released)	// keep checking all particles, if one moved out of open area and moved back, activate it again
				{
					particle[indexID].Was_released = particle[indexID].Is_released;
					particle[indexID].Is_released = true;
					if(FillGrids&& !particle[indexID].Was_released && particle[indexID].Is_released) // wasn't released and now is
						grid.insertPar(&particle[indexID]);
				}
			}
			iter++;
		}
	}
	CanStop();
}


bool ParticleGroups::OutOfForeground(Particle thispar)
{
	if(thispar.OutOfBoudaryKill()) return true;
	// find the par in OpenArea_map
	int index = floor(thispar.pos.x+0.5)+floor(thispar.pos.y+0.5)*ofGetWidth();
	if(Foreground_map[index])
		return false;	// current position is inside the open area
	else
		return true;
}

//---------------------------------------------
//
// Check if particles move outside of window
//
//---------------------------------------------
void ParticleGroups::BoundaryControl_Window()
{
	if(!Is_Foreground)
	{
		Particle* indexPar = &particle[head_par];
		int iter = 0;
		while(iter < numpt)	// start from the first particle
		{
			int indexID = indexPar->id;
			indexPar = indexPar->next;
			// if this par moves out of boundary, do not kill, instead make them move in backstage
			// delete it from grids
			if(particle[indexID].OutOfBoudaryKill())	
			{
				particle[indexID].Was_released = particle[indexID].Is_released;
				particle[indexID].Is_released = false;	// if false, don't draw it, just deactive the birth and death
				if(FillGrids && particle[indexID].Was_released &&!particle[indexID].Is_released)
					grid.deletePar(&particle[indexID]);// delete it from the quadtree
			}
			// if the particle moves back to screen
			// set it as released again and insert it back to grid again
			else
			{
				if(!particle[indexID].Is_released)		// keep checking all particles, if one moved out of boudary and move back, activate it again
				{
					particle[indexID].Was_released = particle[indexID].Is_released;
					particle[indexID].Is_released = true;
					if(FillGrids&& !particle[indexID].Was_released && particle[indexID].Is_released)
						grid.insertPar(&particle[indexID]);
				}
			}
			iter++;
		}
	}
	CanStop();
}


//---------------------------------------------
//
// Decide if the simulation can be stoped 
//
//---------------------------------------------
bool ParticleGroups::CanStop()
{
	bool have_active_pars = false;	// Run until all the particles are not released
	for(int i=0; i<numpt; i++)
	{
		have_active_pars = have_active_pars || particle[i].Is_released;
		if(have_active_pars)	// if there is any alive particle
			return false;
		else if(Is_Foreground && !particle[i].OutOfBoudaryKill())		// else if there is no alive particle but some of them are still inside the window
			return false;
	}
	canstop = true;
	return true;
}


bool ParticleGroups::GetStopSignal()
{
	return canstop;
}


//---------------------------------------------
//
// Kill a certain particle
//
//---------------------------------------------
void ParticleGroups::kill(int indexID)
{
	// break the link for this slot
	particle[indexID].prev->next = particle[indexID].next;
	particle[indexID].next->prev = particle[indexID].prev;
	Particle tempKilledPar = particle[indexID];	// a temporary particle to pass to grid to delete
	
	if(particle[numpt-1].id != indexID)	// if it is not a last particle (memory location) to be killed
	{
		if(rear_par == numpt-1)		// if the rear one (visual location) is the last one (memory location), make rear_par point to indexID par
			rear_par = indexID;
		else if(rear_par == indexID)
			rear_par = particle[indexID].prev->id;
		if(head_par == numpt-1)
			head_par = indexID;
		else if(head_par == indexID)		// if it is the head one should be killed, make head_par point to the next par
			head_par = particle[indexID].next->id;
		
		// copy the last one to this slot
		particle[indexID] = particle[numpt-1];
		particle[indexID].id = indexID;
		// relink the last one's pointers
		particle[numpt-1].next->prev = &particle[indexID];
		particle[numpt-1].prev->next = &particle[indexID];
		if(rear_par == numpt-1)	
			rear_par = indexID;
		if(head_par == numpt-1)
			head_par = indexID;
	}
	
	else // the one that should be killed is a last particle (memory location)
	{
		// update the rear_par
		if(rear_par == indexID)
			rear_par = particle[indexID].prev->id;	
		// update the head_par
		if(head_par == indexID)
			head_par = particle[indexID].next->id;
	}

	// delete the last one
	particle[numpt-1] = NULL;	// killed it

	// update the ids in the grids
	if(FillGrids)
	{
		grid.deletePar(&tempKilledPar);	// delete the particle that was supose to be killed
		grid.idUpdate(numpt-1,particle[indexID]);	// update the last particle's id in the grid
	}
	numpt--;
	////cout<<"numpt: "<<numpt<<"; beacon_num: "<<beacon_num<<endl;
} 

//---------------------------------------------
//
// Set the newborn particle back to regular status
//
//---------------------------------------------
void ParticleGroups::SetAsRegular(Particle* A, Particle* B, float birth_dist)
{
	float AB = distAB(A,B);
	if(B->Is_newborn && distAB(A,B)>=(birth_dist/2))		// if larger than the birth_dist, set the new born ones as regular ones 
		B->Is_newborn = false;
}

//---------------------------------------------
//
// Insert all the particles in the quadtree
//
//---------------------------------------------
void ParticleGroups::CreateGrids()
{
	grid.cleargrid();
	if(numpt!=0)
	{
		for(int i = 0; i<numpt; i++)
		{
			if(particle[i].Is_released)
				if(!grid.insertPar(&particle[i]))
					cout<<"Warning: Cannot insert particle "<<particle[i].id<<" when initializing the grids"<<endl;
		}
	}
}


//---------------------------------------------
//
// Update the quadtree
//
//---------------------------------------------
void ParticleGroups::UpdateGrids()
{
	// Only update the grids when the it is created
	if(FillGrids)
	{
		for(int i=0; i<numpt; i++)
		{
			grid.UpdatePar(&particle[i]);
		}
	}
}





//---------------------------------------------
//
// Detect the collision
//
//---------------------------------------------
void ParticleGroups::GridsCollisionKill()
{
	if(FillGrids)
	{
		// go through all the pars
		for(int i=0; i<numpt; i++)	
		{
			if(particle[i].Is_released)	// only check released pars because when move out of screen,it cannot be found in gird
			{
				// get the parlist that need to kill from grid
				vector<Particle> parlist;
				parlist = grid.CollisionDetection(particle[i]);

				// kill all the pars which are on the list
				if(parlist.size()>0)
				{
					for(int index=0; index<parlist.size();index++)
					{
						int collisionID = parlist[index].id;
						if(particle[collisionID].Is_released)	// in case some pars in the list has been killed already
						{
							if(collisionID != particle[i].id)	// do not kill it self now, in case of double kill
							{
								if(!Is_Foreground)
									kill(collisionID);	// kill the par in particle group and erase from grid
								else if(Is_Foreground && collisionID!=head_par && collisionID!=rear_par)
									kill(collisionID); // In an open-area case, do not kill head par and rear par
							}
						}
					}
					// kill itself also
					if(particle[i].Is_released)
					{
						if(!Is_Foreground)
							kill(particle[i].id);
						else if(Is_Foreground && i!=head_par && i!=rear_par)
							kill(particle[i].id);
					}
				}
			}
		}
	}
}


Particle ParticleGroups::FindPar(float x, float y)
{
	Particle nullpar;
	for(int i=0; i<particle.size();i++)
	{
		if(particle[i].pos.x>=x-5 && particle[i].pos.x<=x+5 && particle[i].pos.y>=y-5 && particle[i].pos.y<=y+5)
			return particle[i];
	}
	return nullpar;
}