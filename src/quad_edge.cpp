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


#include <cpputils.h>
#include <logutil.h>

#include <quad_edge.h>

#define INVALID_VALUE 0xffffffff

#define DEBUG_BEGIN if(false) {
#define DEBUG_END }
#define DEBUG_STMT(x) DEBUG_BEGIN x; DEBUG_END;
#define DEBUG_LOG(x) DEBUG_STMT(_LOG(x))

using namespace std;

uint quad_enext ( uint q )
{
  return ( 4* ( q/4 ) + ( q+1 ) %4 );
}

uint quad_eprev ( uint q )
{
  return ( 4* ( q/4 ) + ( q+3 ) %4 );
}

uint quad_sym ( uint q )
{
  return ( q + 7 ) -2* ( q%8 );
}

QuadEdge::QuadEdge()
{
  init();
}

void QuadEdge::setNumVerts ( const uint & vertex_ct )
{
  m_vert_ct = vertex_ct;
  m_verts  = new uint[vertex_ct];
}

void QuadEdge::setNumQuads ( const uint & quad_ct )
{
  m_quad_ct         = quad_ct;
  m_quad_versions   = new quad[ quad_ct*8];
}

typedef map< pair<uint,uint>,uint,unordered_pair_comparator > edge_map_t;
struct qaapkt
{
  uint           quad_pos;
  edge_map_t     edge_map;
  vector<uint>   edge_list;// list of quads that have {v1,v2} = {u,v}

  qaapkt() :quad_pos ( 0 ),edge_map ( unordered_pair_comparator() ) {}
};

void QuadEdge::start_adding_quads()
{
  add_algo_pkt = new qaapkt();
}

void QuadEdge::add_quad ( const uint *v )
{

  vector<uint> &edge_list = ( ( qaapkt* ) add_algo_pkt )->edge_list;
  edge_map_t   &edge_map  = ( ( qaapkt* ) add_algo_pkt )->edge_map;
  uint         &quad_pos  = ( ( qaapkt* ) add_algo_pkt )->quad_pos;

  // each quad has 8 versions

  // the first four versions
  for ( int i = 0 ; i < 4 ; i++ )
  {
    m_quad_versions[ quad_pos + i].v = v[i];
    // each vert will hit many times but that does not matter.. could count degree here
    m_verts[v[i]] = quad_pos + i;
  }

  for ( int i = 0 ; i < 4 ; i++ )
  {
    quad *q1 = &m_quad_versions[ quad_pos + i];
    quad *q2 = &m_quad_versions[ quad_pos + ( i+1 ) %4];

    edge_map_t::iterator it = edge_map.find ( make_pair ( q1->v, q2->v ) );

    if ( it != edge_map.end() )
    {
      q1->fnext = ( m_quad_versions[ ( *it ).second].v == q2->v ) ? ( ( *it ).second ) : ( quad_sym ( ( *it ).second ) );
      m_quad_versions[q1->fnext].fnext = quad_pos + i;
      q1->e = m_quad_versions[q1->fnext].e;
      edge_map.erase ( it );
    }
    else
    {
      q1->e = edge_list.size();
      q1->fnext = INVALID_VALUE;
      edge_list.push_back ( quad_pos + i );
      edge_map.insert ( make_pair ( make_pair ( q1->v,q2->v ), quad_pos + i ) );
    }
  }

  // the next four are just reflections
  for ( int i = 0 ; i < 4 ; i++ )
  {
    uint qpos = quad_pos + 4 + i;
    quad *q = &m_quad_versions[qpos];

    q->v     = m_quad_versions[quad_enext ( quad_sym ( qpos ) ) ].v;
    q->e     = m_quad_versions[quad_sym ( qpos ) ].e;

    if ( m_quad_versions[quad_sym ( qpos ) ].fnext != INVALID_VALUE )
    {
      q->fnext = quad_sym ( m_quad_versions[quad_sym ( qpos ) ].fnext );
      m_quad_versions[q->fnext].fnext = qpos;
    }
    else
    {
      q->fnext = INVALID_VALUE;
    }
  }
  quad_pos+=8;
}

