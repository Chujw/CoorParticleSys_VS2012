#include "GridCollisionDetection.h"



GridCollisionDetection::GridCollisionDetection(int Row, int Col)
{
	row_num = Row; 
	col_num = Col; 
	gridblock_w=ofGetWidth()/col_num; 
	gridblock_h = ofGetHeight()/row_num; 
	grids.resize(Row*Col);

	for(int j =0; j<row_num; j++)
	{
		for(int i =0; i<col_num; i++)
		{
			// init the gris x and y
			grids[i+j*col_num].x = i*gridblock_w;
			grids[i+j*col_num].y = j*gridblock_h;
			grids[i+j*col_num].id = i+j*col_num;
		}
	}
	//init the grid_pixels
	grid_pixels = new int[ofGetWidth()*ofGetHeight()];

}

//--------------------------------------------------
//
// remove all the particles in the grid
//
//--------------------------------------------------
void GridCollisionDetection::cleargrid()
{
	for(int i=0; i<grids.size(); i++)
	{
		grids[i].grid_pars.clear();
	}
}


//--------------------------------------------------
//
// Get the grid id based on the coordinate
//
//--------------------------------------------------
int GridCollisionDetection::getIndexFromPos(ofVec2f pos)
{
	////cout<<"Geting index info from position......"<<endl;
	if(pos.x < 0 || pos.x >= ofGetWidth() || pos.y < 0 || pos.y >= ofGetHeight())	// if the position is out of the screen, return false
		return -1;

	float X, Y;
	X = ceil(pos.x / gridblock_w)-1;
	Y = ceil(pos.y / gridblock_h)-1;
	int index = X + Y * col_num;
	if(index>=grids.size())
		return -1;
	return index;
}


//--------------------------------------------------
//
// Get the grid id by traversing the whole grids
//
//--------------------------------------------------
bool GridCollisionDetection::getIndexTraversal(Particle thispar, int* realparindex, int* realgridindex)
{
	if(!thispar.OutOfBoudaryKill())
	{
		for(int i=0; i<grids.size(); i++)
		{
			for(int n=0; n<grids[i].grid_pars.size(); n++)
			{
				if(grids[i].grid_pars[n].id == thispar.id)
				{
					*realparindex = n;
					*realgridindex = i;
					return true;	// found it
				}
			}
		}
	}
	return false;	// cannot find it anywhere
}


//--------------------------------------------------
//
// Check if the particle is in the given grid
//
//--------------------------------------------------
int GridCollisionDetection::InGrid(int index, Particle thispar)
{
	//cout<<"Checking if it is in grid..."<<endl;
	if(index>=0)
	{
		//cout<<"grids.size = "<<grids.size()<<endl;
		for(int i=0; i<grids[index].grid_pars.size(); i++)
		{
			if(thispar.id == grids[index].grid_pars[i].id)
			{
				//cout<<"In grid "<<index<<endl;
				return i;	// the particle exist
			}
		}
	}
	////cout<<"Not in grid"<<endl;
	return -1;	// the particle dose not in the given grid
}



