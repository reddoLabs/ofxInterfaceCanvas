#include "Canvas.h"

namespace ofxInterface {

	Canvas::Canvas()
	{
		bRenderChildrenInGroup = true;
	}


	Canvas::~Canvas()
	{
	}

	void Canvas::setup(int width, int height, bool maskBorderElements)
	{
		setSize(width, height);
		container = new LayerContainer();
		container->setMaskBorderElements(maskBorderElements);
		container->setSize(width, height);
		addChild(container);

		ofAddListener(Node::eventNodeSizeChanged, this, &Canvas::onCanvasSizeChanged);

		isSetupReady = true;
	}

	ofFbo Canvas::renderInFbo(int dstWidth, int dstHeight, ofScaleMode mode)
	{
		ofFbo fbo;
		fbo.allocate(dstWidth, dstHeight);
		renderInFboRef(fbo, mode);

		return fbo;
	}

	ofFbo Canvas::renderInFbo()
	{
		return renderInFbo(getWidth(), getHeight());
	}

	void Canvas::renderInFboRef(ofFbo & fbo, ofScaleMode mode)
	{
		ofRectangle src = ofRectangle(0, 0, getWidth(), getHeight());
		ofRectangle dst = ofRectangle(0, 0, fbo.getWidth(), fbo.getHeight());
		src.scaleTo(dst, mode);

		ofRectangle scale = ofRectangle(src.x, src.y, float(src.getWidth()) / float(getWidth()), float(src.getHeight()) / float(getHeight()));

		fbo.begin();

		// opengl3 flips textures vertically, so mirror if opengl3
		if (ofIsGLProgrammableRenderer()) {
			ofPushMatrix();
			ofTranslate(0, fbo.getHeight());
			ofScale(1, -1, 1);
		}

		ofClear(0, 0);

		//move
		ofPushMatrix();
		ofTranslate(scale.x, scale.y);
		//scale
		ofPushMatrix();
		ofScale(scale.width, scale.height);

		renderDynamic();
		ofPopMatrix();
		ofPopMatrix();
		
		if (ofIsGLProgrammableRenderer()) {
			ofPopMatrix();
		}
		fbo.end();
	}

	void Canvas::draw()
	{
		if (!isSetupReady) ofLogError("Canvas::draw()", "Please setup canvas before drawing");
	}

	void Canvas::addLayer(Layer * layer, int insertAt)
	{
		if (insertAt < 0 || insertAt >= layers.size()) {
			layers.push_back(layer);
			container->addChild(layer);
		}
		else {
			layers.insert(layers.begin() + insertAt, layer);
			container->addChild(layer, insertAt);
		}
	}

	void Canvas::removeLayer(Layer * layer)
	{
		int toDel = -1;
		for (int i = 0; i < layers.size(); i++) {
			if (layers[i] == layer) toDel = i;
		}
		if (toDel != -1) {
			layers.erase(layers.begin() + toDel);
			container->removeChild(layer);
		}
	}

	void Canvas::removeLayer(string layerName)
	{
		Layer* l = nullptr;
		for (int i = 0; i < layers.size(); i++) {
			if (layers[i]->getName() == layerName) l = layers[i];
		}
		if (l != nullptr) {
			removeLayer(l);
		}
	}

	void Canvas::removeLayer(int index)
	{
		if (index >= 0 && index < layers.size()) {
			removeLayer(layers[index]);
		}
	}

	Layer * Canvas::getLayer(string layerName)
	{
		for (auto& l:layers)
		{
			if (l->getName() == layerName) {
				return l;
			}
		}
		return nullptr;
	}

	void Canvas::onCanvasSizeChanged(Node & node)
	{
		container->setSize(getWidth(), getHeight());
	}

	ofJson Canvas::getNodeJson()
	{
		auto ret = Node::getNodeJson();
		ret["nodeType"] = "Canvas";
		return ret;
	}

	LayerContainer::LayerContainer()
	{
	}

	LayerContainer::~LayerContainer()
	{
	}

	void LayerContainer::setup(bool cutBorderElements_)
	{
		maskBorderElements = cutBorderElements_;
	}

	void LayerContainer::preDraw()
	{
		if (maskBorderElements) {
			if (!borderFbo.isAllocated() || borderFbo.getWidth() != getWidth() || borderFbo.getHeight() != getHeight()) {
				ofFboSettings s;
				s.width = getWidth();
				s.height = getHeight();
				borderFbo.allocate(s);
			}
			borderFbo.begin();
			ofPushMatrix();
			ofClear(0, 0);
		}
	}

	void LayerContainer::postDraw()
	{
		if (maskBorderElements) {
			ofPopMatrix();
			borderFbo.end();
			borderFbo.draw(0, 0);
		}
	}

	void LayerContainer::setMaskBorderElements(bool maskBorderElements_)
	{
		maskBorderElements = maskBorderElements_;
	}

	bool LayerContainer::getMaskBorderElements()
	{
		return maskBorderElements;
	}

	CanvasRefNode::CanvasRefNode():Node()
	{
	}

	CanvasRefNode::~CanvasRefNode()
	{
	}

	void CanvasRefNode::setup(int width, int height, Canvas * original, ofAlignHorz alignment_, ofScaleMode scaleMode_)
	{
		setSize(width, height);
		canvas = original;
		
		ofFboSettings s;
		alignment = alignment_;
		scaleMode = scaleMode_;

		if (scaleMode == OF_SCALEMODE_FIT) {
			fbo.allocate(canvas->getWidth(), canvas->getHeight());
		}
		else if (scaleMode == OF_SCALEMODE_FILL) {
			ofRectangle src(0, 0, canvas->getWidth(), canvas->getHeight());
			ofRectangle dest(0, 0, getWidth(), getHeight());
			dest.scaleTo(src, OF_SCALEMODE_FIT);
			fbo.allocate(dest.width,dest.height);
			dDraw = ofVec2f(dest.x, -dest.y);
		}
	}

	void CanvasRefNode::draw()
	{
		ofPushMatrix();
		ofEnableAlphaBlending();
			fbo.begin();
			// opengl3 flips textures vertically, so mirror if opengl3
			if (ofIsGLProgrammableRenderer()) {
				ofPushMatrix();
				ofTranslate(0, fbo.getHeight());
				ofScale(1, -1, 1);
			}
			ofPushMatrix();
			ofTranslate(dDraw);
			ofClear(0, 0);
			canvas->render();
			ofSetColor(255, 0, 0);
			ofPopMatrix();
			fbo.end();
			if (ofIsGLProgrammableRenderer()) {
				ofPopMatrix();
			}
		ofRectangle src(0, 0, fbo.getWidth(), fbo.getHeight());
		ofRectangle dest(0, 0, getWidth(), getHeight());
		src.scaleTo(dest, OF_SCALEMODE_FIT);

		ofEnableAlphaBlending();

		
		fbo.draw(src);

		ofPopMatrix();
	}


}