void QuadEdge::end_adding_quads()
{
  vector<uint> &edge_list = ( ( qaapkt* ) add_algo_pkt )->edge_list;
  edge_map_t   &edge_map  = ( ( qaapkt* ) add_algo_pkt )->edge_map;
  //uint         &quad_pos  = ( ( qaapkt* ) add_algo_pkt )->quad_pos;


  m_edge_ct = edge_list.size();
  m_edges = new uint[m_edge_ct];

  for ( uint i = 0 ; i < m_edge_ct;i++ )
  {
    m_edges[i] = edge_list [i];
  }

  m_quads = new uint[m_quad_ct];
  for ( uint i = 0 ; i < m_quad_ct;i++ )
  {
    m_quads[i] = m_quad_ct*8;
  }

  // this is done so that the versions of the quads we look at are cyclically consistant
  // the outer loop will invoke the inner loop once for each connected component
  for ( uint i = 0 ; i < m_quad_ct;i++ )
  {
    if ( m_quads[i] == m_quad_ct*8 )
    {
      queue<uint> quad_queue;

      quad_queue.push ( i*8 );

      while ( quad_queue.size() !=0 )
      {
        uint q = quad_queue.front();
        quad_queue.pop();

        m_quads[q/8] = ( q/8 ) *8 + ( ( q%8 ) /4 ) *4;

        for ( int i = 0 ; i< 4;i++ )
        {
          q = quad_enext ( q );

          uint qfnext = m_quad_versions[q].fnext;

          if ( qfnext != INVALID_VALUE )
          {
            if ( ( m_quads[qfnext/8] == m_quad_ct*8 ) )
            {
              m_quads[qfnext/8] = 0;
              quad_queue.push ( qfnext );
            }
          }
        }
      }
    }
  }



  DEBUG_LOG ( "==================" );
  DEBUG_LOG ( "Quad Versions list" );
  DEBUG_LOG ( "------------------" );
  DEBUG_BEGIN;
  for ( uint i = 0 ; i < m_quad_ct;i++ )
  {
    logQuadSet ( i*8 );
  }
  DEBUG_END;
  DEBUG_LOG ( "==================" );


  m_boundry_quad_ct = edge_map.size();
  m_boundry_quads   = new uint[m_boundry_quad_ct];

  unsigned int bquadno = 0;

  // pick up the boundry quads .. these are the quads that remain unpaired in the edge map
  // remember the outer loop will execute as many times as the no of disconnected cycles
  while ( edge_map.size() != 0 )
  {
    uint bqstart = ( * edge_map.begin() ).second;

    uint bq = bqstart;

    DEBUG_LOG ( "============================================" );
    DEBUG_LOG ( "Edge map contents before boundry computation" );
    DEBUG_LOG ( "--------------------------------------------" );
    DEBUG_BEGIN;
    for ( edge_map_t::iterator it = edge_map.begin() ;it != edge_map.end(); it++ )
    {
      logQuad ( ( *it ).second );
    }
    DEBUG_END;
    DEBUG_LOG ( "--------------------------------------------" );
    DEBUG_LOG ( "Beginning boundry traversal" );
    DEBUG_LOG ( "--------------------------------------------" );

    do
    {
      DEBUG_STMT ( logQuad ( bq ) );

      if ( edge_map.find ( make_pair ( m_quad_versions[bq].v,m_quad_versions[quad_enext ( bq ) ].v ) ) ==edge_map.end() )
      {
        _ERROR ( "did not find this edge in here" );
        exit ( 1 );
      }

      edge_map.erase ( make_pair ( m_quad_versions[bq].v,m_quad_versions[quad_enext ( bq ) ].v ) );

      m_boundry_quads[bquadno++] = bq;

      uint bqnext = quad_enext ( bq );

      while ( m_quad_versions[bqnext].fnext != INVALID_VALUE )
      {
        bqnext = quad_enext ( m_quad_versions[bqnext].fnext );

        if ( bqnext == bq )
        {
          _ERROR ( "Went around the boundry quad but did not find the next boundry edge" );
          exit ( 1 );
        }
      }
      bq = bqnext;
    }
    while ( bq !=bqstart );

    DEBUG_LOG ( "--------------------------------------------" );
    DEBUG_LOG ( "Finished boundry traversal" );
    DEBUG_LOG ( "============================================" );


  }

  delete ( ( qaapkt* ) add_algo_pkt );
}

