
#include <string>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include <sstream>

#include <GL/glew.h>

#include "logutil.h"
#include "GLSLProgram.h"
#include "GLSLShader.h"

using namespace std;

// Public part :

GLSLProgram::GLSLProgram()
    : vertex_ ( NULL ), fragment_ ( NULL ), geometry_ ( NULL )
{
  handle_ = glCreateProgram();
}

GLSLProgram::GLSLProgram
(
  const std::string &shader,
  unsigned int shaderType ,
  const GLuint &geomIn ,
  const GLuint &geomOut
)
    :  vertex_ ( NULL ), fragment_ ( NULL ), geometry_ ( NULL ), _geomIn ( geomIn ), _geomOut ( geomOut )
{
  handle_ = glCreateProgram();

  switch ( shaderType )
  {

    case GL_VERTEX_SHADER_ARB:
      vertex_ = new GLSLShader ( shader, shaderType );
      attach ( vertex_ );
      break;

    case GL_GEOMETRY_SHADER_EXT:
      geometry_ = new GLSLShader ( shader, shaderType );
      attach ( geometry_ );

      if ( ( _geomIn == 0 ) || ( _geomOut == 0 ) )
      {
        _ERROR ( "invalid geomin/geomout" );
        throw "invalid geomin/geomout at ";
      }

      break;

    case GL_FRAGMENT_SHADER_ARB:
      fragment_ = new GLSLShader ( shader, shaderType );
      attach ( fragment_ );
      break;

  }


  link();
}

GLSLProgram::GLSLProgram ( const std::string &vertexShader, const std::string &fragmentShader )
    : vertex_ ( NULL ), fragment_ ( NULL ), geometry_ ( NULL )
{
  handle_ = glCreateProgram();

  vertex_ = new GLSLShader ( vertexShader, GL_VERTEX_SHADER_ARB );
  fragment_ = new GLSLShader ( fragmentShader, GL_FRAGMENT_SHADER_ARB );

  attach ( vertex_ );
  attach ( fragment_ );

  link();
}

GLSLProgram::GLSLProgram
( const std::string &vertexShader,
  const std::string &geometryShader,
  const std::string &fragmentShader,
  const GLuint &geomIn,
  const GLuint &geomOut
)
    : vertex_ ( NULL ), fragment_ ( NULL ), geometry_ ( NULL ), _geomIn ( geomIn ), _geomOut ( geomOut )
{
  handle_ = glCreateProgram();

  if ( vertexShader.size() != 0 )
  {
    vertex_ = new GLSLShader ( vertexShader, GL_VERTEX_SHADER_ARB );
    attach ( vertex_ );
  }

  if ( geometryShader.size() != 0 )
  {
    geometry_ = new GLSLShader ( geometryShader, GL_GEOMETRY_SHADER_EXT );
    attach ( geometry_ );
  }

  if ( fragmentShader.size() != 0 )
  {
    fragment_ = new GLSLShader ( fragmentShader, GL_FRAGMENT_SHADER_ARB );
    attach ( fragment_ );
  }

  link();
}

GLSLProgram::~GLSLProgram()
{
  if ( vertex_ )
    delete vertex_;

  if ( fragment_ )
    delete fragment_;

  if ( geometry_ )
    delete geometry_;

  glDeleteProgram ( handle_ );
}

GLuint GLSLProgram::getHandle() const
{
  return handle_;
}

void GLSLProgram::GetProgramLog ( std::string &log ) const
{
  GLchar *debug;
  GLint debugLength;
  getParameter ( GL_INFO_LOG_LENGTH, &debugLength );

  debug = new GLcharARB[debugLength];
  glGetProgramInfoLog ( handle_, debugLength, &debugLength, debug );

  cout << debug;
  log.append ( debug, 0, debugLength );
  delete [] debug;
}

void GLSLProgram::validate() const
{
  glValidateProgram ( handle_ );
}

