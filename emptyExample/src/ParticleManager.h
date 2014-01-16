#include "ofMain.h"
#include "ParticleGroups.h"
#include "EdgeMap.h"
#include "SpacingMap.h"


#define GROUP_NUM			2
#define XD					ofGetWidth()
#define YD					ofGetHeight()
#define LEYE_RADIUS			YD/6
#define LEYE_CENTER			ofVec2f(XD/4, YD/3)
#define LEYE_START_POINT	ofVec2f(XD/4-LEYE_RADIUS, YD/3)

#define EDGE_START		ofVec2f(-XD/5,YD-1)
#define	EDGE_END		ofVec2f(XD+XD/5,YD-1)

#define SECOND_EDGE_START		ofVec2f(XD+1000,1163)
#define SECOND_EDGE_END		ofVec2f(3000,-853)

#define BKG_START		ofVec2f(1,-YD/3)
#define BKG_END			ofVec2f(1,YD+YD/3)

class ParticleManager{
private:
	vector<ParticleGroups> parGroup;
	vector<EdgeMap> edgemap;
	//ofVec2f* m_closeEdgechain;
	//ofVec2f* m_openEdgechain;
	ofVec2f* m_lineEdgechain;
	ofVec2f* m_bkgEdgechain;
	//EdgeMap edgeMap;
	bool Mngr_canstop;
	int num_group;
	int num_map;
	bool FillObjects;
	bool Saved1stGroup;
	//ofImage edgeImage_buf;
	SpacingMap m_spacing;
public:
	void Setup(ofImage* edgeImage, ofImage* srcImage);
	void CreateMap(ofImage* edgeImage, ofImage* srcImage);
	void CreateEdgeChain();
	//ofImage GetMap();
	//ofImage GetEdgeImage();
	void Simulate(ofImage* canvas);
	void Display();
	void StopGroup();
	void KillGroup();
	bool canstop();

};