#pragma once

#include <glad/gl.h>
#include <EGL/egl.h>
#include <memory>

class TargetManager;

class RenderTarget {
protected:
    friend class TargetManager;
    int width, height;
    RenderTarget(int width, int height) : width{width}, height{height} {}
public:
    virtual void makeCurrent() = 0;
    virtual void releaseCurrent() = 0;
    virtual void swapBuffers() = 0;
    virtual GLuint getRenderTexture() const = 0;

    virtual ~RenderTarget() = default;
};




class EglTarget : public RenderTarget{
private:
    friend class TargetManager;
    EglTarget(int width, int height, EGLDisplay display, EGLConfig config);

    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;

    GLuint renderTexture;
    GLuint frameBuffer;
public:
    void makeCurrent() override;
    void releaseCurrent() override;
    void swapBuffers() override;
    GLuint getRenderTexture() const override;

    ~EglTarget();
};