#include <stdexcept>

#include <GL/glew.h>

#include <glutils.h>
#include <GLSLProgram.h>

extern const char *NORMAL_REN_VERT_GLSL;
extern const char *NORMAL_REN_GEOM_GLSL;
extern const char *NORMAL_REN_FRAG_GLSL;

GLSLProgram * s_normal_shader = NULL;

namespace glutils
{
  void init_normal_renderer()
  {
    if(s_normal_shader != NULL)
      return;

    s_normal_shader = GLSLProgram::createFromSourceStrings
                      (NORMAL_REN_VERT_GLSL,NORMAL_REN_GEOM_GLSL,NORMAL_REN_FRAG_GLSL);
  }

  void clear_normal_renderer()
  {
    if(s_normal_shader == NULL)
      return;

    delete s_normal_shader;

    s_normal_shader = NULL;
  }

  class buffered_normals_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_col_bo;
    bufobj_ptr_t m_idx_bo;
    bufobj_ptr_t m_nrm_bo;

    int ( buffered_normals_ren_t::*render_func ) () const;

    int m_num_points;

    double m_normal_length;

    enum e_render_func {DIRECT_WOC,DIRECT_WC,INDEXED_WOC,INDEXED_WC};

  public:
    buffered_normals_ren_t
        ( bufobj_ptr_t ver_buf,
          bufobj_ptr_t idx_buf,
          bufobj_ptr_t col_buf,
          bufobj_ptr_t nrm_buf,
          double n_len)
    {

      m_ver_bo   = ver_buf;
      m_col_bo   = col_buf;
      m_idx_bo   = idx_buf;
      m_nrm_bo   = nrm_buf;

      m_normal_length = n_len;

      if ( m_ver_bo == NULL)
      {
        throw std::invalid_argument
            ( "no vertex data specified neither in cpu nor gpu" );
      }

      if ( m_nrm_bo == NULL )
      {
        throw std::invalid_argument
            ( "no normal data specified neither in cpu nor gpu" );
      }


      e_render_func render_func_type
          = (e_render_func)
            ( (( m_col_bo == NULL)?(0):(1) )|(( m_idx_bo == NULL )?(0):(2)));


      switch(render_func_type)
      {
      case DIRECT_WOC:
        render_func = &buffered_normals_ren_t::render_without_color_dir;
        m_num_points = m_ver_bo->get_num_items();break;
      case DIRECT_WC:
        render_func = &buffered_normals_ren_t::render_with_color_dir;
        m_num_points = m_ver_bo->get_num_items();break;
      case INDEXED_WOC:
        render_func = &buffered_normals_ren_t::render_without_color_idx;
        m_num_points = m_idx_bo->get_num_items();break;
      case INDEXED_WC:
        render_func = &buffered_normals_ren_t::render_with_color_idx;
        m_num_points = m_idx_bo->get_num_items();break;
      default:
        throw std::logic_error("renderfunc type undetermined");
      }
    }

    virtual int render()
    {
      s_normal_shader->use();

      s_normal_shader->sendUniform("normal_length",(float)m_normal_length);

      int ret = ( this->*render_func ) ();

      s_normal_shader->disable();

      return ret;
    }

    int render_with_color_dir() const
    {
      m_col_bo->bind_to_color_pointer();

      m_ver_bo->bind_to_vertex_pointer();

      m_nrm_bo->bind_to_normal_pointer();

      glDrawArrays(GL_POINTS,0,m_num_points);

      m_nrm_bo->unbind_from_normal_pointer();

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    int render_without_color_dir() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      m_nrm_bo->bind_to_normal_pointer();

      glDrawArrays(GL_POINTS,0,m_num_points);

      m_nrm_bo->unbind_from_normal_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    int render_with_color_idx() const
    {
      m_col_bo->bind_to_color_pointer();

      m_ver_bo->bind_to_vertex_pointer();

      m_nrm_bo->bind_to_normal_pointer();

      glBindBuffer ( m_idx_bo->target(), m_idx_bo->id());

      glDrawElements(GL_POINTS,m_num_points,m_idx_bo->src_type(),0);

      glBindBuffer ( m_idx_bo->target(), 0);

      m_nrm_bo->unbind_from_normal_pointer();

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    int render_without_color_idx() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      m_nrm_bo->bind_to_normal_pointer();

      glBindBuffer ( m_idx_bo->target(), m_idx_bo->id());

      glDrawElements(GL_POINTS,m_num_points,m_idx_bo->src_type(),0);

      glBindBuffer ( m_idx_bo->target(), 0);

      m_nrm_bo->unbind_from_normal_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    virtual ~buffered_normals_ren_t()
    {
    }
  };

  renderable_t * create_buffered_normals_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t i,
        bufobj_ptr_t c,
        bufobj_ptr_t n,
        double n_len)
  {
    return new buffered_normals_ren_t(v,i,c,n,n_len);
  }
}
