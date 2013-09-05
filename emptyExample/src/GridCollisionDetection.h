#pragma once

#include "ofMain.h"
#include "Particle.h"

#define		REG_MAX_PARTICLES	10
#define		GRID_ROW			100
#define		GRID_COL			100
#define		COLLISION_KILL_DIST		3
//#define		PIXEL_KILL_DIST		1

struct Gridblock{
	int id;
	float x;
	float y;
	vector<Particle> grid_pars;
};



class GridCollisionDetection{
private:
	vector<Gridblock> grids;
	int* grid_pixels;
	float gridblock_w;
	float gridblock_h;
	int row_num;
	int col_num;

public:
	GridCollisionDetection(int Row = GRID_ROW, int Col = GRID_COL);
	void cleargrid(); // clear the grid
	bool insertPar(Particle* thispar);
	bool deletePar(Particle* thispar);

	int getIndexFromPos(ofVec2f pos);
	bool getIndexTraversal(Particle thispar, int* realparindex, int* realgridindex);
	int InGrid(int index, Particle thispar);

	void UpdatePar(Particle* thispar);
	bool needUpdate(Particle thispar);
	bool idUpdate(int old_id, Particle curPar);

	vector<Particle> CollisionDetection(Particle thispar);
	void CheckCollisionInGrid(vector<Particle>* parlist, Particle thispar, int grid_id);
	bool CheckPixelInGrid(vector<Particle>* parlist, Particle thispar);
	bool Collide(int cur_pixel_id, Particle thispar);
	int findpar(int beacon_id);
	long int getbeacon(int pos);
};
