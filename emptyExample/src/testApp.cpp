#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
	edgeImage.allocate(ofGetWidth(),ofGetHeight(),OF_IMAGE_COLOR);	// white empty image
	srcImage.allocate(ofGetWidth(),ofGetHeight(),OF_IMAGE_GRAYSCALE);
	canvas.allocate(ofGetWidth(),ofGetHeight(),OF_IMAGE_GRAYSCALE);
	int i = 0;
	while(i<canvas.getPixelsRef().size()){	// make the image white
		canvas.getPixelsRef()[i] = 255;
		i++;
	}
	bStop = false;
	showEGmap = false;
	nowSaveImage = false;
	cmd = "run";
	snapCounter = 0;
	ofBackground(255,255,255);
	ofSetBackgroundAuto(false);
	m_par.Setup(&edgeImage,&srcImage);
	//edgeImage = m_par.GetEdgeImage();	
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

	if(cmd == "run")
	{		
		//do
		//	m_par.Simulate(&canvas);
		//while(!m_par.canstop());
		for(int i =0; i<5000; i++)
			m_par.Simulate(&canvas);
		canvas.reloadTexture();
		ofSetColor(255);
		canvas.draw(0,0);
		if(nowSaveImage)
		{
			nowSaveImage = false;
			SaveImage();
		}
	}
	else if(cmd == "edge")
	{
		edgeImage.reloadTexture();
		ofSetColor(150);
		edgeImage.draw(0,0);
		showEGmap = true;
	}


}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch(key){
	case ' ':
		if(cmd == "run")
			cmd = "stop";
		else if(cmd == "stop")
			cmd = "run";
		break;
	case 'e':	// show edgemap
		if(cmd == "default" || cmd == "run")
			cmd = "edge";
		break;
	case 'r':	// run again
		cmd = "run";
		break;
	case 's':	// save image	
		nowSaveImage = true;
		break;

	default:
		break;
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	cout<<"Mouse: ( "<<mouseX;
	cout<<" , "<<mouseY<<" )"<<endl;
	cout<<endl;

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}


void testApp::SaveImage()
{
	ofImage saveimage;
	saveimage.clone(canvas);
	saveimage.saveImage("snapshot"+ofToString(snapCounter)+".png");
	snapCounter++;
}