//--------------------------------------------------
//
// insert the particle into a grid
//
//--------------------------------------------------
bool GridCollisionDetection::insertPar(Particle* thispar)
{
	//// find its position in grid
	//int index = getIndexFromPos(thispar);
	//if(thispar.grid_id==-1 && thispar.Is_released)	// if the particle has not been inserted
	//{		
	//	if(InGrid(index,thispar))	// if found it in grid
	//	{
	//		cout<<"Warning: particle "<<thispar.id<<" hasn't been inserted, but it does appear in grid"<<index<<endl;
	//		thispar.grid_id = index;
	//		return false;
	//	}
	//	else // if could not find it, insert the particle
	//	{
	//		grids[index].grid_pars.push_back(thispar);	
	//		thispar.grid_id = index;
	//		return true;
	//	}
	//}
	//else if(thispar.grid_id>=0 && thispar.Is_released)	// if the particle has been inserted
	//{
	//	if(thispar.grid_id == index) // it exist in the recorded grid rightfully
	//		return false;	
	//	else	// could not find it in its recorded grid
	//	{
	//		// if the particle is no longer in this grid
	//		grids[index].grid_pars.push_back(thispar);	// insert the particle
	//		thispar.grid_id = index;
	//		return true;
	//	}
	//}
	if(thispar->Is_released)
	{
		int index = getIndexFromPos(thispar->pos); // find the grid according to its position
		if(index>=0 && index < grids.size())
		{
			////cout<<"Insert particle "<<thispar->id<<" in grid "<<index<<endl;
			if(grids[index].grid_pars.size() == grids[index].grid_pars.capacity()-1)
				cout<<"the grid's number of particles reached the max..."<<endl;
			if(grids[index].grid_pars.empty() || grids[index].grid_pars.size() == grids[index].grid_pars.capacity()-1)
				grids[index].grid_pars.reserve(REG_MAX_PARTICLES+grids[index].grid_pars.capacity());
			grids[index].grid_pars.push_back(*thispar);

			//int px_index = thispar->pos.x + thispar->pos.y * ofGetWidth();	// leave the beacon to the grid pixels
			////cout<<"pixel "<<thispar->pos.x<<" , "<<thispar->pos.y<<" marked beacon id "<<thispar->beacon_id<<endl;
			//grid_pixels[px_index] = thispar->beacon_id;
			thispar->grid_id = index;	// update this particle's grid id
			////cout<<endl;


				int size = 0;
				for(int i=0; i<10000; i++)
				{
					if(size<grids[i].grid_pars.size())
					{
						size = grids[i].grid_pars.size();
					}
				}
			return true;
		}
	}
	////else
	////	// it is a newborn particle which is created outside of screen
	////	cout<<"Warning: cannot insert particle "<<thispar->id<<" because it is not released... it is "<<thispar->Is_newborn<<" newborn"<<endl;
	return false;
}


//--------------------------------------------------
//
// Check if the particle need to be updated
//
//--------------------------------------------------
bool GridCollisionDetection::needUpdate(Particle thispar)
{
	
	// if the particle move out of screen or move back
	//if(thispar.Was_released != thispar.Is_released)	
		//return true;

	// if the particle is released and move out of last recorded grid
	if(thispar.Is_released)
	{
		////cout<<"Checking if needs update......"<<endl;
		int curIndex = getIndexFromPos(thispar.pos);
		int lastIndex = getIndexFromPos(thispar.last_pos);
		if(curIndex!=lastIndex)	
			return true;
	}

	// if the particle is NOT released or HAS NOT moved too much
	return false;
}


//--------------------------------------------------
//
// Delete a particle from a grid
//
//--------------------------------------------------
bool GridCollisionDetection::deletePar(Particle* thispar)
{
	////cout<<"Deleting particle "<<thispar->id<<" in grid......"<<endl;
	int parindex = InGrid(thispar->grid_id,*thispar);	// get the particle index in the recorded grid
	int lastposindex = getIndexFromPos(thispar->last_pos);	// get the grid that the particle used to be in
	
	// if this particle is in the recorded grid rightfully
	if(parindex>=0)	
	{
					if(thispar->grid_id==-1 || parindex==-1)
						cout<<"ERROR!!!!!!!!!!!!"<<endl;
		////cout<<"Erasing particle "<<thispar->id<<" in recorded grid "<<thispar->grid_id<<endl;
		grids[thispar->grid_id].grid_pars.erase(grids[thispar->grid_id].grid_pars.begin()+parindex);
		thispar->grid_id = -1;
		return true;
	}

	// find the particle first from the grid that it was in
	else if(lastposindex >= 0)
	{
		////cout<<"lastposindex = "<<lastposindex<<endl;
		int lastparindex = InGrid(lastposindex,*thispar);
		if(lastparindex >= 0)
		{
			////cout<<"Erasing particle "<<thispar->id<<" in last grid "<<lastposindex<<endl;
			grids[lastposindex].grid_pars.erase(grids[lastposindex].grid_pars.begin()+lastparindex);
			thispar->grid_id = -1;
			return true;
		}
	}

	// try to find it from all the grids traversingly
	else
	{
		if(thispar->Is_released)
		{
			int realparindex = -1;
			int realgridindex = -1;
			if(getIndexTraversal(*thispar, &realparindex, &realgridindex))	// if can find it somewhere
			{
				////cout<<"Erasing particle "<<thispar->id<<" in a found grid "<<realgridindex<<endl;
				grids[realgridindex].grid_pars.erase(grids[realgridindex].grid_pars.begin()+realparindex);
				thispar->grid_id = -1;
				return true;
			}
			else	// cannot find it
			{
				thispar->grid_id = -1;
				////cout<<"cannot delete "<<thispar->id<<" from nowhere"<<endl;
				return false;	// cannot delete it from nowhere
			}
		}
	}
	return false;
}


