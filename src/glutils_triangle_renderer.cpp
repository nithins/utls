#include <algorithm>
#include <stdexcept>

#include <GL/glew.h>
#include <glutils_impl.h>
#include <GLSLProgram.h>

#include <logutil.h>
#include <cstring>

extern const char *FLAT_TRIANGLES_VERT;
extern const char *FLAT_TRIANGLES_GEOM;

namespace glutils
{
class buffered_triangles_ren_t: virtual public renderable_t
{

private:
  bufobj_ptr_t m_ver_bo;
  bufobj_ptr_t m_tri_bo;
  bufobj_ptr_t m_nrm_bo;
  bufobj_ptr_t m_col_bo;

  int ( buffered_triangles_ren_t::*render_func ) () const;

  int m_num_triangles;

public:
  buffered_triangles_ren_t
  ( const bufobj_ptr_t & ver_buf,
   const bufobj_ptr_t & tri_buf,
   const bufobj_ptr_t & nrm_buf,
   const bufobj_ptr_t & col_buf )
  {

    m_ver_bo   = ver_buf;
    m_tri_bo   = tri_buf;
    m_nrm_bo   = nrm_buf;
    m_col_bo   = col_buf;

    if ( m_ver_bo.get() == 0 )
    {
      throw std::runtime_error( "no vertex data specified" );
    }

    if ( m_tri_bo.get() == 0)
    {
      throw std::runtime_error( "no triangle index data specified" );
    }

    if ( m_nrm_bo.get() == 0)
    {
      throw std::runtime_error ("no normal data specified" );
    }

    if ( m_col_bo.get() == 0 )
    {
      render_func = &buffered_triangles_ren_t::render_without_color;
    }
    else
    {
      render_func = &buffered_triangles_ren_t::render_with_color;
    }

    m_num_triangles = m_tri_bo->get_num_items();
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

    glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
    glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
    glBindBuffer ( m_tri_bo->target(), 0 );

    m_nrm_bo->unbind_from_normal_pointer();
    m_col_bo->unbind_from_color_pointer();
    m_ver_bo->unbind_from_vertex_pointer();

    return m_num_triangles*3;
  }

  int render_without_color() const
  {
    m_ver_bo->bind_to_vertex_pointer();
    m_nrm_bo->bind_to_normal_pointer();

    glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
    glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
    glBindBuffer ( m_tri_bo->target(), 0 );

    m_nrm_bo->unbind_from_normal_pointer();
    m_ver_bo->unbind_from_vertex_pointer();

    return m_num_triangles*3;
  }

  virtual ~buffered_triangles_ren_t()
  {
  }
};

renderable_t * create_buffered_triangles_ren
( bufobj_ptr_t v,
 bufobj_ptr_t t,
 bufobj_ptr_t n,
 bufobj_ptr_t c )
{
  return new buffered_triangles_ren_t ( v, t, n, c );
}

void compute_vertex_normals
( const vertex_list_t &vlist,const tri_idx_list_t &tlist,normal_list_t & nlist)
{
  nlist.resize(vlist.size());

  memset(nlist.data(),0,nlist.size()*sizeof(normal_list_t::value_type));

  for(int i = 0 ; i< tlist.size();i++)
  {
    tri_idx_t t = tlist[i];

    normal_t  u = vlist[t[0]] - vlist[t[1]];
    normal_t  v = vlist[t[0]] - vlist[t[2]];

    normal_t  n = (u.cross(v)).normalized();

    nlist[t[0]] += n;
    nlist[t[1]] += n;
    nlist[t[2]] += n;
  }

  for(int i = 0 ; i < nlist.size();++i)
  {
    nlist[i].normalize();
  }
}

bufobj_ptr_t make_normals_buf_obj( const vertex_list_t &v,const tri_idx_list_t &t)
{
  normal_list_t n;

  compute_vertex_normals(v,t,n);

  return make_buf_obj(n);
}

GLSLProgram *s_flat_tri_shader = NULL;

class buffered_flat_triangles_ren_t: virtual public renderable_t
{

private:
  bufobj_ptr_t m_ver_bo;
  bufobj_ptr_t m_tri_bo;
  bufobj_ptr_t m_col_bo;

  int ( buffered_flat_triangles_ren_t::*render_func ) () const;

  int m_num_triangles;

public:
  buffered_flat_triangles_ren_t
  ( const bufobj_ptr_t & ver_buf,
   const bufobj_ptr_t & tri_buf,
   const bufobj_ptr_t & col_buf )
  {

    m_ver_bo   = ver_buf;
    m_tri_bo   = tri_buf;
    m_col_bo   = col_buf;

    if ( m_ver_bo.get() == 0 )
    {
      throw std::runtime_error( "no vertex data specified" );
    }

    if ( m_tri_bo.get() == 0)
    {
      throw std::runtime_error( "no triangle index data specified" );
    }

    if ( m_col_bo.get() == 0 )
    {
      render_func = &buffered_flat_triangles_ren_t::render_without_color;
    }
    else
    {
      render_func = &buffered_flat_triangles_ren_t::render_with_color;
    }

    m_num_triangles = m_tri_bo->get_num_items();
  }

  virtual int render()
  {
    return ( this->*render_func ) ();
  }

  int render_with_color() const
  {
    s_flat_tri_shader->use();

    m_col_bo->bind_to_color_pointer();
    m_ver_bo->bind_to_vertex_pointer();

    glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
    glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
    glBindBuffer ( m_tri_bo->target(), 0 );

    m_col_bo->unbind_from_color_pointer();
    m_ver_bo->unbind_from_vertex_pointer();

    s_flat_tri_shader->disable();

    return m_num_triangles*3;
  }

  int render_without_color() const
  {
    s_flat_tri_shader->use();

    m_ver_bo->bind_to_vertex_pointer();

    glBindBuffer ( m_tri_bo->target(), m_tri_bo->id() );
    glDrawElements ( GL_TRIANGLES, m_num_triangles*3, m_tri_bo->src_type(), 0 );
    glBindBuffer ( m_tri_bo->target(), 0 );

    m_ver_bo->unbind_from_vertex_pointer();

    s_flat_tri_shader->disable();

    return m_num_triangles*3;
  }

  virtual ~buffered_flat_triangles_ren_t()
  {
  }
};

renderable_t * create_buffered_flat_triangles_ren
( bufobj_ptr_t v,
 bufobj_ptr_t t,
 bufobj_ptr_t c)
{
  if(s_flat_tri_shader == NULL)
  {

    s_flat_tri_shader = GLSLProgram::createFromSourceStrings
        (FLAT_TRIANGLES_VERT,FLAT_TRIANGLES_GEOM,"");

  }

  return new buffered_flat_triangles_ren_t ( v, t, c );
}

}
