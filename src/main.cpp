#include <boglfw/renderOpenGL/glToolkit.h>
#include <boglfw/renderOpenGL/Renderer.h>
#include <boglfw/renderOpenGL/Viewport.h>
#include <boglfw/renderOpenGL/Camera.h>
#include <boglfw/renderOpenGL/GLText.h>
#include <boglfw/renderOpenGL/IRenderable.h>
#include <boglfw/renderOpenGL/shader.h>
#include <boglfw/input/GLFWInput.h>
#include <boglfw/input/InputEvent.h>
#include <boglfw/input/operations/OperationsStack.h>
#include <boglfw/input/operations/OperationPan.h>
#include <boglfw/math/math3D.h>
#include <boglfw/OSD/ScaleDisplay.h>
#include <boglfw/Infrastructure.h>

#include <boglfw/utils/log.h>

#include <GLFW/glfw3.h>
#define GLEW_NO_GLU
#include <GL/glew.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <stdexcept>
#include <cstdio>

class Mandelbrot : public IRenderable {
public:

	static int nIterations;

	Mandelbrot(Renderer* r) {
		LOGPREFIX("Mandelbrot ctor");
		r->registerRenderable(this);
		shaderProgram_ = Shaders::createProgram("data/shaders/mandelbrot.vert", "data/shaders/mandelbrot.frag");
		if (shaderProgram_ == 0) {
			throw std::runtime_error("Unable to load mandelbrot shaders!!");
		}
		unsigned indexPos = glGetAttribLocation(shaderProgram_, "position");
		indexIterations_ = glGetUniformLocation(shaderProgram_, "nIterations");
		indexTransform_ = glGetUniformLocation(shaderProgram_, "transform");
		indexAspectRatio_ = glGetUniformLocation(shaderProgram_, "aspectRatio");
		checkGLError("getattrLoc");

		glm::vec2 positions[] {
			{-1.f, -1.f},	// bottom left
			{-1.f, +1.f},	// top left
			{+1.f, +1.f},	// top right
			{+1.f, -1.f},	// bottom right
		};
		uint16_t indices[] {
			0, 1, 2, 0, 2, 3
		};

		glGenVertexArrays(1, &VAO_);
		glBindVertexArray(VAO_);
		glGenBuffers(1, &VBO_);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
		glEnableVertexAttribArray(indexPos);
		glVertexAttribPointer(indexPos, 2, GL_FLOAT, GL_FALSE, sizeof(positions[0]), 0);
		glGenBuffers(1, &IBO_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	/*
	 * This is called on the object at each frame, for each active viewport, for each draw batch
	 */
	void render(Viewport* pCrtViewport, unsigned batchId) override {
		glUseProgram(shaderProgram_);
		glBindVertexArray(VAO_);

		glUniform1i(indexIterations_, nIterations);
		float scale = pCrtViewport->height() * 0.5f / pCrtViewport->camera()->getOrthoZoom();
		glm::vec2 translation = vec3xy(pCrtViewport->camera()->position());
		glm::vec3 tr(translation.x, translation.y, scale);
		glUniform3fv(indexTransform_, 1, &tr.x);
		glUniform1f(indexAspectRatio_, pCrtViewport->aspect());
		glDisable(GL_CULL_FACE); // TODO do we need this?
		checkGLError("mandelbrot setup");

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
		checkGLError("mandelbrot render");

		glBindVertexArray(0);
		glUseProgram(0);
	}

	// this signals the component to treat all following draw commands as a new batch, and render them
	// in a separate pass to achieve layering
	void startBatch() override {
	}

	void setupFrameData() override {
	}

	// Called once per frame, after viewports are finished, to clear queued data
	void purgeRenderQueue() override {
	}

	// called once when the renderer is destroyed to release all resources associated with this renderable.
	void unload() override {
		glDeleteProgram(shaderProgram_);
		glDeleteVertexArrays(1, &VAO_);
		glDeleteBuffers(1, &VBO_);
		glDeleteBuffers(1, &IBO_);
	}

	const char* getName() const override {
		static char name[] = "Mandelbrot";
		return name;
	}

private:
	int shaderProgram_;
	int indexIterations_;
	int indexTransform_;
	int indexAspectRatio_;
	unsigned VAO_;
	unsigned VBO_;
	unsigned IBO_;
};

int Mandelbrot::nIterations = 24;
bool drawInfo = true;

void onInputEventHandler(InputEvent& ev) {
	if (ev.isConsumed())
		return;
	switch (ev.type) {
		case InputEvent::EV_KEY_CHAR:
		switch (ev.ch) {
			case '-': if (Mandelbrot::nIterations > 1) Mandelbrot::nIterations--; break;
			case '+':
			case '=': Mandelbrot::nIterations++; break;
			case ' ': drawInfo = !drawInfo;
			default: break;
		}
		break;
		default: break;
	}
}

ScaleDisplay scale({15, 25}, 0, 300);

void renderInfo(Viewport* vp) {
	if (!drawInfo)
		return;

	scale.draw(vp);

	std::stringstream ss;
	ss << "Iterations: " << Mandelbrot::nIterations << "; Use +/- to increase/decrease";
	GLText::get()->print(ss.str(), {15.f, 50.f}, 0, 16, glm::vec3(1.f, 1.f, 1.f));

	GLText::get()->print("Use SPACE to toggle info display ON/OFF", {15.f, 70.f}, 0, 16, glm::vec3(1.f, 1.f, 1.f));
}

int main(int argc, char* argv[]) {
	// initialize stuff:
	int winW = 1280, winH = 720;
	SSDescriptor ssdesc {SSDescriptor::SS_9X}; // 2x2 super samples
	if (!gltInitGLFWSupersampled(winW, winH, ssdesc, "GLMandlebrot")) {
		ERROR("Cannot initialize openGL!");
		return -1;
	}

	GLFWInput::initialize(gltGetWindow());
	GLFWInput::onInputEvent.add(onInputEventHandler);

	Renderer renderer(winW, winH);
	auto vp = std::make_unique<Viewport>(0, 0, winW, winH);
	auto vp1 = vp.get();
	renderer.addViewport("main", std::move(vp));
	vp1->camera()->setOrthoZoom(winH / 2.f);

	Mandelbrot m(&renderer);

	OperationsStack opStack(vp1, nullptr, nullptr);
	opStack.pushOperation(std::unique_ptr<IOperation>(new OperationPan(InputEvent::MB_LEFT)));

	std::vector<drawable> drawList;
	drawList.push_back(&renderInfo);
	vp1->setDrawList(drawList);

	while (GLFWInput::checkInput()) {
		// wait until previous frame finishes rendering and show frame output:
		gltEnd();
		gltBegin();
		renderer.startBatch();
		renderer.render();
		// now rendering is on-going, move on to the next update:
	}

	renderer.unload();
	Infrastructure::shutDown();

	return 0;
}

