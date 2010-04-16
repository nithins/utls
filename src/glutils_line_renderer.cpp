#include <algorithm>

#include <GL/glew.h>

#include <glutils.h>

using namespace std;

namespace glutils
{

  class buffered_lines_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_lin_bo;
    bufobj_ptr_t m_col_bo;

    int ( buffered_lines_ren_t::*render_func ) () const;

    int m_num_lines;

    double m_extent[6];

  public:
    buffered_lines_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & lin_buf,
          const bufobj_ptr_t & col_buf )
    {

      m_ver_bo   = ver_buf;
      m_lin_bo   = lin_buf;
      m_col_bo   = col_buf;

      if ( m_ver_bo->id() == 0 && m_ver_bo->src_ptr() == NULL )
      {
        throw std::invalid_argument
            ( "no vertex data specified neither in cpu nor gpu" );
      }

      if ( m_lin_bo->id() == 0 && m_lin_bo->src_ptr() == NULL )
      {
        throw std::invalid_argument
            ( "no line index data specified neither in cpu nor gpu" );
      }

      if ( m_col_bo->id() == 0 && m_col_bo->src_ptr() == NULL )
      {
        render_func = &buffered_lines_ren_t::render_without_color;
      }
      else
      {
        render_func = &buffered_lines_ren_t::render_with_color;
      }

      compute_extent ( m_ver_bo, m_extent );

      m_num_lines = m_lin_bo->get_num_items();

    }

    virtual int render() const
    {
      return ( this->*render_func ) ();
    }

    int render_with_color() const
    {
      m_col_bo->bind_to_color_pointer();

      m_ver_bo->bind_to_vertex_pointer();

      glBindBuffer ( m_lin_bo->target(), m_lin_bo->id() );

      glDrawElements ( GL_LINES, m_num_lines*2, m_lin_bo->src_type(), 0 );

      glBindBuffer ( m_lin_bo->target(), 0 );

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*2;
    }

    int render_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      glBindBuffer ( m_lin_bo->target(), m_lin_bo->id() );

      glDrawElements ( GL_LINES, m_num_lines*2, m_lin_bo->src_type(), 0 );

      glBindBuffer ( m_lin_bo->target(), 0 );

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*2;
    }

    virtual ~buffered_lines_ren_t()
    {
    }

    virtual bool get_extent ( double * extent )
    {
      copy ( m_extent, m_extent + 6, extent );
      return true;
    }
  };

  renderable_t * create_buffered_lines_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t l,
        bufobj_ptr_t c )
  {
    return new buffered_lines_ren_t ( v, l, c );
  }

}
