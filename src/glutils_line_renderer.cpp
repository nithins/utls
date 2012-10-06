#include <algorithm>
#include <stdexcept>

#include <GL/glew.h>

#include <glutils.h>
#include <map>

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

  void compute_line_adj(const line_idx_list_t &ll ,
                        line_adj_idx_list_t &ladj,
                        vertex_list_t &vl)
  {
//    map<int,int> idmap;
//    int E = l.size();

//    for(int i = 0 ; i < E; ++i )
//      for(int j = 0 ; j < 2; ++j )
//        if(idmap.count(l[j]) ==0)
//          idmap.insert(make_pair(l[j],idmap.size()));

//    line_idx_t ll;

//    for(int i = 0 ; i < E; ++i )
//      for(int j = 0 ; j < 2; ++j )
//        ll[i][j] = idmap[l[i][j];

    int V = vl.size();
    int E = ll.size();

    vector<int> off(V+1,0);

    for(int i = 0 ; i < E; ++i)
      for(int j = 0 ; j < 2; ++j )
        off[ll[i][j]] += 1;

    partial_sum(off.begin(),off.end(),off.begin());

    vector<int> adj(off[V-1],-1);

    for(int i = 0 ; i < E; ++i)
    {
      int u = ll[i][0],v = ll[i][1];
      adj[--off[u]] = v;
      adj[--off[v]] = u;
    }

    off[V] = adj.size();

    for(int i = 0 ; i < E; ++i )
    {
      int q = ll[i][0];
      int r = ll[i][1];
      int p = q;
      int s = r;

      if((off[q+1] - off[q] ) == 2)
        p = (adj[off[q]] != r)?(adj[off[q]]):(adj[off[q]+1]);

      if((off[r+1] - off[r] ) == 2)
        s = (adj[off[r]] != q)?(adj[off[r]]):(adj[off[r]+1]);

      ladj.push_back(la::make_vec<idx_t>(p,q,r,s));
    }

    for(int i = 0 ; i < E; ++i )
    {
      idx_t &p = ladj[i][0];
      idx_t &q = ladj[i][1];
      idx_t &r = ladj[i][2];
      idx_t &s = ladj[i][3];

      if(p ==q)
      {
        vl.push_back(vl[q] + vl[q] - vl[r]);
        p = vl.size()-1;
      }

      if(r ==s)
      {
        vl.push_back(vl[r] + vl[r] - vl[q]);
        s = vl.size()-1;
      }
    }
  }

  class lines_adj_ren_t: virtual public renderable_t
  {

  private:
    bufobj_ptr_t m_ver_bo;
    bufobj_ptr_t m_lin_bo;
    bufobj_ptr_t m_col_bo;

    int ( lines_adj_ren_t::*render_func ) () const;

    int m_num_lines;

  public:
    lines_adj_ren_t
        ( const bufobj_ptr_t & ver_buf,
          const bufobj_ptr_t & lin_buf,
          const bufobj_ptr_t & col_buf )
    {
      m_ver_bo   = ver_buf;
      m_lin_bo   = lin_buf;
      m_col_bo   = col_buf;

      ensure_ia(m_ver_bo,"Vertex data unavailable");

      if ( !m_lin_bo)
      {
        if ( !m_col_bo )
          render_func = &lines_adj_ren_t::render_direct_without_color;
        else
          render_func = &lines_adj_ren_t::render_direct_with_color;

        m_num_lines = m_ver_bo->get_num_items()/4;
      }
      else
      {
        if ( !m_col_bo)
          render_func = &lines_adj_ren_t::render_indexed_without_color;
        else
          render_func = &lines_adj_ren_t::render_indexed_with_color;

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

      glDrawElements ( GL_LINES_ADJACENCY, m_num_lines*4, m_lin_bo->src_type(), 0 );

      glBindBuffer ( m_lin_bo->target(), 0 );

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*4;
    }

    int render_indexed_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      glBindBuffer ( m_lin_bo->target(), m_lin_bo->id() );

      glDrawElements ( GL_LINES_ADJACENCY, m_num_lines*4, m_lin_bo->src_type(), 0 );

      glBindBuffer ( m_lin_bo->target(), 0 );

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*4;
    }

    int render_direct_with_color() const
    {
      m_col_bo->bind_to_color_pointer();

      m_ver_bo->bind_to_vertex_pointer();

      glDrawArrays(GL_LINES_ADJACENCY, 0, m_num_lines*4);

      m_col_bo->unbind_from_color_pointer();

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*4;
    }

    int render_direct_without_color() const
    {
      m_ver_bo->bind_to_vertex_pointer();

      glDrawArrays(GL_LINES_ADJACENCY, 0, m_num_lines*4);

      m_ver_bo->unbind_from_vertex_pointer();

      return m_num_lines*4;
    }

    virtual ~lines_adj_ren_t()
    {
    }
  };

  renderable_t * create_lines_adj_ren
  ( bufobj_ptr_t v,
   bufobj_ptr_t l,
   bufobj_ptr_t c)
  {
    return new lines_adj_ren_t(v,l,c);
  }

}
