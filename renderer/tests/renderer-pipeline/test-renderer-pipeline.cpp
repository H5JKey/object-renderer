#include <gtest/gtest.h>

#include "render-engine.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"

class RendererPipelineTest : public ::testing::Test {
   protected:
    static SceneLoader loader;
    static std::unique_ptr<RenderEngine> engine;
    static std::shared_ptr<RenderTarget> egl;
    static Scene scene;

    static void SetUpTestSuite() {
        ASSERT_NO_THROW(TargetManager::init());
        engine = std::make_unique<RenderEngine>();
        EXPECT_NO_THROW(egl = TargetManager::getInstance().createEGLTarget(10, 10));
        ASSERT_NE(egl, nullptr);
        ASSERT_NO_THROW(scene = loader.loadGltfFromFile("tests/data/test-scene.glb"));
    }
    static void TearDownTestSuite() {
        egl.reset();
        engine->destroy();
    }
};

std::unique_ptr<RenderEngine> RendererPipelineTest::engine;
Scene RendererPipelineTest::scene;
std::shared_ptr<RenderTarget> RendererPipelineTest::egl;
SceneLoader RendererPipelineTest::loader;

TEST_F(RendererPipelineTest, RendererWorks) { EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 2)); }

TEST_F(RendererPipelineTest, RendersMultipleTimes) {
    std::shared_ptr<RenderTarget> egl2;
    EXPECT_NO_THROW(egl2 = TargetManager::getInstance().createEGLTarget(10, 10));
    ASSERT_NE(egl2, nullptr);
    for (int i = 1; i <= 4; i++) {
        EXPECT_NO_THROW(engine->renderFrame(*egl2, scene, 1));
    }
}

TEST_F(RendererPipelineTest, RendersEmptyScene) {
    Scene scene;
    EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 5));
}