bool GLSLProgram::IsValidProgram() const
{
  GLint status;
  getParameter ( GL_VALIDATE_STATUS, &status );

  if ( !status )
    return false;
  else
    return true;
}

void GLSLProgram::getParameter ( GLenum param, GLint *data ) const
{
  glGetProgramiv ( handle_, param, data );
}

void GLSLProgram::attach ( GLSLShader &shader )
{
  glAttachShader ( handle_, shader.getHandle() );
}

void GLSLProgram::attach ( GLSLShader *shader )
{
  attach ( *shader );
}

void GLSLProgram::detach ( GLSLShader &shader )
{
  glDetachShader ( handle_, shader.getHandle() );
}

void GLSLProgram::detach ( GLSLShader *shader )
{
  detach ( *shader );
}

void GLSLProgram::link()
{

  glProgramParameteriEXT ( handle_, GL_GEOMETRY_INPUT_TYPE_EXT, _geomIn );
  glProgramParameteriEXT ( handle_, GL_GEOMETRY_OUTPUT_TYPE_EXT, _geomOut );

  int temp;
  glGetIntegerv ( GL_MAX_GEOMETRY_OUTPUT_VERTICES_EXT, &temp );
  glProgramParameteriEXT ( handle_, GL_GEOMETRY_VERTICES_OUT_EXT, temp );

  glLinkProgram ( handle_ );
  uniforms_.clear();
}

void GLSLProgram::use() const
{
  glUseProgram ( handle_ );
}

void GLSLProgram::disable() const
{
  glUseProgram ( 0 );
}

void GLSLProgram::sendUniform ( const std::string &name, float x )
{
  glUniform1f ( getLoc ( name ), x );
}

void GLSLProgram::sendUniform ( const std::string &name, float x, float y )
{
  glUniform2f ( getLoc ( name ), x, y );
}

void GLSLProgram::sendUniform ( const std::string &name, float x, float y, float z )
{
  glUniform3f ( getLoc ( name ), x, y, z );
}

void GLSLProgram::sendUniform ( const std::string &name, float x, float y, float z,
                                float w )
{
  glUniform4f ( getLoc ( name ), x, y, z, w );
}

void GLSLProgram::sendUniform ( const std::string &name, int x )
{
  glUniform1i ( getLoc ( name ), x );
}

void GLSLProgram::sendUniform ( const std::string &name, int x, int y )
{
  glUniform2i ( getLoc ( name ), x, y );
}

void GLSLProgram::sendUniform ( const std::string &name, int x, int y, int z )
{
  glUniform3i ( getLoc ( name ), x, y, z );
}

void GLSLProgram::sendUniform ( const std::string &name, int x, int y, int z,
                                int w )
{
  glUniform4i ( getLoc ( name ), x, y, z, w );
}

void GLSLProgram::sendUniform ( const std::string &name, float *matrix, bool transpose,
                                int size )
{
  int loc = getLoc ( name );

  switch ( size )
  {

    case '2':
      glUniformMatrix2fv ( loc, 0, transpose, matrix );
      break;

    case '3':
      glUniformMatrix3fv ( loc, 0, transpose, matrix );
      break;

    case '4':
      glUniformMatrix4fv ( loc, 0, transpose, matrix );
      break;
  }
}

// sendUniform functions with supplied location
void GLSLProgram::sendUniform ( GLuint location, float x )
{
  glUniform1f ( location, x );
}

void GLSLProgram::sendUniform ( GLuint location, float x, float y )
{
  glUniform2f ( location, x, y );
}

void GLSLProgram::sendUniform ( GLuint location, float x, float y, float z )
{
  glUniform3f ( location, x, y, z );
}

void GLSLProgram::sendUniform ( GLuint location, float x, float y, float z,
                                float w )
{
  glUniform4f ( location, x, y, z, w );
}

void GLSLProgram::sendUniform ( GLuint location, int x )
{
  glUniform1i ( location, x );
}

