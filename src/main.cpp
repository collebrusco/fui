#include <flgl.h>
#include <flgl/tools.h>
#include <flgl/logger.h>
#include "UI.h"
LOG_MODULE(main);

int main() {
	gl.init();
	window.create("hello window", 1280, 720);
	
	while (!window.should_close()) {
		if (window.keyboard[GLFW_KEY_ESCAPE].down) window.close();
		window.update();
	}

	gl.destroy();
	
	return 0;
}
