#pragma once

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <iomanip>
#include <vector>
#include <array>
#include <unordered_map>

#include "character.h"


#define SRC_WIDTH util::Globals::camera.width()
#define SRC_HEIGHT util::Globals::camera.height()

class Light;
class Camera;
class Light_vertices;
class Model;
struct Texture;

#define check_glError() util::checkGLError_(__FILE__, __LINE__)

#define INIT_GLFW() \
	glfwInit(); \
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); \
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6); \
    glfwWindowHint(GLFW_SAMPLES, 4); \
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); \
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);  // Comment this line when releasing the application


namespace util
{
    struct Globals
    {
        static bool first_mouse, blend, cullFace, skybox, visualizeNormal, wireframe, visualizeTangent,
            hdr, bloom, deferred, SSAO, pbrMat;
        static double last_xpos, last_ypos;
        static int cascade_size, cubemap_size, frameIndexMod16;
        static GLuint blurBrightImageUnit, deferedFrameIndex;
        static Camera camera;
        static double deltaTime, lastTime;
        static float normal_magnitude, shadow_bias, tangent_magnitude, exposure, threshold, ssaoRadius;
        static glm::vec3 bg_color;
        static std::vector<float> cascade_levels;
    };
    GLenum checkGLError_(const char* file, int line);
    void APIENTRY debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
        const char* message, const void* userParam);
    void mouse_callback(GLFWwindow* window, double xpos, double ypos);
    void scroll_callback(GLFWwindow* window, double delta_x, double delta_y);
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void process_input(GLFWwindow* window);
    glm::mat4 face_camera_model(const glm::mat4& camera_mat, glm::vec3 light_pos);

    inline void print_row(const std::vector<std::string>& texts, int col_width)
    {
        std::cout << "|";
        for(const std::string& s: texts)
        {
            int pad_left = (s.size() + col_width) * 0.5f;
            int pad_right = col_width - pad_left;
            std::cout << std::setw(pad_left) << s << std::setw(pad_right) << " " << "|";
        }
        std::cout << std::endl;
    }
    
    inline GLuint create_vbo_2_vao(float* data, GLuint size)
    {
        GLuint VBO;
        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        return VBO;
    }

    inline void print_mat(glm::mat4 mat)
    {
        for(int i = 0; i < 4; ++i)
        {
            for(int j = 0; j < 4; ++j)
                std::cout << mat[j][i] << " ";
            std::cout << "\n";
        }
    }

    inline void print_mat(glm::mat3 mat)
    {
        for(int i = 0; i < 3; ++i)
        {
            for(int j = 0; j < 3; ++j)
                std::cout << mat[j][i] << " ";
            std::cout << "\n";
        }
    }

    inline void set_float(const char* name, float value, GLuint program_id) 
    { 
        GLint location = glGetUniformLocation(program_id, name);
        if(location != -1) glUniform1f(location, value);
    }
    inline void set_int(const char* name, int value, GLuint program_id) 
    { 
        GLint location = glGetUniformLocation(program_id, name);
        if(location != -1) glUniform1i(location, value); 
    }

    inline void set_bool(const char* name, bool value, GLuint program_id) 
    { 
        GLint location = glGetUniformLocation(program_id, name);
        if(location != -1) glUniform1i(location, value); 
    }

    inline void set_floats(const char* name, const glm::vec3& vec3, GLuint program_id)
    { 
        GLint location = glGetUniformLocation(program_id, name);
        if(location != -1) glUniform3f(location, vec3.x, vec3.y, vec3.z); 
    }
    
    inline void set_floats(const char* name, const glm::vec4& vec4, GLuint program_id)
    { 
        GLint location = glGetUniformLocation(program_id, name);
        if(location != -1) glUniform4f(location, vec4.x, vec4.y, vec4.z, vec4.w); 
    }

    inline void set_mat(const char* name, const glm::mat4& mat4, GLuint program_id) 
    { 
        GLint location = glGetUniformLocation(program_id, name);
        if(location != -1) glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat4)); 
    }

    inline void set_mat(const char* name, const glm::mat3& mat3, GLuint program_id) 
    { 
        GLint location = glGetUniformLocation(program_id, name);
        if(location != -1) glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(mat3)); 
    }

    inline void checkFrameBufferInComplete(const char* fboName)
    {
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            std::cerr << "FrameBuffer: [" << fboName << "] incomplete!Check your code!" << std::endl;
            exit(1); 
        }
    }

    void create_cascademap_framebuffer(GLuint &depth_fbo, Texture &texture, const char* tex_name);
    void create_depthcubemap_framebuffer(GLuint& depth_fbo, Texture& texture, const char* tex_name);
    void create_HDRI(const char* path, Texture& texture);
    void create_texture(const char* path, bool is_SRGB, Texture& texture);
    void create_cubemap(const std::vector<std::string>& faces_path,
        Texture& cubemap_texture, 
        Texture& diffuse_irrad_texture,
        Texture& prefilter_envmap_texture,
        Texture& BRDF_LUT);
    void createCubemap(const char* hdri_path, 
        Texture& hdri_cubemap_texture,
        Texture& diffuse_irrad_texture,
        Texture& prefilter_envmap_texture,
        Texture& BRDF_LUT);
    void create_diffuse_irrad(const Texture& cubemap_tex, Texture& diffuse_irrad_tex);
    void create_prefilter_envmap(const Texture& cubemap_tex, Texture& prefiltered_envmap);
    void create_BRDF_intergral(const Texture& cubemap_tex, Texture& BRDF_LUT);
    void ImGUIDesign(Model &model);
    std::array<glm::vec3, 64> get_ssao_samples();
    std::array<glm::vec3, 8> get_frustum_corner_world(const Camera& camera, float near, float far);
}