void GLSLProgram::sendUniform ( GLuint location, int x, int y )
{
  glUniform2i ( location, x, y );
}

void GLSLProgram::sendUniform ( GLuint location, int x, int y, int z )
{
  glUniform3i ( location, x, y, z );
}

void GLSLProgram::sendUniform ( GLuint location, int x, int y, int z,
                                int w )
{
  glUniform4i ( location, x, y, z, w );
}

void GLSLProgram::sendUniform ( GLuint location, float *matrix, bool transpose,
                                int size )
{
  switch ( size )
  {

    case '2':
      glUniformMatrix2fv ( location, 0, transpose, matrix );
      break;

    case '3':
      glUniformMatrix3fv ( location, 0, transpose, matrix );
      break;

    case '4':
      glUniformMatrix4fv ( location, 0, transpose, matrix );
      break;
  }
}

void GLSLProgram::fetchUniform ( const std::string &name, int &value )
{
  glGetUniformiv ( handle_, getLoc ( name ), &value );
}

GLuint GLSLProgram::getUniformLocation ( const std::string &name ) const
{
  return glGetUniformLocation ( handle_, name.c_str() );
}

void GLSLProgram::setAttributeLocation ( const std::string &name, GLuint location )
{
  glBindAttribLocation ( handle_, location, name.c_str() );
}

GLuint GLSLProgram::getAttributeLocation ( const std::string &name ) const
{
  return glGetAttribLocation ( handle_, name.c_str() );
}

void GLSLProgram::getAttributeInfo ( GLuint location, std::string &name, GLenum &datatype, GLint &size ) const
{
  // first we need to query the length of the longest name
  GLsizei length;
  getParameter ( GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, ( GLint* ) &length );

  std::vector<char> nametmp ( length );
  GLsizei copiedsize;
  glGetActiveAttrib ( handle_, location, length, &copiedsize, &size, &datatype, &nametmp[0] );

  if ( 0 == copiedsize )
    return;

  name.assign ( &nametmp[0] );
}

void GLSLProgram::getUniformInfo ( GLuint location, std::string &name, GLenum &datatype, GLint &size ) const
{
  // first we need to query the length of the longest name
  GLint length;
  getParameter ( GL_ACTIVE_UNIFORM_MAX_LENGTH, &length );

  std::vector<char> nametmp ( length );
  GLsizei copiedsize;
  glGetActiveUniform ( handle_, location, length, &copiedsize, &size, &datatype, &nametmp[0] );

  if ( 0 == copiedsize )
    return;

  name.assign ( &nametmp[0] );
}

void GLSLProgram::getAttachedShaders ( std::vector<GLuint> &shaderhandles )
{
  GLint number;
  getParameter ( GL_ATTACHED_SHADERS, &number );

  shaderhandles.resize ( number );
  glGetAttachedShaders ( handle_, number, NULL, &shaderhandles[0] );
}

namespace
{

  struct shaderconstuctor
  {
    std::vector<GLSLShader> &shaders_;
    shaderconstuctor ( std::vector<GLSLShader> &shaders ) : shaders_ ( shaders )
    {
    }

    void operator() ( std::vector<GLuint>::value_type data )
    {
      shaders_.push_back ( GLSLShader ( data ) );
    }
  };
}

void GLSLProgram::getAttachedShaders ( std::vector<GLSLShader> &shaders )
{
  std::vector<GLuint> shaderhandles;
  getAttachedShaders ( shaderhandles );

  std::for_each ( shaderhandles.begin(), shaderhandles.end(), shaderconstuctor ( shaders ) );
}

// Private part :

int GLSLProgram::getLoc ( const std::string &name )
{
  int loc = -1;
  uniformmap::const_iterator it = uniforms_.find ( name );

  if ( it == uniforms_.end() )
  {
    loc = glGetUniformLocation ( handle_, name.c_str() );

    if ( loc == -1 )
    {
      std::string s;
      s.append ( name );
      s.append ( " - is not a valid uniform variable name" );
      throw logic_error ( s );
    }

    uniforms_.insert ( uniformmap::value_type ( name, loc ) );
  }
  else
    loc = it->second;

  return loc;
}

