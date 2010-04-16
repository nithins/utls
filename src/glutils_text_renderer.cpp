#include <config.h>


#if defined(GLUT_FOUND)
#define USING_GLUT_FONT_RENDERING
#elif defined(FTGL_FOUND)
#define USING_FTGL_FONT_RENDERING
#else
#error "No Font lib found"
#endif

#if defined(USING_GLUT_FONT_RENDERING)
#include <GL/glut.h>
#endif

#if defined(USING_FTGL_FONT_RENDERING)
#include <FTGL/ftgl.h>
#endif

#include <glutils.h>
#include <stdexcept>
#include <logutil.h>

namespace glutils
{

#if defined(USING_FTGL_FONT_RENDERING)
  FTFont * s_text_font = NULL;
#endif

  void init_text_rendering()
  {
#if defined(USING_GLUT_FONT_RENDERING)

    int argc = 1;
    char *argv[] = {"-gldebug"} ;

    glutInit(&argc,argv);
#endif

#if defined(USING_FTGL_FONT_RENDERING)

    const char * fontfile = "test.ttf";

    s_text_font = new FTTextureFont(fontfile);

    if(s_text_font->Error())
      throw std::runtime_error(std::string("unable to load font file::")+
                               std::string(fontfile));

    s_text_font->FaceSize(20);
    s_text_font->CharMap(ft_encoding_unicode);

#endif

  }

  void render3d_ascii_text(const char * text,int size,vertex_t pos)
  {

#if defined(USING_GLUT_FONT_RENDERING)
    glRasterPos3d ( pos[0],pos[1],pos[2]);

    for ( uint j = 0 ; j < size;j++ )
    {
      char c = text[j];
      glutBitmapCharacter ( GLUT_BITMAP_HELVETICA_12  , c );
    }

#endif

#if defined(USING_FTGL_FONT_RENDERING)
    glTranslated(pos[0],pos[1],pos[2]);
    s_text_font->Render(text);
#endif
  }

  class text_renderer_t: public renderable_t
  {

    GLuint m_render_dl;

  public:

    text_renderer_t
        (  const std::vector<std::string> &texts,
           const std::vector<vertex_t>  &locations):
        m_render_dl(0)
    {

      // check if we have proper arguments
      if(texts.size() != locations.size())
        throw std::length_error("texts and locations have different sizes");

      m_render_dl = glGenLists ( 1 );

      glNewList ( m_render_dl, GL_COMPILE );

      for(uint i = 0 ;i < texts.size(); ++i)
        render3d_ascii_text(texts[i].c_str(),texts[i].size(),locations[i]);

      glEndList();
    }

    virtual int render() const
    {
      if ( m_render_dl )
        glCallList ( m_render_dl );
      else
        throw std::runtime_error("m_render_dl is NULL");

      return 0;
    }

    virtual ~text_renderer_t()
    {
      if ( m_render_dl )
        glDeleteLists ( m_render_dl, 1 );

      m_render_dl = 0;
    }
  };

  renderable_t * create_buffered_text_ren
      (const std::vector<std::string> &s,
       const std::vector<vertex_t> &p)
  {
    return new text_renderer_t(s,p);
  }


}
