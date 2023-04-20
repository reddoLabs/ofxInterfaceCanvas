#pragma once
#include "Node.h"

#define STRINGIFY(...) STRINGIFY_AUX(__VA_ARGS__)
#define STRINGIFY_AUX(...) #__VA_ARGS__


namespace ofxInterface {

	class LayerSettings {
	public:
		LayerSettings(int width, int height, string name = "", float opacity = 1.0, ofTexture mask = ofTexture(), string maskPath = "");
		~LayerSettings() {};

		int width;
		int height;
		ofTexture mask;
		float opacity;
		string maskPath;
		string name;
		ofBlendMode blendmode = OF_BLENDMODE_ALPHA;
	};

	/// \brief Photoshop-like layer that allows to render nodes with masking options and blend-modes
	class Layer : public Node
	{
	public:
		Layer();
		~Layer();

		void setup(LayerSettings settings);

		void preDraw() override;
		void postDraw() override;

		float getOpacity();
		void setOpacity(float opacity);

		virtual ofJson getNodeJson() override;

	protected:
		void drawMaskedLayer();
		void drawElements();

		void initShader();

	private:
		float opacity = 1.0;
		ofFbo renderedLayer;
		ofFbo maskFbo;
		ofTexture mask;
		string maskPath;

		//function objects
		ofShader maskShader;
		ofBlendMode blendMode;

	};

}
