/***************************************************************************
 *   Copyright (C) 2009 by nithin,,,   *
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
#ifndef __QUAD_EDGE_H_INCLUDED__
#define __QUAD_EDGE_H_INCLUDED__


typedef unsigned int uint;

struct quad
{

  // offsets that index into a vertex cell list
  uint v;

  // offsets that index into an edge cell list
  uint e;

  // offsets that index into a quad cell list
  // is implicit (ptrdiff)/8 will give the quad cell

  // offset that indexes to a quadlist
  // this shares the edge (v1,v2)
  uint fnext;
};

uint quad_enext ( uint q );

uint quad_eprev ( uint q );

uint quad_sym ( uint q );

class QuadEdge
{
  private:
    quad *m_quad_versions;   // all versions of all quad.. 8 of each

    void * add_algo_pkt;

  public:

    uint *m_verts;   // index list to quads that contain {v1} = {a}
    uint  m_vert_ct;

    uint *m_edges;   // index list to quads that contain {v1,v2} = {a,b}
    uint  m_edge_ct;

    uint *m_quads;   // index list to quads that contain {v1,v2,v3,v4} in consistent orientation
    uint  m_quad_ct; 

    uint *m_boundry_quads; // index list to quads that are on the boundry .. only one version is stored
    uint  m_boundry_quad_ct;

  public:
    QuadEdge();

    void setNumVerts ( const uint & vertex_ct );

    void setNumQuads ( const uint & quad_ct );

    void start_adding_quads();

    void add_quad ( const uint *v );

    void end_adding_quads();

    void logQuad ( const uint &qpos ) const ;

    void logQuadSet ( const uint &quadsetstart ) const;

    void init();

    void destroy();

    uint vertIndex ( uint q ) const;

    uint edgeIndex ( uint q ) const;

    uint quadIndex ( uint q ) const;

    uint quadFnext ( uint q ) const;

    bool hasFnext  ( uint q ) const;

};

#endif
