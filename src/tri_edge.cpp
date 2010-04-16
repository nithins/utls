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

void TriEdge::setNumVerts ( const uint & vertex_ct )
{
  m_vert_ct = vertex_ct;
  m_verts  = new uint[vertex_ct];
  m_vert_degree = new uint[vertex_ct];
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

  memset ( m_vert_degree,0,sizeof ( uint ) *m_vert_ct );
}

void TriEdge::add_tri ( const uint *v )
{

  vector<uint> &edge_list = ( ( qaapkt* ) add_algo_pkt )->edge_list;
  edge_map_t   &edge_map  = ( ( qaapkt* ) add_algo_pkt )->edge_map;
  uint         &tri_pos  = ( ( qaapkt* ) add_algo_pkt )->tri_pos;

  // each tri has 8 versions

  // the first four versions
  for ( int i = 0 ; i < 3 ; i++ )
  {
    m_tri_versions[ tri_pos + i].v = v[i];
    // each vert will hit many times but that does not matter.. could count degree here
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

      ++m_vert_degree[q1->v];
      ++m_vert_degree[q2->v];
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



  DEBUG_LOG ( "==================" );
  DEBUG_LOG ( "Tri Versions list" );
  DEBUG_LOG ( "------------------" );
  DEBUG_BEGIN;
  for ( uint i = 0 ; i < m_tri_ct;i++ )
  {
    logTriSet ( i*6 );
  }
  DEBUG_END;
  DEBUG_LOG ( "==================" );


  m_boundry_tri_ct = edge_map.size();
  m_boundry_tris   = new uint[m_boundry_tri_ct];

  unsigned int btrino = 0;

  // pick up the boundry tris .. these are the tris that remain unpaired in the edge map
  // remember the outer loop will execute as many times as the no of disconnected cycles

  m_boundry_cycle_ct = 0;

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
      logTri ( ( *it ).second );
    }
    DEBUG_END;
    DEBUG_LOG ( "--------------------------------------------" );
    DEBUG_LOG ( "Beginning boundry traversal" );
    DEBUG_LOG ( "--------------------------------------------" );

    ++m_boundry_cycle_ct;

    do
    {
      DEBUG_STMT ( logTri ( bq ) );

      if ( edge_map.find ( make_pair ( m_tri_versions[bq].v,m_tri_versions[tri_enext ( bq ) ].v ) ) ==edge_map.end() )
      {
        _ERROR ( "did not find this edge in here" );
        exit ( 1 );
      }

      edge_map.erase ( make_pair ( m_tri_versions[bq].v,m_tri_versions[tri_enext ( bq ) ].v ) );

      m_boundry_tris[btrino++] = bq;

      uint bqnext = tri_enext ( bq );

      while ( m_tri_versions[bqnext].fnext != INVALID_VALUE )
      {
        bqnext = tri_enext ( m_tri_versions[bqnext].fnext );

        if ( bqnext == bq )
        {
          _ERROR ( "Went around the boundry tri but did not find the next boundry edge" );
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

  m_max_vert_degree = *max_element ( m_vert_degree,m_vert_degree+m_vert_ct );

  delete ( ( qaapkt* ) add_algo_pkt );
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
  m_boundry_tri_ct = 0;
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
  if ( m_boundry_tri_ct !=0 )
  {
    delete []m_boundry_tris;
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