//-----------------------------------------------------------------------------------------------
//
// Update the particle's information in grid. Delete first and insert it into a new right grid
//
//-----------------------------------------------------------------------------------------------
void GridCollisionDetection::UpdatePar(Particle* thispar)
{
	//-----------------------------------------------------
	// if the particle move across grids
	// find it, delete it and insert it to the grid
	//-----------------------------------------------------
	if(needUpdate(*thispar))	
	{
		if(thispar->Is_released)
		{
			//delete it first
			////cout<<"Updating particle "<<thispar->id<<" ..."<<endl;
			if(deletePar(thispar))
			{
				insertPar(thispar);	// insert it again
				// update the beacon id for the 4 pixels around thispar
				int px_index0 = floor(thispar->pos.x+0.5) + floor(thispar->pos.y+0.5) * ofGetWidth();	
				int px_index1 = floor(thispar->pos.x+1+0.5) + floor(thispar->pos.y+0.5) * ofGetWidth();
				int px_index2 = floor(thispar->pos.x+0.5) + floor(thispar->pos.y+1+0.5) * ofGetWidth();
				int px_index3 = floor(thispar->pos.x+1+0.5) + floor(thispar->pos.y+1+0.5) * ofGetWidth();
				
				//----------------------------------------------------------------------------------------
				// Update beacon ids even for pars out of open areas but still inside of window boundaries
				// However, when it is in open area mode, when pars move out of open areas, 
				// they will be marked as not_released, and will not get in this function
				//----------------------------------------------------------------------------------------
				////cout<<"pixel "<<thispar->pos.x<<" , "<<thispar->pos.y<<" marked beacon id "<<thispar->beacon_id<<endl;
				if(px_index0>=0 && px_index0<ofGetWidth()*ofGetHeight() && grid_pixels[px_index0]<0)
					grid_pixels[px_index0] = thispar->beacon_id;
				if(px_index1>=0 && px_index1<ofGetWidth()*ofGetHeight() && grid_pixels[px_index1]<0)
					grid_pixels[px_index1] = thispar->beacon_id;
				if(px_index2>=0 && px_index2<ofGetWidth()*ofGetHeight() && grid_pixels[px_index2]<0)
					grid_pixels[px_index2] = thispar->beacon_id;
				if(px_index3>=0 && px_index3<ofGetWidth()*ofGetHeight() && grid_pixels[px_index3]<0)
					grid_pixels[px_index3] = thispar->beacon_id;
			}
			else
			{
				cout<<endl;
				////cout<<"Warning: Cannot update (delete) released particle "<<thispar->id<<" because cannot find it in any grid. Now insert it..."<<endl;
				// update the beacon id for the pixel
				int px_index0 = floor(thispar->pos.x+0.5) + floor(thispar->pos.y+0.5) * ofGetWidth();	
				int px_index1 = floor(thispar->pos.x+1+0.5) + floor(thispar->pos.y+0.5) * ofGetWidth();
				int px_index2 = floor(thispar->pos.x+0.5) + floor(thispar->pos.y+1+0.5) * ofGetWidth();
				int px_index3 = floor(thispar->pos.x+1+0.5) + floor(thispar->pos.y+1+0.5) * ofGetWidth();	
				////cout<<"pixel "<<thispar->pos.x<<" , "<<thispar->pos.y<<" marked beacon id "<<thispar->beacon_id<<endl;

				// only update grid cells that have not been taken yet
				if(px_index0>=0 && px_index0<ofGetWidth()*ofGetHeight() && grid_pixels[px_index0]<0)
					grid_pixels[px_index0] = thispar->beacon_id;
				if(px_index1>=0 && px_index1<ofGetWidth()*ofGetHeight() && grid_pixels[px_index1]<0)
					grid_pixels[px_index1] = thispar->beacon_id;
				if(px_index2>=0 && px_index2<ofGetWidth()*ofGetHeight() && grid_pixels[px_index2]<0)
					grid_pixels[px_index2] = thispar->beacon_id;
				if(px_index3>=0 && px_index3<ofGetWidth()*ofGetHeight() && grid_pixels[px_index3]<0)
					grid_pixels[px_index3] = thispar->beacon_id;
				//else
				//	cout<<"Warning: Cannot update beacon because the par moves out of Boundary"<<endl;
				insertPar(thispar);
				////cout<<endl;
			}
		}
		else	// if it is not released (maybe just moved out of boundary)
		{
			if(deletePar(thispar))
				cout<<"Warning: A not released particle "<<thispar->id<<" was still inside a grid. Please check it."<<endl;
		}
	}
	//-----------------------------------------------------
	// if the particle just moves inside a same grid
	// find it and update the information stored in grid
	//-----------------------------------------------------
	else if(thispar->Is_released)
	{
		int index = InGrid(thispar->grid_id,*thispar);
		if(index >= 0)
		{
			//cout<<"Updating the position of particle "<<thispar->id<<endl; 
			int px_index0 = floor(thispar->pos.x+0.5) + floor(thispar->pos.y+0.5) * ofGetWidth();	
			int px_index1 = floor(thispar->pos.x+1+0.5) + floor(thispar->pos.y+0.5) * ofGetWidth();
			int px_index2 = floor(thispar->pos.x+0.5) + floor(thispar->pos.y+1+0.5) * ofGetWidth();
			int px_index3 = floor(thispar->pos.x+1+0.5) + floor(thispar->pos.y+1+0.5) * ofGetWidth();
			grids[thispar->grid_id].grid_pars[index] = *thispar;	// update the par in grid

			if(px_index0>=0 && px_index0<ofGetWidth()*ofGetHeight() && grid_pixels[px_index0]!= thispar->beacon_id && grid_pixels[px_index0]<0)
			{
				//cout<<"pixel "<<thispar->id<<" ("<<floor(thispar->pos.x+0.5)<<" , "<<floor(thispar->pos.y+0.5)<<") marked beacon id "<<thispar->beacon_id<<endl;
				grid_pixels[px_index0] = thispar->beacon_id;	// update the beacon id
			}

			if(px_index1>=0 && px_index1<ofGetWidth()*ofGetHeight() && grid_pixels[px_index1]!= thispar->beacon_id && grid_pixels[px_index1]<0)
			{
				//cout<<"pixel "<<thispar->id<<" ("<<floor(thispar->pos.x+0.5)<<" , "<<floor(thispar->pos.y+0.5)<<") marked beacon id "<<thispar->beacon_id<<endl;
				grid_pixels[px_index1] = thispar->beacon_id;	// update the beacon id
			}

			if(px_index2>=0 && px_index2<ofGetWidth()*ofGetHeight() && grid_pixels[px_index2]!= thispar->beacon_id && grid_pixels[px_index2]<0)
			{
				//cout<<"pixel "<<thispar->id<<" ("<<floor(thispar->pos.x+0.5)<<" , "<<floor(thispar->pos.y+0.5)<<") marked beacon id "<<thispar->beacon_id<<endl;
				grid_pixels[px_index2] = thispar->beacon_id;	// update the beacon id
			}

			if(px_index3>=0 && px_index3<ofGetWidth()*ofGetHeight() && grid_pixels[px_index3]!= thispar->beacon_id && grid_pixels[px_index3]<0)
			{
				//cout<<"pixel "<<thispar->id<<" ("<<floor(thispar->pos.x+0.5)<<" , "<<floor(thispar->pos.y+0.5)<<") marked beacon id "<<thispar->beacon_id<<endl;
				grid_pixels[px_index3] = thispar->beacon_id;	// update the beacon id
			}

			//else
			//	cout<<"Warning: Cannot update beacon because the par moves out of Boundary"<<endl;
		}
	}
}

