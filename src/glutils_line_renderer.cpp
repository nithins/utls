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

  public:
    buffered_lines_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & lin_buf,
          const bufobj_ptr_t & col_buf )
    {

      m_ver_bo   = ver_buf;
      m_lin_bo   = lin_buf;
      m_col_bo   = col_buf;

      if ( !m_ver_bo.get())
      {
        throw std::invalid_argument
            ( "no vertex data specified" );
      }

      if ( m_lin_bo == NULL)
      {
        if ( m_col_bo == NULL )
        {
          render_func = &buffered_lines_ren_t::render_direct_without_color;
        }
        else
        {
          render_func = &buffered_lines_ren_t::render_direct_with_color;
        }

        m_num_lines = m_ver_bo->get_num_items()/2;
      }
      else
      {
        if ( m_col_bo.get() == NULL)
        {
          render_func = &buffered_lines_ren_t::render_indexed_without_color;
        }
        else
        {
          render_func = &buffered_lines_ren_t::render_indexed_with_color;
        }

        m_num_lines = m_lin_bo->get_num_items();
      }
    }

    virtual int render()
    {
      return ( this->*render_func ) ();
    }

    int render_indexed_with_color() const
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

    int render_indexed_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      glBindBuffer ( m_lin_bo->target(), m_lin_bo->id() );

      glDrawElements ( GL_LINES, m_num_lines*2, m_lin_bo->src_type(), 0 );

      glBindBuffer ( m_lin_bo->target(), 0 );

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*2;
    }

    int render_direct_with_color() const
    {
      m_col_bo->bind_to_color_pointer();

      m_ver_bo->bind_to_vertex_pointer();

      glDrawArrays(GL_LINES, 0, m_num_lines*2);

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*2;
    }

    int render_direct_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      glDrawArrays(GL_LINES, 0, m_num_lines*2);

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*2;
    }


    virtual ~buffered_lines_ren_t()
    {
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
