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

#include <map>
#include <vector>
#include <queue>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <algorithm>


#include <cpputils.h>
#include <logutil.h>

#include <tri_edge.h>

#define INVALID_VALUE 0xffffffff

#define DEBUG_BEGIN if(false) {
#define DEBUG_END }
#define DEBUG_STMT(x) DEBUG_BEGIN x; DEBUG_END;
#define DEBUG_LOG(x) DEBUG_STMT(_LOG(x))

using namespace std;

uint tri_enext ( uint q )
{
  return ( 3* ( q/3 ) + ( q+1 ) %3 );
}

uint tri_eprev ( uint q )
{
  return ( 3* ( q/3 ) + ( q+2 ) %3 );
}

uint tri_sym ( uint q )
{
  return ( q + 5 ) -2* ( q%6 );
}

TriEdge::TriEdge()
{
  init();
}

TriEdge::~TriEdge()
{
  destroy();
}

void TriEdge::setNumVerts ( const uint & vertex_ct )
{
  m_vert_ct = vertex_ct;
  m_verts  = new uint[vertex_ct];
}

void TriEdge::setNumTris ( const uint & tri_ct )
{
  m_tri_ct         = tri_ct;
  m_tri_versions   = new tri[ tri_ct*6];
}

typedef map< pair<uint,uint>,uint,unordered_pair_comparator > edge_map_t;
struct qaapkt
{
  uint           tri_pos;
  edge_map_t     edge_map;
  vector<uint>   edge_list;// list of tris that have {v1,v2} = {u,v}

  qaapkt() :tri_pos ( 0 ),edge_map ( unordered_pair_comparator() ) {}
};

void TriEdge::start_adding_tris()
{
  add_algo_pkt = new qaapkt();
}

void TriEdge::add_tri ( const uint *v )
{

  vector<uint> &edge_list = ( ( qaapkt* ) add_algo_pkt )->edge_list;
  edge_map_t   &edge_map  = ( ( qaapkt* ) add_algo_pkt )->edge_map;
  uint         &tri_pos  = ( ( qaapkt* ) add_algo_pkt )->tri_pos;

  // each tri has 6 versions

  // the first four versions
  for ( int i = 0 ; i < 3 ; i++ )
  {
    m_tri_versions[ tri_pos + i].v = v[i];
    m_verts[v[i]] = tri_pos + i;
  }

  for ( int i = 0 ; i < 3 ; i++ )
  {
    tri *q1 = &m_tri_versions[ tri_pos + i];
    tri *q2 = &m_tri_versions[ tri_pos + ( i+1 ) %3];

    edge_map_t::iterator it = edge_map.find ( make_pair ( q1->v, q2->v ) );

    if ( it != edge_map.end() )
    {
      q1->fnext = ( m_tri_versions[ ( *it ).second].v == q2->v ) ? ( ( *it ).second ) : ( tri_sym ( ( *it ).second ) );
      m_tri_versions[q1->fnext].fnext = tri_pos + i;
      q1->e = m_tri_versions[q1->fnext].e;
      edge_map.erase ( it );
    }
    else
    {
      q1->e = edge_list.size();
      q1->fnext = INVALID_VALUE;
      edge_list.push_back ( tri_pos + i );
      edge_map.insert ( make_pair ( make_pair ( q1->v,q2->v ), tri_pos + i ) );
    }
  }

  // the next four are just reflections
  for ( int i = 0 ; i < 3 ; i++ )
  {
    uint qpos = tri_pos + 3 + i;
    tri *q = &m_tri_versions[qpos];

    q->v     = m_tri_versions[tri_enext ( tri_sym ( qpos ) ) ].v;
    q->e     = m_tri_versions[tri_sym ( qpos ) ].e;

    if ( m_tri_versions[tri_sym ( qpos ) ].fnext != INVALID_VALUE )
    {
      q->fnext = tri_sym ( m_tri_versions[tri_sym ( qpos ) ].fnext );
      m_tri_versions[q->fnext].fnext = qpos;
    }
    else
    {
      q->fnext = INVALID_VALUE;
    }
  }
  tri_pos+=6;
}

