#include "ParticleManager.h"



void ParticleManager::Setup(ofImage* edgeImage, ofImage* srcImage)
{
	num_map = 0;
	num_group = 0;
	Mngr_canstop = false;
	FillObjects = false;
	Saved1stGroup = false;
	parGroup.resize(GROUP_NUM*2);
	edgemap.resize(GROUP_NUM*2);
	for(int i=0; i<GROUP_NUM; i++)
	{
		parGroup[i].Setup();	// setup parGroup's par_list size, head and rear pointer etc.
		num_group++;
	}
	CreateMap(edgeImage, srcImage);
	
}

void ParticleManager::CreateMap(ofImage* edgeImage, ofImage* srcImage)	
{
	edgemap[num_map].InitEdgeMap(num_map);	 // set up map ID & allocate EG_map
	edgemap[num_map].MakeImages();	// create srcImage and edge map; return the ofImage
	
	*srcImage = edgemap[num_map].GetsrcImage();	// store the edge map into its own class
	*edgeImage = edgemap[num_map].GetEdgeMap();

	m_spacing.SetupSpacingMaps(edgemap[num_map].GetGrayscaleMap());
	CreateEdgeChain();	// make close and open edge chains;


	num_map++;
	//return canvas_buf;
}


void ParticleManager::CreateEdgeChain()
{
	int bkgpar_numpt = 0;
	int linepar_numpt = 0;
	int second_linepar_numpt = 0;
	//------------------------------------------------------
	// For backgroud particles
	//------------------------------------------------------
	// Create backgroud edge chain
	m_bkgEdgechain = edgemap[num_map].MakeBkgEdgeChains(BKG_START,BKG_END,&bkgpar_numpt,m_spacing.GetDefaultSpacing());
	// pass the particle distribution list to groups
	parGroup[0].Setup_parlist_bkg(m_bkgEdgechain,bkgpar_numpt,m_spacing);	// closed contour particles list
	// setup the map of open areas
	int allpixels_num = 0;
	ofVec2f* AllPixelsInChain = edgemap[num_map].GetAllPixelsInChain(&allpixels_num);
	parGroup[0].Setup_Foreground_map(AllPixelsInChain, allpixels_num);
	//------------------------------------------------------
	// For particles in open areas
	//------------------------------------------------------
	if(!edgemap[num_map].MakeOpenEdgeChainsFromThreadEdge(EDGE_START,EDGE_END,&linepar_numpt,m_spacing.GetDefaultSpacing()))
	{
		parGroup.erase(parGroup.begin()+1);
		num_group--;
	}
	else
	{	// get the open edge chain list
		m_lineEdgechain = edgemap[num_map].GetForgEdgeChain();// will change the m_lineEdgechain
		parGroup[1].Setup_parlist_forg(m_lineEdgechain,linepar_numpt,m_spacing);	
		parGroup[1].Setup_Foreground_map(AllPixelsInChain, allpixels_num);
	}
	////----------------------------------------------------
	//// Make a third group with different directions
	////----------------------------------------------------
	//edgemap[num_map].MakeOpenEdgeChainsFromThreadEdge(SECOND_EDGE_START,SECOND_EDGE_END,&second_linepar_numpt,m_spacing.GetDefaultSpacing());
	//m_lineEdgechain = edgemap[num_map].GetForgEdgeChain();
	//parGroup[2].Setup_parlist_forg(m_lineEdgechain,second_linepar_numpt,m_spacing);	
	//parGroup[2].Setup_2ndForeground_map(AllPixelsInChain, allpixels_num);
}

//---------------------------------------------
//
// Main loop of the simulation
//
//---------------------------------------------
void ParticleManager::Simulate(ofImage* canvas)
{
	if(!Mngr_canstop)
	{
		//// uncomment it if need a third group
		//if(!parGroup[2].GetStopSignal())
		//	parGroup[2].Simulate(m_spacing, canvas);
		
		if(!parGroup[1].GetStopSignal())
			parGroup[1].Simulate(m_spacing, canvas);

		else if(!parGroup[0].GetStopSignal())
		{
			if(!Saved1stGroup)
			{
			ofImage saveimage;
			saveimage.clone(*canvas);
			saveimage.saveImage("1stGroup.png");
			Saved1stGroup = true;
			}
			parGroup[0].Simulate(m_spacing, canvas);
		}
		else
		{
			Mngr_canstop = true;
			ofImage saveimage;
			saveimage.clone(*canvas);
			saveimage.saveImage("FinalResult.png");
		}
	}
}

bool ParticleManager::canstop()
{
	return Mngr_canstop;
}