//-----------------------------------------------------------------------------------
//
// Update the id for particles if the id has been changed
//
//-----------------------------------------------------------------------------------
bool GridCollisionDetection::idUpdate(int old_id, Particle curPar)
{
	////cout<<"idUpdate"<<endl;
	if(curPar.grid_id>=0 && old_id >=0)
	{
		for(int i = 0; i<grids[curPar.grid_id].grid_pars.size(); i++)
		{
			if(grids[curPar.grid_id].grid_pars[i].id == old_id)
			{
				grids[curPar.grid_id].grid_pars[i].id = curPar.id;
				////cout<<"Updated id for "<<old_id<<", now is "<<curPar.id<<endl;
				return true;
			}
		}
	}
	else if(curPar.Is_released)
		////cout<<"Warning: Cannot update id for particle "<<curPar.id<<endl;
	return false;
}


//-----------------------------------------------------------------------
//
// Main entrance of collision detection, search all 9 grids for one par
//
//-----------------------------------------------------------------------
vector<Particle> GridCollisionDetection::CollisionDetection(Particle thispar)
{
	////cout<<"collisionDetection......"<<endl;
	//if(thispar.beacon_id == 706)
	//	cout<<"here"<<endl;
	vector<Particle> parlist;
	// find the par
	int thisparindex = InGrid(thispar.grid_id,thispar);
	//if(thispar.pos.x>=470 && thispar.pos.x<= 483 || thispar.id==116)
	//	cout<<endl;
	if(thisparindex>=0)	// if finds it
	{
		parlist.reserve(REG_MAX_PARTICLES/2);	// initialize parlist
		int grid_0 = thispar.grid_id-col_num-1;
		int grid_1 = thispar.grid_id-col_num;
		int grid_2 = thispar.grid_id-col_num+1;
		int grid_3 = thispar.grid_id-1;
		int grid_4 = thispar.grid_id;
		int grid_5 = thispar.grid_id+1;
		int grid_6 = thispar.grid_id+col_num-1;
		int grid_7 = thispar.grid_id+col_num;
		int grid_8 = thispar.grid_id+col_num+1;

		// Check 9 grids
		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_0);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_1);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_2);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_3);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_4);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_5);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_6);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_7);

		if(parlist.size()>=parlist.capacity()-1)
			parlist.reserve(REG_MAX_PARTICLES/2 + parlist.capacity());
		CheckCollisionInGrid(&parlist, thispar, grid_8);

		// check if the particle are crossing any taken pixel if could not find any collision in previous code
		//if(parlist.size()==0)
			CheckPixelInGrid(&parlist,thispar);
	}

	else
		cout<<"Warning: Cannot get the parlist for "<<thispar.id<<endl;
	return parlist;
}