void GLSLProgram::clear()
{
  if ( vertex_ )
    delete vertex_;

  vertex_ = NULL;

  if ( fragment_ )
    delete fragment_;

  fragment_ = NULL;

  if ( geometry_ )
    delete geometry_;

  geometry_ = NULL;

  _geomIn = 0;

  _geomOut = 0;

  glDeleteProgram ( handle_ );

  uniforms_.clear();


}

void GLSLProgram::createFromSourceStrings
( GLSLProgram &prog,
  const std::string &vertexShader,
  const std::string &geometryShader,
  const std::string &fragmentShader,
  const GLuint &geomIn,
  const GLuint &geomOut

)
{
  prog.clear();

  prog.handle_ = glCreateProgram();

  prog._geomIn  = geomIn;
  prog._geomOut = geomOut;

  if ( vertexShader.size() != 0 )
  {
    prog.vertex_ = new GLSLShader (  GL_VERTEX_SHADER_ARB );  
    prog.vertex_->setShaderSource ( vertexShader );
    prog.vertex_->compile();
    prog.attach ( prog.vertex_ );
  }

  if ( geometryShader.size() != 0 )
  {
    prog.geometry_ = new GLSLShader (  GL_GEOMETRY_SHADER_EXT );  
    prog.geometry_->setShaderSource ( geometryShader );
    prog.geometry_->compile();
    prog.attach ( prog.geometry_ );
  }

  if ( fragmentShader.size() != 0 )
  {
    prog.fragment_ = new GLSLShader (  GL_FRAGMENT_SHADER_ARB );  
    prog.fragment_->setShaderSource ( fragmentShader );
    prog.fragment_->compile();
    prog.attach ( prog.fragment_ );
  }

  prog.link();

}

GLSLProgram * GLSLProgram::createFromSourceStrings
( const std::string &vertexShader,
  const std::string &geometryShader,
  const std::string &fragmentShader,
  const GLuint &geomIn,
  const GLuint &geomOut
)
{
  GLSLProgram *ret = new GLSLProgram();

  GLSLProgram::createFromSourceStrings ( *ret, vertexShader, geometryShader, fragmentShader, geomIn, geomOut );

  return ret;
}

void GLSLProgram::createFromSourceStrings
(
  GLSLProgram & prog,
  const std::string &shader,
  unsigned int shaderType,
  const GLuint & geomIn ,
  const GLuint & geomOut
)
{
  prog.clear();

  prog.handle_ = glCreateProgram();

  prog._geomIn  = geomIn;
  prog._geomOut = geomOut;

  switch ( shaderType )
  {

    case GL_VERTEX_SHADER_ARB:
      prog.vertex_ = new GLSLShader ( shader, shaderType );
      prog.attach ( prog.vertex_ );
      break;

    case GL_GEOMETRY_SHADER_EXT:
      prog.geometry_ = new GLSLShader ( shader, shaderType );
      prog.attach ( prog.geometry_ );

      if ( ( prog._geomIn == 0 ) || ( prog._geomOut == 0 ) )
      {
        _ERROR ( "invalid geomin/geomout" );
        throw "invalid geomin/geomout at ";
      }

      break;

    case GL_FRAGMENT_SHADER_ARB:
      prog.fragment_ = new GLSLShader ( shader, shaderType );
      prog.attach ( prog.fragment_ );
      break;

  }


  prog.link();

}

GLSLProgram * GLSLProgram::createFromSourceStrings
( const std::string &shader,
  unsigned int shaderType ,
  const GLuint & geomIn ,
  const GLuint & geomOut
)
{
  GLSLProgram * prog = new GLSLProgram();

  GLSLProgram::createFromSourceStrings ( *prog, shader, shaderType, geomIn, geomOut );

  return prog;
}
