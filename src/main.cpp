#include <boglfw/renderOpenGL/glToolkit.h>
#include <boglfw/renderOpenGL/Renderer.h>
#include <boglfw/renderOpenGL/Viewport.h>
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

	int nIterations = 2;

	Mandelbrot(Renderer* r) {
		r->registerRenderable(this);
		shaderProgram_ = Shaders::createProgram("data/shaders/mandelbrot.vert", "data/shaders/mandelbrot.frag");
		if (shaderProgram_ == 0) {
			throw std::runtime_error("Unable to load mandelbrot shaders!!");
		}
		indexPos_ = glGetAttribLocation(shaderProgram_, "position");
		checkGLError("getattrLoc");
		indexUV_ = glGetAttribLocation(shaderProgram_, "uv");
		checkGLError("getattrLoc");
		indexIterations_ = glGetUniformLocation(shaderProgram_, "nIterations");
		checkGLError("getattrLoc");

		positions_[0] = {-1.f, -1.f};	// bottom left
		positions_[1] = {-1.f, +1.f};	// top left
		positions_[2] = {+1.f, +1.f};	// top right
		positions_[3] = {+1.f, -1.f};	// bottom right
		UVs_[0] = {0.f, 0.f};	// bottom left
		UVs_[1] = {0.f, 1.f};	// top left
		UVs_[2] = {1.f, 1.f};	// top right
		UVs_[3] = {1.f, 0.f};	// bottom right
		indices_[0] = 0;
		indices_[1] = 1;
		indices_[2] = 2;
		indices_[3] = 0;
		indices_[4] = 2;
		indices_[5] = 3;
	}

	/*
	 * This is called on the object at each frame, for each active viewport, for each draw batch
	 */
	void render(Viewport* pCrtViewport, unsigned batchId) override {
		glUseProgram(shaderProgram_);
		checkGLError("mandelbrot render 0a");
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		checkGLError("mandelbrot render 0b");
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		checkGLError("mandelbrot render 1");
		glEnableVertexAttribArray(indexPos_);
		checkGLError("mandelbrot render 2");
		glEnableVertexAttribArray(indexUV_);
		checkGLError("mandelbrot render 3");
		glUniform1i(indexIterations_, nIterations);
		checkGLError("mandelbrot render 4");
		glDisable(GL_CULL_FACE); // TODO do we need this?
		checkGLError("mandelbrot render 5");

		glVertexAttribPointer(indexPos_, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), &positions_[0]);
		checkGLError("mandelbrot render 6");
		glVertexAttribPointer(indexUV_, 2, GL_FLOAT, GL_FALSE, 2*sizeof(float), &UVs_[0]);
		checkGLError("mandelbrot render 7");
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, &indices_[0]);
		checkGLError("mandelbrot render 8");
	}

	// this signals the component to treat all following draw commands as a new batch, and render them
	// in a separate pass to achieve layering
	void startBatch() override {
	}

	/**
	 * Called once per frame, after viewports are finished, to clear queued data
	 */
	void purgeRenderQueue() override {
	}

	// called once when the renderer is destroyed to release all resources associated with this renderable.
	void unload() override {
		glDeleteProgram(shaderProgram_);
	}

	const char* getName() const override {
		static char name[] = "Mandelbrot";
		return name;
	}

private:
	int shaderProgram_;
	int indexPos_;
	int indexUV_;
	int indexIterations_;

	glm::vec2 positions_[4];
	glm::vec2 UVs_[4];
	uint16_t indices_[6];
};

void onInputEventHandler(InputEvent& ev) {
	if (ev.isConsumed())
		return;
}

int main(int argc, char* argv[]) {
	// initialize stuff:
	int winW = 1280, winH = 720;
	if (!gltInit(winW, winH, "GLMandlebrot"))
		return -1;

	GLFWInput::initialize(gltGetWindow());
	GLFWInput::onInputEvent.add(onInputEventHandler);

	Renderer renderer(winW, winH);
	auto vp = std::make_unique<Viewport>(0, 0, winW, winH);
	auto vp1 = vp.get();
	renderer.addViewport("main", std::move(vp));

	Mandelbrot m(&renderer);

	OperationsStack opStack(vp1, nullptr, nullptr);
	opStack.pushOperation(std::unique_ptr<IOperation>(new OperationPan(InputEvent::MB_RIGHT)));

	ScaleDisplay scale({15, 25}, 0, 300);

	std::vector<drawable> drawList;
	drawList.push_back(&scale);
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

