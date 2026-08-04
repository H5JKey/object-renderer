#include <gtest/gtest.h>

#include "render-engine.hpp"
#include "scene-loader.hpp"
#include "scene.hpp"
#include "target-manager.hpp"

class RendererPipelineTest : public ::testing::Test {
   protected:
    static SceneLoader loader;
    static std::unique_ptr<RenderEngine> engine;
    static Scene scene;

    static void SetUpTestSuite() {
        ASSERT_NO_THROW(TargetManager::init());
        engine = std::make_unique<RenderEngine>();
        ASSERT_NO_THROW(scene = loader.loadGltfFromFile("tests/data/test-scene.glb"));
    }
    static void TearDownTestSuite() {
        engine->destroy();
        TargetManager::terminate();
    }
};

std::unique_ptr<RenderEngine> RendererPipelineTest::engine;
Scene RendererPipelineTest::scene;
SceneLoader RendererPipelineTest::loader;

TEST_F(RendererPipelineTest, RendererWorks) {
    std::shared_ptr<RenderTarget> egl;
    EXPECT_NO_THROW(egl = TargetManager::getInstance().createEGLTarget(100, 80));
    ASSERT_NE(egl, nullptr);
    ;
    EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 3));
}

TEST_F(RendererPipelineTest, RendersMultipleTimes) {
    for (int i = 1; i <= 5; i++) {
        std::shared_ptr<RenderTarget> egl;
        EXPECT_NO_THROW(egl = TargetManager::getInstance().createEGLTarget(i * 10, i * 10));
        ASSERT_NE(egl, nullptr);
        EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 1));
    }
}

TEST_F(RendererPipelineTest, RendersEmptyScene) {
    Scene scene;
    std::shared_ptr<RenderTarget> egl;
    EXPECT_NO_THROW(egl = TargetManager::getInstance().createEGLTarget(50, 50));
    ASSERT_NE(egl, nullptr);
    EXPECT_NO_THROW(engine->renderFrame(*egl, scene, 5));
}