void TriEdge::end_adding_tris()
{
  vector<uint> &edge_list = ( ( qaapkt* ) add_algo_pkt )->edge_list;
  edge_map_t   &edge_map  = ( ( qaapkt* ) add_algo_pkt )->edge_map;
  //uint         &tri_pos  = ( ( qaapkt* ) add_algo_pkt )->tri_pos;


  m_edge_ct = edge_list.size();
  m_edges = new uint[m_edge_ct];

  for ( uint i = 0 ; i < m_edge_ct;i++ )
  {
    m_edges[i] = edge_list [i];
  }

  m_tris = new uint[m_tri_ct];
  for ( uint i = 0 ; i < m_tri_ct;i++ )
  {
    m_tris[i] = m_tri_ct*6;
  }

  // this is done so that the versions of the tris we look at are cyclically consistant
  // the outer loop will invoke the inner loop once for each connected component
  for ( uint i = 0 ; i < m_tri_ct;i++ )
  {
    if ( m_tris[i] == m_tri_ct*6 )
    {
      queue<uint> tri_queue;

      tri_queue.push ( i*6 );

      while ( tri_queue.size() !=0 )
      {
        uint q = tri_queue.front();
        tri_queue.pop();

        m_tris[q/6] = ( q/6 ) *6 + ( ( q%6 ) /3 ) *3;

        for ( int i = 0 ; i< 3;i++ )
        {
          q = tri_enext ( q );

          uint qfnext = m_tri_versions[q].fnext;

          if ( qfnext != INVALID_VALUE )
          {
            if ( ( m_tris[qfnext/6] == m_tri_ct*6 ) )
            {
              m_tris[qfnext/6] = 0;
              tri_queue.push ( qfnext );
            }
          }
        }
      }
    }
  }

  delete ( ( qaapkt* ) add_algo_pkt );
}

void TriEdge::setup(const glutils::tri_idx_list_t &tlist,const uint & vert_ct)
{
  setNumVerts(vert_ct);

  setNumTris(tlist.size());

  start_adding_tris();

  for(uint i = 0 ;i < tlist.size(); ++i)
    add_tri(tlist[i].data());

  end_adding_tris();
}

void TriEdge::logTri ( const uint &qpos ) const
{
  if ( m_tri_versions[qpos ].fnext != INVALID_VALUE )
  {

    _LOG
    (
      "#"<<qpos<<" ("<<
      m_tri_versions[qpos].v<<" "<<
      m_tri_versions[tri_enext ( qpos ) ].v<<" "<<
      m_tri_versions[tri_enext ( tri_enext ( qpos ) ) ].v<<") "<<
      " e ="<<m_tri_versions[qpos].e<<
      " fnext ="<<m_tri_versions[qpos].fnext
    );
  }
  else
  {
    _LOG
    (
      "#"<<qpos<<" ("<<
      m_tri_versions[qpos].v<<" "<<
      m_tri_versions[tri_enext ( qpos ) ].v<<" "<<
      m_tri_versions[tri_enext ( tri_enext ( qpos ) ) ].v<<") "<<
      " e ="<<m_tri_versions[qpos].e
    );
  }
}

void TriEdge::logTriSet ( const uint &trisetstart ) const
{
  unsigned qstart = 6* ( trisetstart/6 );

  for ( int i = 0 ; i < 6 ; i++ )
  {
    uint qpos = qstart + i;
    logTri ( qpos );
  }

  _LOG ( "-------------------------------------" );
}

void TriEdge::init()
{
  m_vert_ct = 0;
  m_edge_ct = 0;
  m_tri_ct = 0;
}

void TriEdge::destroy()
{
  if ( m_vert_ct !=0 )
  {
    delete []m_verts;
  }
  if ( m_edge_ct !=0 )
  {
    delete []m_edges;
  }
  if ( m_tri_ct !=0 )
  {
    delete []m_tris;
    delete []m_tri_versions;
  }
  init();
}

#define CHECK_VALID_Q_VERSION(q) \
  DEBUG_STMT(\
  if ( (q) >= m_tri_ct*6 )\
{\
    _ERROR ( "Tri version index out of range" );\
    _ERROR ( "q         = "<<(q) );\
    _ERROR ( "m_tri_ct = "<<m_tri_ct );\
    exit(1);\
}\
  )\


uint TriEdge::vertIndex ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );
  return m_tri_versions[q].v;
}

uint TriEdge::edgeIndex ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );
  return m_tri_versions[q].e;
}

uint TriEdge::triIndex ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );
  return q/6;
}

uint TriEdge::triFnext ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );

  DEBUG_BEGIN
  if ( !hasFnext ( q ) )
  {
    _ERROR ( "invalid fnext requested" );
    exit ( 1 );
  }
  DEBUG_END

  return m_tri_versions[q].fnext;
}

bool TriEdge::hasFnext ( uint q ) const
{
  return ! ( m_tri_versions[q].fnext == INVALID_VALUE );
}

uint tri_cell_complex_t::get_num_cells_dim (uint dim)
{
  switch(dim)
  {
  case 0: return m_tri_edge->m_vert_ct;
  case 1: return m_tri_edge->m_edge_ct;
  case 2: return m_tri_edge->m_tri_ct;
  }

  throw std::runtime_error("invalid dim specified");
}

uint tri_cell_complex_t::get_cell_dim (cellid_t c) const
{
  if(c < m_tri_edge->m_vert_ct)
    return 0;

  c -= m_tri_edge->m_vert_ct;

  if(c < m_tri_edge->m_edge_ct)
    return 1;

  c -= m_tri_edge->m_edge_ct;

  if(c < m_tri_edge->m_tri_ct)
    return 2;

  throw std::runtime_error("cellid out of range");
}

