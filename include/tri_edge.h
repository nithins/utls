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

class TriEdge
{
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
    TriEdge();

    ~TriEdge();

    void setNumVerts ( const uint & vertex_ct );

    void setNumTris ( const uint & tri_ct );

    void start_adding_tris();

    void add_tri ( const uint *v );

    void end_adding_tris();

    void setup(const glutils::tri_idx_list_t &,const uint & num_verts);

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

typedef boost::shared_ptr<TriEdge> tri_edge_ptr_t;

class tri_cell_complex_t
{
  tri_edge_ptr_t m_tri_edge;

public:

  static const uint cc_dim = 2;

  typedef uint cellid_t;

  tri_cell_complex_t(tri_edge_ptr_t ptr): m_tri_edge(ptr){}

  tri_cell_complex_t():m_tri_edge(new TriEdge){}

  inline tri_edge_ptr_t get_tri_edge() const;

  inline uint get_dim() const;

  uint get_cell_dim (cellid_t c) const ;

  uint get_cell_points (cellid_t  ,cellid_t   * ) const;

  uint get_cell_facets (cellid_t  ,cellid_t  * ) const;

  uint get_cell_co_facets (cellid_t  ,cellid_t  * ) const;

  uint get_vert_star(cellid_t  ,cellid_t  * ) const;

  bool is_cell_boundry(cellid_t ) const;

  inline uint get_num_cells ();

  uint get_num_cells_dim (uint dim);

  inline uint get_num_cells_max_dim (uint dim);
};

inline tri_edge_ptr_t tri_cell_complex_t::get_tri_edge() const
{
  return m_tri_edge;
}

inline uint tri_cell_complex_t::get_num_cells_max_dim (uint dim)
{
  uint n = 0;

  for(uint i = 0 ; i <= dim; ++i)
    n += get_num_cells_dim(i);

  return n;
}

inline uint tri_cell_complex_t::get_num_cells ()
{
  return get_num_cells_max_dim(cc_dim);
}

inline uint tri_cell_complex_t::get_dim() const
{
  return cc_dim;
}





#endif
