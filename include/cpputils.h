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

#ifndef __CPPUTILS_H_INCLUDED__
#define __CPPUTILS_H_INCLUDED__

#include <iterator>
#include <string>
#include <ostream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include <boost/array.hpp>

struct uint_iterator
{
  typedef unsigned int  value_type;
  typedef unsigned int* pointer;
  typedef unsigned int& reference;
  typedef int difference_type;
  typedef std::forward_iterator_tag iterator_category;

  unsigned int m_pos;

  uint_iterator ( unsigned int _pos )
      : m_pos ( _pos )
  {
  }

  unsigned int operator*()
  {
    return m_pos;
  }

  uint_iterator operator++()
  {
    m_pos++;
    return *this;
  }

  uint_iterator operator++ ( int )
  {
    m_pos++;
    return uint_iterator ( m_pos -1 );
  }

  bool operator== ( const uint_iterator &rhs )
  {
    return ( m_pos == rhs.m_pos );
  }

  bool operator!= ( const uint_iterator &rhs )
  {
    return ! ( *this == rhs );
  }

  static std::pair<uint_iterator, uint_iterator> iterators ( unsigned int s, unsigned int e )
  {
    return std::make_pair ( uint_iterator ( s ), uint_iterator ( e ) );
  }
};


struct unordered_pair_comparator
{
  typedef unsigned int uint;

  bool operator() ( const std::pair<uint, uint> &p1, const std::pair<uint, uint> &p2 ) const
  {
    if ( std::min ( p1.first, p1.second ) == std::min ( p2.first, p2.second ) )
      return std::max ( p1.first, p1.second ) < std::max ( p2.first, p2.second );
    else
      return std::min ( p1.first, p1.second ) < std::min ( p2.first, p2.second );
  }
};

struct ordered_pair_comparator
{
  typedef unsigned int uint;

  bool operator() ( const std::pair<uint, uint> &p1, const std::pair<uint, uint> &p2 ) const
  {
    if ( p1.first == p2.first )
      return ( p1.second < p2.second );
    else
      return ( p1.first < p2.first );
  }
};

template <typename list_t, typename ind_t>
static bool compareListItems ( list_t * list, ind_t i, ind_t j )
{
  if ( list[i] != list[j] )
    return list[i] < list[j];
  else
    return i < j;
}

template <class InputIterator1, class InputIterator2, class AddToSet_ftor_t>
void set_intersection_ftor ( InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2, InputIterator2 last2,
                             AddToSet_ftor_t addToSet_ftor )
{
  while ( first1 != last1 && first2 != last2 )
  {
    if ( *first1 < *first2 )
      ++first1;
    else
      if ( *first2 < *first1 )
        ++first2;
      else
      {
        addToSet_ftor ( *first1 );
        first1++;
        first2++;
      }
  }
}

template<typename data_t>
void delete_ftor ( data_t * ptr )
{
  delete ptr;
}

template<typename num_t>

struct seq_num_gen_ftor
{
  num_t current;
  num_t inc;

  seq_num_gen_ftor() : current ( 0 ), inc ( 1 ) {}

  seq_num_gen_ftor ( num_t _inc ) : current ( 0 ), inc ( _inc ) {}

  seq_num_gen_ftor ( num_t _inc, num_t _start ) : current ( _start ), inc ( _inc ) {}

  int operator() ()
  {
    num_t ret_num = current;
    current += inc;
    return ret_num;
  }

};

class genericException
{
    std::string m_str;

  public:

    genericException ( std::string str ) ;
    std::string str() const;
    friend std::ostream &operator<< ( std::ostream &stream, const genericException &e );
};

template <typename num_t>

class num_generator_t
{
    num_t incr;
    num_t cur_val;

  public:

    num_generator_t ( num_t _incr = 1, num_t start = 0 ) :
        incr ( _incr ), cur_val ( start ) {}

    num_t operator() ()
    {
      num_t retval = cur_val;
      cur_val += incr;
      return retval;
    }
};


template <typename T,typename ST>
T  shift_right(const T & t,const ST &st)
{
  return t>>st;
}

