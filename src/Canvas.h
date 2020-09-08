#pragma once

#include "Layer.h"
#include "ofxInterface.h"

namespace ofxInterface {

	/// \brief Container that holds the canvas layers
	/// \ designed to be able to mask all the layers in Node size
	class LayerContainer : public Node {
	public:
		LayerContainer();
		~LayerContainer();

		void setup(bool maskBorderElements = true);

		void preDraw() override;
		void postDraw() override;

		/// \brief enable/disable masking of elements outside of the node border
		void setMaskBorderElements(bool maskBorderElements);
		/// \brief returns mask status of border elements
		bool getMaskBorderElements();

	private:
		ofFbo borderFbo;
		bool maskBorderElements = true; // masks every pixel outside the canvas
	};

	/// \brief Canvas working with layers and blendmodes as you it from Photoshop
	class Canvas : public Node
	{
	public:
		Canvas();
		~Canvas();

		void setup(int width, int height, bool cutBorderElements = true);
		ofFbo renderInFbo(int dstWidth, int dstHeight, ofScaleMode mode = OF_SCALEMODE_FIT);
		ofFbo renderInFbo();
		void renderInFboRef(ofFbo& fbo, ofScaleMode mode = OF_SCALEMODE_FIT);

		void draw() override;

		void addLayer(Layer* layer, int insertAt = -1);
		void removeLayer(Layer* layer);
		void removeLayer(string layerName);
		void removeLayer(int index);
		const vector<Layer*>& getLayers() const { return layers; }
		Layer* getLayer(string layerName);

		void onCanvasSizeChanged(Node& node);

		virtual ofJson getNodeJson() override;

	protected:


	private:
		//attributes
		bool isSetupReady = false;
		float pixelPerInch;

		//elements
		LayerContainer* container;
		vector<Layer*> layers;

	};

	/// \brief a class to draw multiple instances of a Canvas in a convenient way
	class CanvasRefNode : public Node
	{
	public:
		CanvasRefNode();
		~CanvasRefNode();

		void setup(int width, int height, Canvas* original, ofAlignHorz alignment = OF_ALIGN_HORZ_CENTER, ofScaleMode scaleMode = OF_SCALEMODE_FIT);
		void draw();

	private:
		Canvas* canvas;
		ofFbo fbo;
		ofAlignHorz alignment = OF_ALIGN_HORZ_CENTER;
		ofScaleMode scaleMode = OF_SCALEMODE_FIT;
		ofVec2f dDraw = ofVec2f(0, 0);
	};

}