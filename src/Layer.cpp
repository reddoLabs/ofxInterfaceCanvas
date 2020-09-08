#include "Layer.h"

namespace ofxInterface {

	Layer::Layer()
	{
	}


	Layer::~Layer()
	{
	}

	void Layer::setup(LayerSettings settings)
	{
		initShader();
		setName(settings.name);
		setSize(settings.width, settings.height);
		opacity = settings.opacity;
		mask = settings.mask;
		maskPath = settings.maskPath;
		blendMode = settings.blendmode;
	}

	void Layer::preDraw()
	{
		
		if (!renderedLayer.isAllocated()) renderedLayer.allocate(getWidth(), getHeight());

		if (mask.isAllocated()) {
			if (!maskFbo.isAllocated()) maskFbo.allocate(getWidth(), getHeight());
			maskFbo.begin();
			ofClear(0, 0);
			
		}
		else if (opacity != 1.0){
			renderedLayer.begin();
			ofClear(0, 0);
		}
		ofEnableBlendMode(blendMode);
	}


	void Layer::postDraw()
	{
		if (mask.isAllocated()) {
			maskFbo.end();

			renderedLayer.begin();

			maskShader.begin();
			maskShader.setUniformTexture("maskTex", mask, 1);
			maskFbo.draw(0, 0);
			maskShader.end();

			ofDisableAlphaBlending();
			
			
		}
		if (mask.isAllocated() || opacity != 1.0) {
			renderedLayer.end();

			// opengl3 flips textures vertically, so mirror if opengl3
			if (ofIsGLProgrammableRenderer()) {
				ofPushMatrix();
				ofTranslate(0, renderedLayer.getHeight());
				ofScale(1, -1, 1);
			}
			ofSetColor(255, opacity * 255);
			renderedLayer.draw(0, 0);
			if (ofIsGLProgrammableRenderer()) {
				ofPopMatrix();
			}
		}
		//ofDisableBlendMode();
	}

	float Layer::getOpacity()
	{
		return opacity;
	}

	void Layer::setOpacity(float opacity_)
	{
		opacity = opacity_;
	}

	ofJson Layer::getNodeJson()
	{
		auto ret = Node::getNodeJson();
		ret["nodeType"] = "Layer";
		ret["opacity"] = opacity;
		ret["maskPath"] = maskPath;
		return ret;
	}


	void Layer::initShader()
	{
		string vertex = STRINGIFY(
		#version 150\n

		uniform mat4 modelViewProjectionMatrix;

		in vec4 position;
		in vec2 texcoord;



		void main()
		{

			// send the vertices to the fragment shader
			gl_Position = modelViewProjectionMatrix * position;
		}

		);
		string fragment = STRINGIFY(
			#version 150\n

			// this is how we receive the texture
			uniform sampler2DRect tex0;
		uniform sampler2DRect maskTex;


		out vec4 outputColor;

		void main()
		{
			vec4 color = texture(tex0, gl_FragCoord.xy);
			vec4 mask = texture(maskTex, gl_FragCoord.xy);
			outputColor = vec4(color.r, color.g, color.b, mask.r);
		}
		);
		maskShader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
		maskShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
		maskShader.linkProgram();
		//maskShader.load("shader");
	}

	void Layer::drawMaskedLayer()
	{
	}

	void Layer::drawElements()
	{
		for (auto& c : childNodes) {
			c->render();
		}
	}

	LayerSettings::LayerSettings(int width_, int height_, string name_, float opacity_, ofTexture mask_, string maskPath_)
	{
		width = width_;
		height = height_;
		mask = mask_;
		opacity = opacity_;
		maskPath = maskPath_;
		name = name_;
	}
}