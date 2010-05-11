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

#include <string>
#include <sstream>
#include <ostream>
#include <vector>

#include <boost/array.hpp>
#include <boost/any.hpp>

typedef unsigned int  uint;
typedef unsigned char uchar;

struct unordered_pair_comparator
{


  bool operator() ( const std::pair<uint, uint> &p1,
                    const std::pair<uint, uint> &p2 ) const;
};

struct ordered_pair_comparator
{
  bool operator() ( const std::pair<uint, uint> &p1,
                    const std::pair<uint, uint> &p2 ) const;
};

template<typename data_t>
void delete_ftor ( data_t * ptr )
{
  delete ptr;
}

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

    template<typename OT>
    n_tuple_t(const boost::array<OT,N> &o)
    {
      for(uint i = 0 ; i < N ; ++i)
        (*this)[i] = o[i];
    }

    n_tuple_t()
    {
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

    std::string to_string()
    {
      std::stringstream ss;

      ((std::ostream&)ss)<<(*this);

      return ss.str();
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

    template<typename OT>
    two_tuple_t(const boost::array<OT,2> &o):
        n_tuple_t<tup_type_t, 2,ordered>(o){}

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

    template<typename OT>
    three_tuple_t(const boost::array<OT,3> &o):
        n_tuple_t<tup_type_t, 3,ordered>(o){}

};

class configurable_t
{
public:

  typedef two_tuple_t<int> data_index_t;

  virtual int         rows()    = 0 ;

  virtual int         columns() = 0 ;

  virtual bool        exchange_data(const data_index_t &,boost::any &) = 0;

  virtual std::string get_header(int i)
  {
    std::stringstream ss;
    ss<<i;
    return ss.str();
  }

  template <typename T>
      static bool s_exchange_rw(T &p_val,boost::any &c_val)
  {
    bool ret = true;

    if(c_val.empty())
    {
      c_val = boost::any(p_val);
    }
    else
    {
      ret   = (p_val != boost::any_cast<T>(c_val));
      p_val = boost::any_cast<T>(c_val);
    }
    return ret;
  }

  template <typename T>
      static bool s_exchange_ro(const T &p_val,boost::any &c_val)
  {
    if(c_val.empty())
    {
      c_val = boost::any(p_val);
    }
    else
    {
      throw std::logic_error("read only property cannot write");
    }

    return false;
  }
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
