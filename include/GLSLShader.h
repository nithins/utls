#ifndef GLSL_SHADER
#define GLSL_SHADER

#include <GL/gl.h>
#include <string>

class GLSLShader
{
public:
  GLSLShader(const std::string &filename, GLenum shaderType = GL_VERTEX_SHADER);
  GLSLShader(GLenum shaderType = GL_VERTEX_SHADER );
  ~GLSLShader();
  void compile();
  bool isCompiled() const; 
  void getShaderLog(std::string &log) const;
  void getShaderSource(std::string &shader) const;
  void setShaderSource(const std::string &code);

  GLuint getHandle() const;
  void getParameter(GLenum param, GLint *data) const;

private:
  char *readShader(const std::string &filename);
  bool compiled_;
  GLint handle_;
};

#endif //GLSL_SHADER