void QuadEdge::logQuad ( const uint &qpos ) const
{
  if ( m_quad_versions[qpos ].fnext != INVALID_VALUE )
  {

    _LOG
    (
      "#"<<qpos<<" ("<<
      m_quad_versions[qpos].v<<" "<<
      m_quad_versions[quad_enext ( qpos ) ].v<<" "<<
      m_quad_versions[quad_enext ( quad_enext ( qpos ) ) ].v<<" "<<
      m_quad_versions[quad_enext ( quad_enext ( quad_enext ( qpos ) ) ) ].v<<") "<<
      " e ="<<m_quad_versions[qpos].e<<
      " fnext ="<<m_quad_versions[qpos].fnext
    );
  }
  else
  {
    _LOG
    (
      "#"<<qpos<<" ("<<
      m_quad_versions[qpos].v<<" "<<
      m_quad_versions[quad_enext ( qpos ) ].v<<" "<<
      m_quad_versions[quad_enext ( quad_enext ( qpos ) ) ].v<<" "<<
      m_quad_versions[quad_enext ( quad_enext ( quad_enext ( qpos ) ) ) ].v<<") "<<
      " e ="<<m_quad_versions[qpos].e
    );
  }
}

void QuadEdge::logQuadSet ( const uint &quadsetstart ) const
{
  unsigned qstart = 8* ( quadsetstart/8 );

  for ( int i = 0 ; i < 8 ; i++ )
  {
    uint qpos = qstart + i;
    logQuad ( qpos );
  }

  _LOG ( "-------------------------------------" );
}

void QuadEdge::init()
{
  m_vert_ct = 0;
  m_edge_ct = 0;
  m_quad_ct = 0;
  m_boundry_quad_ct = 0;
}

void QuadEdge::destroy()
{
  if ( m_vert_ct !=0 )
  {
    delete []m_verts;
  }
  if ( m_edge_ct !=0 )
  {
    delete []m_edges;
  }
  if ( m_quad_ct !=0 )
  {
    delete []m_quads;
    delete []m_quad_versions;
  }
  if ( m_boundry_quad_ct !=0 )
  {
    delete []m_boundry_quads;
  }
  init();
}

#define CHECK_VALID_Q_VERSION(q) \
  DEBUG_STMT(\
  if ( (q) >= m_quad_ct*8 )\
  {\
    _ERROR ( "Quad version index out of range" );\
    _ERROR ( "q         = "<<(q) );\
    _ERROR ( "m_quad_ct = "<<m_quad_ct );\
    exit(1);\
  }\
  )\
 

uint QuadEdge::vertIndex ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );
  return m_quad_versions[q].v;
}

uint QuadEdge::edgeIndex ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );
  return m_quad_versions[q].e;
}

uint QuadEdge::quadIndex ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );
  return q/8;
}

uint QuadEdge::quadFnext ( uint q ) const
{
  CHECK_VALID_Q_VERSION ( q );

  DEBUG_BEGIN
  if ( !hasFnext ( q ) )
  {
    _ERROR ( "invalid fnext requested" );
    exit ( 1 );
  }
  DEBUG_END

  return m_quad_versions[q].fnext;
}

bool QuadEdge::hasFnext ( uint q ) const
{
  return ! ( m_quad_versions[q].fnext == INVALID_VALUE );
}

