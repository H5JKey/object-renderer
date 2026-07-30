#include <gtest/gtest.h>

#include "render-engine.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"
#include "utils.hpp"

class RendererPipelineTest : public ::testing::Test {
   protected:
    SceneLoader loader;
    static std::unique_ptr<RenderEngine> engine;

    static void SetUpTestSuite() {
        ASSERT_NO_THROW(TargetManager::init());
        engine = std::make_unique<RenderEngine>();
    }
    static void TearDownTestSuite() { engine->destroy(); }
};
std::unique_ptr<RenderEngine> RendererPipelineTest::engine;

TEST_F(RendererPipelineTest, RendererWorks) {
    Scene scene;
    std::shared_ptr<RenderTarget> egl;
    EXPECT_NO_THROW(egl = TargetManager::getInstance().createEGLTarget(1, 1));
    ASSERT_NE(egl, nullptr);

    EXPECT_NO_THROW(scene = loader.loadGltf("tests/data/test-scene.glb"));
    EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 2));
}

TEST_F(RendererPipelineTest, RendersMultipleTimes) {
    Scene scene;
    std::shared_ptr<RenderTarget> egl;
    EXPECT_NO_THROW(scene = loader.loadGltf("tests/data/test-scene.glb"));

    for (int i = 1; i <= 3; i++) {
        EXPECT_NO_THROW(egl = TargetManager::getInstance().createEGLTarget((i+1) / 2));
        ASSERT_NE(egl, nullptr);
        EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 1));
    }
}

TEST_F(RendererPipelineTest, RendersEmptyScene) {
    Scene scene;
    std::shared_ptr<RenderTarget> egl;
    EXPECT_NO_THROW(egl = TargetManager::getInstance().createEGLTarget(30, 30));
    ASSERT_NE(egl, nullptr);

    EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 3));
}