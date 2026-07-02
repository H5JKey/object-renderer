#include "render-engine.hpp"

Engine::Engine() {

}

void Engine::renderFrame(RenderTarget& target, const Scene& scene) const {
    target.makeCurrent();
    target.swapBuffers();

}