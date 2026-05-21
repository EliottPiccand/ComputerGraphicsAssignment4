#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <Lib/glfw.h>

#include "Resources/Shader.h"

class Input;

class Window
{
  public:
    static constexpr const char *DEFAULT_TITLE = "Computer Graphics Assignment #4";
    static constexpr const uint16_t DEFAULT_WIDTH = 1280;
    static constexpr const uint16_t DEFAULT_HEIGHT = 720;

    Window();
    ~Window();

    [[nodiscard]] bool shouldClose() const;
    void startRendering() const;

    void setMotionBlurFactor(float factor);

    void bindFrameBuffer() const;
    void unbindFrameBuffer() const;
    void mapFrameBuffer(const std::vector<std::weak_ptr<resource::Shader>> &shaders) const;

    void endFrame();

    void setTitle(std::string title) const;
    void toggleFullScreen();
    void captureMouse();
    void releaseMouse();

    [[nodiscard]] std::pair<uint32_t, uint32_t> getFramebufferSize() const;

    void close();

  private:
    friend Input;

    GLFWwindow *handle_;

    int non_full_screen_position_x_;
    int non_full_screen_position_y_;
    int non_full_screen_width_;
    int non_full_screen_height_;
    bool is_full_screen_;

    uint32_t width_;
    uint32_t height_;

    float motion_blur_factor_;
    bool motion_blur_history_initialized_;

    GLuint frame_buffer_;
    GLuint color_texture_;
    GLuint depth_texture_;
    GLuint normals_texture_;
    GLuint color_texture_snapshot_;
    GLuint depth_texture_snapshot_;
    GLuint normals_texture_snapshot_;

    GLuint motion_blur_frame_buffer_;
    GLuint motion_blur_color_texture_;
    GLuint motion_blur_history_texture_;

    void createColorDepthNormalsTextures(uint32_t width, uint32_t height);
    void deleteColorDepthNormalsTextures();
    void createMotionBlurTextures(uint32_t width, uint32_t height);
    void deleteMotionBlurTextures();
    void createFrameBuffer(uint32_t width, uint32_t height);
    void deleteFrameBuffer();
};