//--------------------------------------------------------------------------------------
//
// update the parlist by checking the distance with other pars in a given grid
//
//--------------------------------------------------------------------------------------
void GridCollisionDetection::CheckCollisionInGrid(vector<Particle>* parlist, Particle thispar, int grid_id)
{
	// make sure the grid id is valid
	////cout<<"CheckCollisionInGrid......"<<endl;
	if(thispar.Is_released && grid_id>=0 && grid_id<grids.size())
	{
		int prev = thispar.prev->beacon_id;
		int next = thispar.next->beacon_id;
		for(int i=0;i<grids[grid_id].grid_pars.size();i++) // for all the pars inside the grid
		{
			float dist = thispar.last_pos.distance(grids[grid_id].grid_pars[i].pos);// now that the pos is updated so we should use last_pos
			int collisionID = grids[grid_id].grid_pars[i].beacon_id;
			// compare the distance with the predefined threshold
			if(dist<=COLLISION_KILL_DIST && collisionID!=thispar.beacon_id)	// do not compare with itself
			{
				bool donotkill = false;
				// check if it is one of the neighbors need to be killed
				if(collisionID == prev || collisionID == next)
				{
					if(collisionID == prev && thispar.Is_newborn)	// it itself is newborn
					{
						if(thispar.parentbeacon == prev && thispar.prev->childbeacon == thispar.beacon_id)	// goes apart from each other, do not kill
							donotkill = true;
					}
					else if(collisionID == next && thispar.next->Is_newborn)	// its next is newborn
					{
						if(thispar.childbeacon == next && thispar.next->parentbeacon == thispar.beacon_id)	// goes apart from each other
							donotkill = true;
					}
				}

				// mark the collision pars into parlist
				if(!donotkill)
					parlist->push_back(grids[grid_id].grid_pars[i]);
			}
		}
	}
}


