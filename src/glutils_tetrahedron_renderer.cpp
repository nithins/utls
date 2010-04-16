#include <algorithm>

#include <GL/glew.h>

#include <glutils.h>
#include <GLSLProgram.h>

// file generated by copy_files_to_c_header.py
// to include shader source code as c strings
#include <shadersources.h>
#include <logutil.h>


namespace glutils
{

  class buffered_flat_tetrahedrons_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_tet_bo;
    bufobj_ptr_t m_col_bo;

    int ( buffered_flat_tetrahedrons_ren_t::*render_func ) () const;

    int m_num_tetrahedrons;

    double m_extent[6];

    static GLSLProgram * s_flat_tet_shader;

  public:
    buffered_flat_tetrahedrons_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & tet_buf,
          const bufobj_ptr_t & col_buf )
    {

      m_ver_bo   = ver_buf;
      m_tet_bo   = tet_buf;
      m_col_bo   = col_buf;

      if ( s_flat_tet_shader == NULL )
      {
        s_flat_tet_shader = GLSLProgram::createFromSourceStrings
                            (
                                std::string ( flat_tetrahedrons_vert ),
                                std::string ( flat_tetrahedrons_geom ),
                                std::string (),
                                GL_LINES_ADJACENCY_EXT,
                                GL_TRIANGLES
                                );

        std::string flat_tet_shader_log;

        s_flat_tet_shader->GetProgramLog ( flat_tet_shader_log );

        _LOG_VAR ( flat_tet_shader_log );
      }

      if ( m_ver_bo->id() == 0 && m_ver_bo->src_ptr() == NULL )
      {
        _ERROR ( "no vertex data specified neither in cpu nor gpu" );
      }

      if ( m_tet_bo->id() == 0 && m_tet_bo->src_ptr() == NULL )
      {
        _ERROR ( "no triangle index data specified neither in cpu nor gpu" );
      }

      if ( m_col_bo->id() == 0 && m_col_bo->src_ptr() == NULL )
      {
        render_func = &buffered_flat_tetrahedrons_ren_t::render_without_color;
      }
      else
      {
        render_func = &buffered_flat_tetrahedrons_ren_t::render_with_color;
      }

      compute_extent ( m_ver_bo, m_extent );

      m_num_tetrahedrons = m_tet_bo->get_num_items();

    }

    virtual int render() const
    {
      return ( this->*render_func ) ();
    }

    int render_with_color() const
    {
      m_col_bo->bind_to_color_pointer();
      m_ver_bo->bind_to_vertex_pointer();

      s_flat_tet_shader->use();

      glBindBuffer ( m_tet_bo->target(), m_tet_bo->id() );
      glDrawElements ( GL_LINES_ADJACENCY_EXT, m_num_tetrahedrons*4, m_tet_bo->src_type(), 0 );
      glBindBuffer ( m_tet_bo->target(), 0 );

      s_flat_tet_shader->disable();

      m_col_bo->unbind_from_color_pointer();
      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_tetrahedrons*4;
    }

    int render_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      s_flat_tet_shader->use();

      glBindBuffer ( m_tet_bo->target(), m_tet_bo->id() );
      glDrawElements ( GL_LINES_ADJACENCY_EXT, m_num_tetrahedrons*4, m_tet_bo->src_type(), 0 );
      glBindBuffer ( m_tet_bo->target(), 0 );

      s_flat_tet_shader->disable();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_tetrahedrons*4;
    }
    virtual ~buffered_flat_tetrahedrons_ren_t()
    {

    }

    virtual bool get_extent ( double * extent )
    {
      std::copy ( m_extent, m_extent + 6, extent );
      return true;
    }
  };

  GLSLProgram * buffered_flat_tetrahedrons_ren_t::s_flat_tet_shader = NULL;

  renderable_t * create_buffered_flat_tetrahedrons_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t c )
  {
    return new buffered_flat_tetrahedrons_ren_t ( v, t, c );
  }

}
