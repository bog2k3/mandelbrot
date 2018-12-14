#include <boglfw/renderOpenGL/glToolkit.h>
#include <boglfw/renderOpenGL/Renderer.h>
#include <boglfw/renderOpenGL/Viewport.h>
#include <boglfw/renderOpenGL/GLText.h>
#include <boglfw/input/GLFWInput.h>
#include <boglfw/input/InputEvent.h>
#include <boglfw/input/operations/OperationsStack.h>
#include <boglfw/input/operations/OperationPan.h>
#include <boglfw/math/math3D.h>
#include <boglfw/OSD/ScaleDisplay.h>
#include <boglfw/Infrastructure.h>

#include <boglfw/utils/log.h>

#include <GLFW/glfw3.h>

#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <stdexcept>
#include <cstdio>

void onInputEventHandler(InputEvent& ev) {
	if (ev.isConsumed())
		return;
}

int main(int argc, char* argv[]) {
	do {

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
			// build the render queue for the current frame and start the actual openGL render (which is independent of our world)
			renderer.render();
			// now rendering is on-going, move on to the next update:
		}

		renderer.unload();
		Infrastructure::shutDown();
	} while (0);

	return 0;
}

