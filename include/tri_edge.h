/***************************************************************************
 *   Copyright (C) 2009 by Nithin Shivashankar,   *
 *   nithin@gauss   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef __TRI_EDGE_H_INCLUDED__
#define __TRI_EDGE_H_INCLUDED__

#include <cpputils.h>

#include <boost/shared_ptr.hpp>

#include <glutils.h>

struct tri
{

  // offsets that index into a vertex cell list
  uint v;

  // offsets that index into an edge cell list
  uint e;

  // offsets that index into a tri cell list
  // is implicit (ptrdiff)/6 will give the tri cell

  // offset that indexes to a trilist
  // this shares the edge (v1,v2)
  uint fnext;
};

uint tri_enext ( uint q );

uint tri_eprev ( uint q );

uint tri_sym ( uint q );

class tri_edge_t
{
public:

  typedef uint                    cellid_t;

  typedef glutils::tri_idx_list_t tri_idx_list_t;

  typedef glutils::tri_idx_t      tri_idx_t;

private:
  tri *m_tri_versions;   // all versions of all tri.. 8 of each

  void * add_algo_pkt;

public:

  uint *m_verts;   // index list to tris that contain {v1} = {a}
  uint  m_vert_ct;

  uint *m_edges;   // index list to tris that contain {v1,v2} = {a,b}
  uint  m_edge_ct;

  uint *m_tris;   // index list to tris that contain {v1,v2,v3} in consistent orientation
  uint  m_tri_ct;

public:
  tri_edge_t();

  ~tri_edge_t();

  void setNumVerts ( const uint & vertex_ct );

  void setNumTris ( const uint & tri_ct );

  void start_adding_tris();

  void add_tri ( const uint *v );

  void end_adding_tris();

  void setup(const tri_idx_list_t &,const uint & num_verts);

  void logTri ( const uint &qpos ) const ;

  void logTriSet ( const uint &trisetstart ) const;

  void init();

  void destroy();

  uint vertIndex ( uint q ) const;

  uint edgeIndex ( uint q ) const;

  uint triIndex ( uint q ) const;

  uint triFnext ( uint q ) const;

  bool hasFnext ( uint q ) const;
};

typedef boost::shared_ptr<tri_edge_t> tri_edge_ptr_t;

class tri_cc_t
{  
public:

  typedef uint                       cellid_t;

  typedef tri_edge_t::tri_idx_list_t tri_idx_list_t;

  typedef tri_edge_t::tri_idx_t      tri_idx_t;

  static const uint cc_dim = 2;

protected:

  tri_edge_ptr_t m_tri_edge;

public:

  tri_cc_t():m_tri_edge(new tri_edge_t){}

  inline void init(const tri_idx_list_t &,const uint & num_verts);

  inline void clear();

  inline uint get_dim() const;

  uint get_cell_dim (cellid_t c) const ;

  uint get_cell_points (cellid_t  ,cellid_t   * ) const;

  uint get_cell_facets (cellid_t  ,cellid_t  * ) const;

  uint get_cell_co_facets (cellid_t  ,cellid_t  * ) const;

  uint get_vert_star(cellid_t  ,cellid_t  * ) const;

  bool is_adjacent(cellid_t  ,cellid_t ) const;

  bool is_cell_boundry(cellid_t ) const;

  inline uint get_num_cells () const;

  uint get_num_cells_dim (uint dim) const;

  inline uint get_num_cells_max_dim (uint dim) const;

  void save(std::ostream & os,const glutils::vertex_list_t& v,const std::vector<double> & fn) const;
};

typedef boost::shared_ptr<tri_cc_t> tri_cc_ptr_t;

class tri_cc_geom_t
{

public:

  typedef tri_cc_t::cellid_t       cellid_t;

  typedef tri_cc_t::tri_idx_list_t tri_idx_list_t;

  typedef tri_cc_t::tri_idx_t      tri_idx_t;

  typedef glutils::vertex_t        vertex_t;

  typedef glutils::vertex_list_t   vertex_list_t;

  typedef glutils::vertex_t        normal_t;

  typedef glutils::vertex_list_t   normal_list_t;

  static const uint cc_dim =       tri_cc_t::cc_dim;

protected:

  tri_cc_ptr_t  m_tri_cc;

  vertex_list_t m_cell_pos;

  normal_list_t m_cell_normal;

  double        m_average_length;

protected:

  double compute_average_length();

public:

  void init(const tri_idx_list_t &,const vertex_list_t &);

  void init(const tri_cc_ptr_t &,const vertex_list_t &);

  void clear();

  inline uint get_dim() const
  { return m_tri_cc->get_dim();  }

  inline uint get_cell_dim (cellid_t c) const
  { return m_tri_cc->get_cell_dim(c);  }

  inline uint get_cell_points (cellid_t  c,cellid_t   * cl) const
  { return m_tri_cc->get_cell_points(c,cl);  }

  inline uint get_cell_facets (cellid_t  c,cellid_t  * cl) const
  { return m_tri_cc->get_cell_facets(c,cl);  }

  inline uint get_cell_co_facets (cellid_t  c,cellid_t  * cl) const
  { return m_tri_cc->get_cell_co_facets(c,cl);  }

  inline uint get_vert_star(cellid_t  c,cellid_t  * cl) const
  { return m_tri_cc->get_vert_star(c,cl);  }

  inline bool is_cell_boundry(cellid_t c) const
  { return m_tri_cc->is_cell_boundry(c);  }

  inline uint get_num_cells () const
  { return m_tri_cc->get_num_cells();  }

  inline uint get_num_cells_dim (uint d) const
  { return m_tri_cc->get_num_cells_dim(d);  }

  inline uint get_num_cells_max_dim (uint d) const
  { return m_tri_cc->get_num_cells_max_dim(d);  }

  inline vertex_t get_cell_position(cellid_t c) const
  { return m_cell_pos[c]; }

  inline normal_t get_cell_normal(cellid_t c) const
  { return m_cell_normal[c]; }

  inline const vertex_list_t &get_cell_positions() const
  { return m_cell_pos; }

  inline const normal_list_t &get_cell_normals() const
  { return m_cell_normal; }

  inline double get_average_edge_length() const
  { return m_average_length;}

};

typedef boost::shared_ptr<tri_cc_geom_t> tri_cc_geom_ptr_t;

inline void tri_cc_t::init(const tri_idx_list_t &tl,const uint & nv)
{
  m_tri_edge->setup(tl,nv);
}

inline void tri_cc_t::clear()
{
  m_tri_edge->destroy();
}

inline uint tri_cc_t::get_num_cells_max_dim (uint dim) const
{
  uint n = 0;

  for(uint i = 0 ; i <= dim; ++i)
    n += get_num_cells_dim(i);

  return n;
}

inline uint tri_cc_t::get_num_cells () const
{
  return get_num_cells_max_dim(cc_dim);
}

inline uint tri_cc_t::get_dim() const
{
  return cc_dim;
}

#endif
