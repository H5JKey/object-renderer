#include <print>
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"

int main(int argc, char* argv[]) {
    auto egl = TargetManager::getInstance().createEGLTarget(800,600);
    
}