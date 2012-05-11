#include <algorithm>
#include <cstring>

#include <GL/glew.h>

#include <glutils_impl.h>

namespace glutils
{
  class buffered_quad_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_quad_bo;
    bufobj_ptr_t m_nrm_bo;
    bufobj_ptr_t m_col_bo;

    int ( buffered_quad_ren_t::*render_func ) () const;

    int m_num_quads;

  public:
    buffered_quad_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & quad_buf,
          const bufobj_ptr_t & nrm_buf,
          const bufobj_ptr_t & col_buf )
    {

      m_ver_bo   = ver_buf;
      m_quad_bo   = quad_buf;
      m_nrm_bo   = nrm_buf;
      m_col_bo   = col_buf;

      if ( m_ver_bo.get() == 0 )
      {
        throw std::runtime_error( "no vertex data specified" );
      }

      if ( m_quad_bo.get() == 0)
      {
        throw std::runtime_error( "no triangle index data specified" );
      }

      if ( m_nrm_bo.get() == 0)
      {
        throw std::runtime_error ("no normal data specified" );
      }

      if ( m_col_bo.get() == 0 )
      {
        render_func = &buffered_quad_ren_t::render_without_color;
      }
      else
      {
        render_func = &buffered_quad_ren_t::render_with_color;
      }

      m_num_quads = m_quad_bo->get_num_items();
    }

    virtual int render()
    {
      return ( this->*render_func ) ();
    }

    int render_with_color() const
    {
      m_col_bo->bind_to_color_pointer();
      m_ver_bo->bind_to_vertex_pointer();
      m_nrm_bo->bind_to_normal_pointer();

      glBindBuffer ( m_quad_bo->target(), m_quad_bo->id() );
      glDrawElements ( GL_QUADS, m_num_quads*4, m_quad_bo->src_type(), 0 );
      glBindBuffer ( m_quad_bo->target(), 0 );

      m_nrm_bo->unbind_from_normal_pointer();
      m_col_bo->unbind_from_color_pointer();
      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_quads*4;
    }

    int render_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();
      m_nrm_bo->bind_to_normal_pointer();

      glBindBuffer ( m_quad_bo->target(), m_quad_bo->id() );
      glDrawElements ( GL_QUADS, m_num_quads*4, m_quad_bo->src_type(), 0 );
      glBindBuffer ( m_quad_bo->target(), 0 );

      m_nrm_bo->unbind_from_normal_pointer();
      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_quads*4;
    }
  };

  void compute_vertex_normals
      ( const vertex_list_t   &vlist,
        const quad_idx_list_t &qlist,
        normal_list_t & nlist)
  {
    nlist.resize(vlist.size());

    std::memset(nlist.data(),0,nlist.size()*sizeof(normal_list_t::value_type));

    for(int i = 0 ; i< qlist.size();i++)
    {
      quad_idx_t q = qlist[i];

      normal_t  u = vlist[q[0]] - vlist[q[1]];
      normal_t  v = vlist[q[0]] - vlist[q[2]];
      normal_t  n = cross_product(u,v);

      u  = vlist[q[0]] - vlist[q[2]];
      v  = vlist[q[0]] - vlist[q[3]];
      n += cross_product(u,v);

      n = boost::numeric::ublas::norm_2(n);

      nlist[q[0]] += n;
      nlist[q[1]] += n;
      nlist[q[2]] += n;
      nlist[q[3]] += n;
    }

    for(int i = 0 ; i < nlist.size();++i)
    {
      nlist[i] = boost::numeric::ublas::norm_2(nlist[i]);
    }
  }


  renderable_t * create_buffered_quads_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t n,
        bufobj_ptr_t c )
  {
    return new buffered_quad_ren_t( v, t, n, c );
  }

}
