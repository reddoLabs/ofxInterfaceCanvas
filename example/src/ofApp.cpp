#include "ofApp.h"
using namespace ofxInterface;
//--------------------------------------------------------------
void ofApp::setup(){
	ofLogToConsole();

	scene = new Node();
	scene->setSize(ofGetWidth(), ofGetHeight());
	scene->setName("Scene");

	TouchManager::one().setup(scene);

	// load textures
	textures.push_back(loadFileAsTexture("cat.png"));
	textures.push_back(loadFileAsTexture("hat.png"));
	textures.push_back(loadFileAsTexture("hole.jpg"));
	textures.push_back(loadFileAsTexture("mask.png"));

	// create a canvas
	canvas = new Canvas();
	scene->addChild(canvas);
	canvas->setup(780, 780);

	// create layer settings
	LayerSettings settings = LayerSettings(780,780,"cat");


	// the first layer, loading the cat picture
	TextureNode* tCat = new TextureNode();
	tCat->setTexture(textures[0]);
	tCat->setSize(textures[0].getWidth(), textures[0].getHeight());

	// create a layer with the layer settings
	cat = new Layer();
	cat->setup(settings);
	cat->addChild(tCat);

	// add the layer to the canvas
	canvas->addLayer(cat);


	// now create the mask layer
	settings.name = "mask";
	settings.mask = textures[3];

	// create a new texture node that will be masked
	TextureNode* tMask = new TextureNode();
	tMask->setTexture(textures[2]);
	tMask->setSize(textures[2].getWidth(), textures[2].getHeight());

	mask = new Layer();
	mask->setup(settings);
	mask->addChild(tMask);
	mask->setName("mask");
	canvas->addLayer(mask);

	// the third layer will have a dynamic opacity
	LayerSettings settings2 = LayerSettings(780, 780, "hat");
	settings.name = "hat";
	settings.mask = ofTexture();

	// create a new texture node that will be masked
	TextureNode* tHat = new TextureNode();
	tHat->setTexture(textures[1]);
	tHat->setSize(textures[1].getWidth(), textures[1].getHeight());

	hat = new Layer();
	hat->setup(settings2);
	hat->addChild(tHat);
	hat->setName("hat");
	canvas->addLayer(hat);
}

//--------------------------------------------------------------
void ofApp::update(){
	TouchManager::one().update();

	// control hat opacity with mouse position
	hat->setOpacity(float(ofGetMouseX()) / float(ofGetWidth()));
}

//--------------------------------------------------------------
void ofApp::draw(){
	scene->render();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
	TouchManager::one().touchMove(button, ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	TouchManager::one().touchDown(button, ofVec2f(x, y));

	// change mask visibility with mouse click
	mask->setVisible(!mask->getVisible());
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	TouchManager::one().touchUp(button, ofVec2f(x, y));
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

ofTexture ofApp::loadFileAsTexture(string path)
{
	ofTexture ret;
	ofStringReplace(path, "\\", "/");
	ofImage bi;
	bi.load(path);
	ret.loadData(bi.getPixels());
	return ret;
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
