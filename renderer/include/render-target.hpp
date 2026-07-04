#pragma once

#include <glad/gl.h>
#include <EGL/egl.h>
#include <memory>

class TargetManager;

class RenderTarget {
protected:
    friend class TargetManager;
    int width, height;
    bool initialized = false;
    RenderTarget(int width, int height) : width{width}, height{height} {}
    virtual void makeCurrent() = 0;
    virtual void release() = 0;
public:
    struct ContextGuard {
        ContextGuard(RenderTarget& target) : target(target) {
            target.makeCurrent();
        }

        ~ContextGuard() {
            //target.release();
        }
    private:
        RenderTarget& target;

    };
    int getWidth() const noexcept {return width;}
    int getHeight() const noexcept {return height;}
    virtual void swapBuffers() = 0;
    virtual GLuint getRenderTexture() const = 0;

    virtual ~RenderTarget() = default;
};



class EglTarget : public RenderTarget{
private:
    friend class TargetManager;
    EglTarget(int width, int height, EGLDisplay display, EGLConfig config, EGLContext context);

    void makeCurrent() override;
    void release() override;
    void initFBO();
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    GLuint frameBuffer;
    GLuint renderTexture;

public:
    void swapBuffers() override;
    GLuint getRenderTexture() const override;

    ~EglTarget();
};