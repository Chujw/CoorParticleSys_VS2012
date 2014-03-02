#pragma once

#include "ofMain.h"
#include "ParticleManager.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void SaveImage();

		int snapCounter;
		ofImage edgeImage;
		ofImage srcImage;
		ofImage canvas;
		ParticleManager m_par;
		ChainCode tempchain;
		ofVec2f tempcurPixel;
		bool bStop;					// stop 
		bool showEGmap;				// show edge map
		bool nowSaveImage;
		bool pdfmaker;
		string cmd;
};
