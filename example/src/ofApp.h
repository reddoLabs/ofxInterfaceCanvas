#pragma once

#include "ofMain.h"
#include "ofxInterface.h"
#include "Layer.h"
#include "Canvas.h"

using namespace ofxInterface;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void onClickMask(TouchEvent& t);

		ofxInterface::Node* scene;

		static ofTexture loadFileAsTexture(string path);
		vector< ofTexture> textures;
		Layer* cat;
		Layer* hat;
		Layer* mask;
		Canvas* canvas;

		ofFbo fbo;
};
