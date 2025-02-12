#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Shader
{
    class TAAResolve;
    class PostProc;
    class LightingPass;
    class BloomBlur;
    class SSAO;
    class SSAOBlur;
    class Composite;
}
class Camera;

class PostprocQuad
{
public:
    static PostprocQuad* getInstance();
    void drawOnScreen(const Shader::PostProc &shader) const;
    void compositePass(const Shader::Composite& shader, GLuint fbo) const;
    void lightingPass(const Shader::LightingPass &shader, const Camera &camera, GLuint fbo) const;
    void SSAOPass(const Shader::SSAO &shader, const Shader::SSAOBlur &blur_shader, GLuint ssao_fbo, GLuint ssao_blur_fbo) const;
    void TAAResolvePass(const Shader::TAAResolve& shader, GLuint fbo) const;
private:
    void runBlur(const Shader::BloomBlur &shader, GLuint &blur_image_unit) const;
    static PostprocQuad* instance;
    PostprocQuad();
    GLuint quadVAO, quad_VBO;
};