//----------------------------------------------------------------
//
// Check if the particle is crossing a black pixel line
//
//----------------------------------------------------------------
bool GridCollisionDetection::CheckPixelInGrid(vector<Particle>* parlist, Particle thispar)
{
	// get the pixel position of thispar
	//int XinGrid = thispar.pos.x - grids[grid_id].x;
	//int YinGrid = thispar.pos.y - grids[grid_id].y;
	//int thispixel_id = XinGrid + YinGrid * gridblock_w;


	// get the pixel color inside the PIXEL_KILL_DIST

	//if(thispar.pos.y>=639 && thispar.pos.y<= 650)
	//	cout<<"found it......"<<endl;
	if(thispar.Is_released)
	{
		//for(int j=thispar.pos.y-PIXEL_KILL_DIST; j>=0 && j<=thispar.pos.y+PIXEL_KILL_DIST && j<ofGetHeight(); j++)
		//{
		//	for(int i=thispar.pos.x-PIXEL_KILL_DIST; i>=0 && i<=thispar.pos.x+PIXEL_KILL_DIST && i<ofGetWidth(); i++)
		//	{
		//		int pixel_id = i+j*ofGetWidth();
		//		if(grid_pixels[pixel_id]>=0 && grid_pixels[pixel_id]!=thispar.beacon_id)	// if find some pixels have others' id
		//		{
		//			cout<<"About to cross a pixel with beacon id: "<<grid_pixels[pixel_id]<<endl;
		//			parlist->push_back(thispar);	// only mark to kill itself, do not mark others
		//			return true;
		//		}
		//	}
		//}
		////cout<<"checking pixel in grid......"<<endl;
		int cur_pixel_id0 = floor(thispar.pos.x+0.5) + floor(thispar.pos.y+0.5) * ofGetWidth();
		int cur_pixel_id1 = floor(thispar.pos.x+1+0.5) + floor(thispar.pos.y+0.5) * ofGetWidth();
		int cur_pixel_id2 = floor(thispar.pos.x+0.5) + floor(thispar.pos.y+1+0.5) * ofGetWidth();
		int cur_pixel_id3 = floor(thispar.pos.x+1+0.5) + floor(thispar.pos.y+1+0.5) * ofGetWidth();
		bool HasCollided = false;
		if(cur_pixel_id0<0 || cur_pixel_id0>=ofGetWidth()*ofGetHeight())
			cur_pixel_id0 = -1;
		if(cur_pixel_id1<0 || cur_pixel_id1>=ofGetWidth()*ofGetHeight())
			cur_pixel_id1 = -1;
		if(cur_pixel_id2<0 || cur_pixel_id2>=ofGetWidth()*ofGetHeight())
			cur_pixel_id2 = -1;
		if(cur_pixel_id3<0 || cur_pixel_id3>=ofGetWidth()*ofGetHeight())
			cur_pixel_id3 = -1;

		HasCollided = Collide(cur_pixel_id0, thispar) || Collide(cur_pixel_id1, thispar)
					|| Collide(cur_pixel_id2, thispar)|| Collide(cur_pixel_id3, thispar);

		if(HasCollided)
		{
			//int par = findpar(grid_pixels[cur_pixel_id]);//---------------------------
			////cout<<"particle "<<thispar.id<<" about to cross with beacon"<<endl;
			parlist->push_back(thispar);	// only mark to kill itself, do not mark others
			return true;
		}
	}
	return false;
}

bool GridCollisionDetection::Collide(int cur_pixel_id, Particle thispar)
{
	bool donotkill = false;
	if(cur_pixel_id >=0 && grid_pixels[cur_pixel_id]>=0 && grid_pixels[cur_pixel_id]!=thispar.beacon_id)	// if find some pixels have others' id
	{
		if(thispar.next->beacon_id == grid_pixels[cur_pixel_id] && thispar.next->Is_newborn && thispar.childbeacon == thispar.next->beacon_id)
			donotkill = true;
		else if(thispar.prev->beacon_id == grid_pixels[cur_pixel_id] && thispar.Is_newborn && thispar.parentbeacon == thispar.prev->beacon_id)
			donotkill = true;
		else if(thispar.parentbeacon == thispar.prev->beacon_id && thispar.prev->beacon_id == grid_pixels[cur_pixel_id])
			donotkill = true;
		else if(thispar.parentbeacon == thispar.next->beacon_id && thispar.next->beacon_id == grid_pixels[cur_pixel_id])
			donotkill = true;

		if(!donotkill)
			return true;
	}
	return false;
}

int GridCollisionDetection::findpar(int beacon)
{
	////cout<<"Finding beacon......"<<endl;
	for(int i=0; i<grids.size();i++)
	{
		for(int index =0; index < grids[i].grid_pars.size(); index++)
		{
			if(grids[i].grid_pars[index].beacon_id == beacon)
			{
				int par = grids[i].grid_pars[index].id;
				return par;
			}
		}
	}
	return -1;
}

long int GridCollisionDetection::getbeacon(int pos)
{
	return grid_pixels[pos];
}