template<typename T, std::size_t N,bool O= true>
class n_tuple_t: public boost::array<T,N>
{

  public:

    typedef typename boost::array<T,N>              base_t;

    typedef typename base_t::value_type             value_type;
    typedef typename base_t::iterator               iterator;
    typedef typename base_t::const_iterator         const_iterator;
    typedef typename base_t::reference              reference;
    typedef typename base_t::const_reference        const_reference;
    typedef typename base_t::size_type              size_type;
    typedef typename base_t::difference_type        difference_type;
    typedef typename base_t::reverse_iterator       reverse_iterator;
    typedef typename base_t::const_reverse_iterator const_reverse_iterator;

    friend std::ostream &operator<< ( std::ostream &stream, const n_tuple_t &e )
    {
      stream<<"(";
      if(N > 0)
      {
        stream<<e.elems[0];
        for(unsigned int i = 1 ; i < N ; ++i)
        {
          stream<<","<<e.elems[i];
        }
      }
      stream<<")";

      return stream;
    }

    friend std::istream &operator>>( std::istream &stream, n_tuple_t &e )
    {
      char comma,bracket;
      stream>>bracket;
      if(N > 0)
      {
        stream>>e.elems[0];
        for(unsigned int i = 1 ; i < N ; ++i)
        {
          stream>>comma;
          stream>>e.elems[i];
        }
      }
      stream>>bracket;

      return stream;
    }
};

template<class T, std::size_t N>
bool operator< (const n_tuple_t<T,N,false>& x, const n_tuple_t<T,N,false>& y)
{
  T ex[N], ey[N];

  std::copy ( x.begin(), x.end(), ex );
  std::copy ( y.begin(), y.end(), ey );

  std::sort ( ex, ex + N );
  std::sort ( ey, ey + N );

  return ( std::lexicographical_compare ( ex, ex + N, ey, ey + N ) );
}


template<class T, std::size_t N>
bool operator== (const n_tuple_t<T,N,false>& x, const n_tuple_t<T,N,false>& y)
{
  T ex[N], ey[N];

  std::copy ( x.begin(), x.end(), ex );
  std::copy ( y.begin(), y.end(), ey );

  std::sort ( ex, ex + N );
  std::sort ( ey, ey + N );

  return std::equal(ex, ex+N, ey);
}


template<typename tup_type_t,bool ordered = true>
class two_tuple_t: public n_tuple_t<tup_type_t, 2,ordered>
{

  public:
    two_tuple_t ( const tup_type_t &e1, const tup_type_t &e2 )
    {
      (*this)[0] = e1;
      (*this)[1] = e2;
    }

    two_tuple_t ():n_tuple_t<tup_type_t,2,ordered>(){}
};

template<typename tup_type_t,bool ordered = true>
class three_tuple_t: public n_tuple_t<tup_type_t, 3,ordered>
{

  public:
    three_tuple_t
    ( const tup_type_t &e1,
      const tup_type_t &e2,
      const tup_type_t &e3 )
    {
      (*this)[0] = e1;
      (*this)[1] = e2;
      (*this)[2] = e3;
    }

    three_tuple_t():n_tuple_t<tup_type_t,3,ordered>(){}
};

std::string stripLineComments ( const std::string& line, const char& comment_char = '#' );

std::string stripLeadingWS ( const std::string& line );

std::string stripTrailingWS ( const std::string& line );

std::string stripWS ( const std::string &line );

void tokenize_string ( const std::string& , std::vector<std::string>& ,
                       const std::string& delimiters = " " );

/**
 * Splits the given string given on the given string delimiter. ( delimter string not included)
 * @param line_str
 * @param tokens
 * @param split_str
 */
void split_string ( const std::string & line_str,
                    std::vector<std::string> & tokens,
                    const std::string & split_str );

template<typename type1_t,typename type2_t>
std::pair<type2_t,type1_t> reverse_pair(const std::pair<type1_t,type2_t> &p)
{
  return std::pair<type2_t,type1_t>(p.second,p.first);
}


#endif