uint tri_cell_complex_t::get_cell_points (cellid_t  c,cellid_t   *p ) const
{
  if(c < m_tri_edge->m_vert_ct)
  {
    p[0] = c;

    return 1;
  }

  c -= m_tri_edge->m_vert_ct;

  if(c < m_tri_edge->m_edge_ct)
  {
    uint t = m_tri_edge->m_edges[c];

    p[0] = m_tri_edge->vertIndex(t); t = tri_enext(t);
    p[1] = m_tri_edge->vertIndex(t);

    return 2;
  }

  c -= m_tri_edge->m_edge_ct;

  if(c < m_tri_edge->m_tri_ct)
  {
    uint t = m_tri_edge->m_tris[c];

    p[0] = m_tri_edge->vertIndex(t); t = tri_enext(t);
    p[1] = m_tri_edge->vertIndex(t); t = tri_enext(t);
    p[2] = m_tri_edge->vertIndex(t);

    return 3;
  }

  throw std::runtime_error("cellid out of range");
}

uint tri_cell_complex_t::get_cell_facets (cellid_t  c,cellid_t  * f) const
{
  if(c < m_tri_edge->m_vert_ct)
  {
    return 0;
  }

  c -= m_tri_edge->m_vert_ct;

  if(c < m_tri_edge->m_edge_ct)
  {
    uint t = m_tri_edge->m_edges[c];

    f[0] = m_tri_edge->vertIndex(t); t = tri_enext(t);
    f[1] = m_tri_edge->vertIndex(t);

    return 2;
  }

  c -= m_tri_edge->m_edge_ct;

  if(c < m_tri_edge->m_tri_ct)
  {
    uint t = m_tri_edge->m_tris[c];

    f[0] = m_tri_edge->m_vert_ct + m_tri_edge->edgeIndex(t); t = tri_enext(t);
    f[1] = m_tri_edge->m_vert_ct + m_tri_edge->edgeIndex(t); t = tri_enext(t);
    f[2] = m_tri_edge->m_vert_ct + m_tri_edge->edgeIndex(t);

    return 3;
  }

  throw std::runtime_error("cellid out of range");
}

uint tri_cell_complex_t::get_cell_co_facets (cellid_t c ,cellid_t  * cf) const
{

  std::vector<cellid_t> cf_vec;

  if(c < m_tri_edge->m_vert_ct)
  {
    uint tstart = m_tri_edge->m_verts[c];

    uint t = tstart;

    do
    {
      if ( !m_tri_edge->hasFnext ( t ) ) break;

      t = tri_enext ( m_tri_edge->triFnext ( t ) );
    }
    while ( t != tstart );

    uint cf_ct = 0 ;

    tstart = t;

    do
    {
      cf_vec.push_back(m_tri_edge->m_vert_ct + m_tri_edge->edgeIndex(t));

      t = tri_eprev ( t );

      if ( !m_tri_edge->hasFnext ( t ) )
      {
        cf_vec.push_back(m_tri_edge->m_vert_ct + m_tri_edge->edgeIndex(t));

        break;
      }

      t = m_tri_edge->triFnext ( t );
    }
    while ( t != tstart );

    if(cf_vec.size() > 20)
      throw std::runtime_error("max cf size exceeded");

    std::copy(cf_vec.begin(),cf_vec.end(),cf);

    return cf_vec.size();
  }

  c -= m_tri_edge->m_vert_ct;

  if(c < m_tri_edge->m_edge_ct)
  {
    uint cf_ct = 0 ;

    uint tri_id_bias = m_tri_edge->m_edge_ct + m_tri_edge->m_vert_ct;

    uint t = m_tri_edge->m_edges[c];

    cf[cf_ct++] = tri_id_bias + m_tri_edge->triIndex ( t )  ;

    if ( m_tri_edge->hasFnext ( t ) )
    {
      t = m_tri_edge->triFnext ( t );

      cf[cf_ct++] = tri_id_bias + m_tri_edge->triIndex ( t )  ;
    }

    return cf_ct;
  }

  c -= m_tri_edge->m_edge_ct;

  if(c < m_tri_edge->m_tri_ct)
  {
    return 0;
  }

  throw std::runtime_error("cellid out of range");
}

bool tri_cell_complex_t::is_cell_boundry(cellid_t c) const
{
  if(c < m_tri_edge->m_vert_ct)
  {
    uint tstart = m_tri_edge->m_verts[c];

    uint t = tstart;

    do
    {
      if ( !m_tri_edge->hasFnext ( t ) ) return true;

      t = tri_enext ( m_tri_edge->triFnext ( t ) );
    }
    while ( t != tstart );

    return false;
  }

  c -= m_tri_edge->m_vert_ct;

  if(c < m_tri_edge->m_edge_ct)
  {
    return (!m_tri_edge->hasFnext ( m_tri_edge->m_edges[c]) );
  }

  c -= m_tri_edge->m_edge_ct;

  if(c < m_tri_edge->m_tri_ct)
  {
    return false;
  }

  throw std::runtime_error("cellid out of range");
}
