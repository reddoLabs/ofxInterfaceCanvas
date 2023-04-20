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
		fbo.allocate(dstWidth, dstHeight, GL_RGBA32F_ARB);
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
		enableSeperateBlending();


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

		disableSeperateBlending();
		fbo.end();
	}

	void Canvas::draw()
	{
		if (!isSetupReady) ofLogError("Canvas::draw()", "Please setup canvas before drawing");
	}

	void Canvas::setSize(float w, float h)
	{
		Node::setSize(w,h);
		for (auto& layer : getLayers()) {
			layer->setSize(w,h);
		}
	}

	void Canvas::setSize(const ofVec2f& s)
	{
		setSize(s.x, s.y);
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

	void Canvas::enableSeperateBlending()
	{
		// blending seperate alpha
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	}

	void Canvas::disableSeperateBlending()
	{
		glDisable(GL_BLEND);
		glPopAttrib();
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
				s.numSamples = GL_RGBA32F_ARB;
				borderFbo.allocate(s);
			}
			borderFbo.begin();
			Canvas::enableSeperateBlending();
			ofPushMatrix();
			ofClear(0, 0);
		}
	}

	void LayerContainer::postDraw()
	{
		if (maskBorderElements) {
			ofPopMatrix();
			Canvas::disableSeperateBlending();
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

	void CanvasRefNode::setup(CanvasRefNodeSettings settings)
	{
		Node::setup(settings);
		canvas = settings.original;

		ofFboSettings s;
		horzAlignment = settings.horzAlignment;
		vertAlignment = settings.vertAlignment;

		scaleMode = settings.scaleMode;

		checkFbo();
	}

	void CanvasRefNode::setup(int width, int height, Canvas * original, ofAlignHorz alignment_, ofScaleMode scaleMode_)
	{
		CanvasRefNodeSettings settings;
		settings.size = ofVec2f(width, height);
		settings.original = original;
		
		settings.horzAlignment = alignment_;
		settings.scaleMode = scaleMode_;

		setup(settings);
	}

	void CanvasRefNode::setCanvas(Canvas* original)
	{
		canvas = original;
		checkFbo();
	}

	void CanvasRefNode::draw()
	{
		checkFbo();

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

			// move to align correctly
			if (vertAlignment == OF_ALIGN_VERT_BOTTOM) {
				dDraw.y = fbo.getHeight() - canvas->getHeight();
			}else if (vertAlignment == OF_ALIGN_VERT_CENTER) {
				dDraw.y = 0.5*(fbo.getHeight() - canvas->getHeight());
			}
			if (horzAlignment == OF_ALIGN_HORZ_RIGHT) {
				dDraw.x = fbo.getWidth() - canvas->getWidth();
			}
			else if (horzAlignment == OF_ALIGN_VERT_CENTER) {
				dDraw.x = 0.5 * (fbo.getWidth() - canvas->getWidth());

			}
			ofTranslate(dDraw);
			ofClear(0, 0);
			canvas->render();
			ofPopMatrix();

			//  blending seperate alpha
			glDisable(GL_BLEND);
			glPopAttrib();

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

	void CanvasRefNode::checkFbo()
	{
		bool allocate = false;
		ofRectangle scaleDim;
		if (!fbo.isAllocated()) {
			allocate = true;
		}

		if (scaleMode == OF_SCALEMODE_FIT &&
			(fbo.getWidth() != canvas->getWidth() || fbo.getHeight() != canvas->getHeight())) {
			scaleDim = ofRectangle(0, 0, canvas->getWidth(), canvas->getHeight());
			allocate = true;
		}
		else if (scaleMode == OF_SCALEMODE_FILL) {
			ofRectangle src(0, 0, canvas->getWidth(), canvas->getHeight());
			scaleDim = ofRectangle(0, 0, getWidth(), getHeight());
			scaleDim.scaleTo(src, OF_SCALEMODE_FIT);

			if (fbo.getWidth() != scaleDim.width || fbo.getHeight() != scaleDim.height) {
				allocate = true;
			}
			
		}

		if (allocate) {
			fbo.allocate(scaleDim.width, scaleDim.height, GL_RGBA32F_ARB);
			dDraw = ofVec2f(scaleDim.x, -scaleDim.y);
		}
	}


}