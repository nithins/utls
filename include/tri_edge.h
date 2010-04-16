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


typedef unsigned int uint;

struct tri
{

  // offsets that index into a vertex cell list
  uint v;

  // offsets that index into an edge cell list
  uint e;

  // offsets that index into a tri cell list
  // is implicit (ptrdiff)/8 will give the tri cell

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

    uint *m_tris;   // index list to tris that contain {v1,v2,v3,v4} in consistent orientation
    uint  m_tri_ct;

    uint *m_boundry_tris; // index list to tris that are on the boundry .. only one version is stored
    uint  m_boundry_tri_ct;

    uint *m_vert_degree;
    uint  m_max_vert_degree;

    uint  m_boundry_cycle_ct;

  public:
    TriEdge();

    void setNumVerts ( const uint & vertex_ct );

    void setNumTris ( const uint & tri_ct );

    void start_adding_tris();

    void add_tri ( const uint *v );

    void end_adding_tris();

    void logTri ( const uint &qpos ) const ;

    void logTriSet ( const uint &trisetstart ) const;

    void init();

    void destroy();

    uint vertIndex ( uint q ) const;

    uint edgeIndex ( uint q ) const;

    uint triIndex ( uint q ) const;

    uint triFnext ( uint q ) const;

    bool hasFnext ( uint q ) const;

    static TriEdge* readFromFile ( char * filename ); // for now only ply file is supported
};

#endif
