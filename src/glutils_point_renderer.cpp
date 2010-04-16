#include <algorithm>
#include <stdexcept>

#include <GL/glew.h>

#include <glutils.h>

namespace glutils
{

  class buffered_points_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_col_bo;
    bufobj_ptr_t m_idx_bo;

    int ( buffered_points_ren_t::*render_func ) () const;

    int m_num_points;

    double m_extent[6];

  public:
    buffered_points_ren_t
        ( bufobj_ptr_t ver_buf,
          bufobj_ptr_t idx_buf,
          bufobj_ptr_t col_buf)
    {

      m_ver_bo   = ver_buf;
      m_col_bo   = col_buf;
      m_idx_bo   = idx_buf;

      if ( m_ver_bo->id() == 0 && m_ver_bo->src_ptr() == NULL )
      {
        throw std::invalid_argument
            ( "no vertex data specified neither in cpu nor gpu" );
      }

      enum e_render_func {DIRECT_WOC,DIRECT_WC,INDEXED_WOC,INDEXED_WC};

      e_render_func render_func_type
          = (e_render_func)
            ( (( m_col_bo->id() == 0 && m_col_bo->src_ptr() == NULL )?(0):(1) )|
            (( m_idx_bo->id() == 0 && m_idx_bo->src_ptr() == NULL )?(0):(2)));



      switch(render_func_type)
      {
      case DIRECT_WOC:
        render_func = &buffered_points_ren_t::render_without_color_dir;
        m_num_points = m_ver_bo->get_num_items();break;
      case DIRECT_WC:
        render_func = &buffered_points_ren_t::render_with_color_dir;
        m_num_points = m_ver_bo->get_num_items();break;
      case INDEXED_WOC:
        render_func = &buffered_points_ren_t::render_without_color_idx;
        m_num_points = m_idx_bo->get_num_items();break;
      case INDEXED_WC:
        render_func = &buffered_points_ren_t::render_with_color_idx;
        m_num_points = m_idx_bo->get_num_items();break;
      default:
        throw std::logic_error("renderfunc type undetermined");
      }

      compute_extent ( m_ver_bo, m_extent );
    }

    virtual int render() const
    {
      return ( this->*render_func ) ();
    }

    int render_with_color_dir() const
    {
      m_col_bo->bind_to_color_pointer();

      m_ver_bo->bind_to_vertex_pointer();

      glDrawArrays(GL_POINTS,0,m_num_points);

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    int render_without_color_dir() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      glDrawArrays(GL_POINTS,0,m_num_points);

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    int render_with_color_idx() const
    {
      m_col_bo->bind_to_color_pointer();

      m_ver_bo->bind_to_vertex_pointer();

      glBindBuffer ( m_idx_bo->target(), m_idx_bo->id());

      glDrawElements(GL_POINTS,m_num_points,m_idx_bo->src_type(),0);

      glBindBuffer ( m_idx_bo->target(), 0);

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    int render_without_color_idx() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      glBindBuffer ( m_idx_bo->target(), m_idx_bo->id());

      glDrawElements(GL_POINTS,m_num_points,m_idx_bo->src_type(),0);

      glBindBuffer ( m_idx_bo->target(), 0);

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_points;
    }

    virtual ~buffered_points_ren_t()
    {
    }

    virtual bool get_extent ( double * extent )
    {
      std::copy ( m_extent, m_extent + 6, extent );
      return true;
    }
  };

  renderable_t * create_buffered_points_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t i,
        bufobj_ptr_t c)
  {
    return new buffered_points_ren_t(v,i,c);
  }
}
