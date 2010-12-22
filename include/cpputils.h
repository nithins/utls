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

#include <n_vector.h>

#include <boost/array.hpp>
#include <boost/any.hpp>
#include <boost/function.hpp>

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


class configurable_t
{
public:

  enum eFieldType {EFT_DATA_RO,EFT_DATA_RW,EFT_ACTION};

  typedef n_vector_t<int,2> data_index_t;
  typedef boost::function<void(void)> action_callback_t;

  virtual data_index_t dim() = 0 ;

  virtual bool exchange_field(const data_index_t &,boost::any &)  = 0;

  virtual eFieldType exchange_header(const int &,boost::any &) = 0;

  static bool s_exchange_action
      (const action_callback_t &p_val,boost::any &c_val)
  {
    if(c_val.empty())
      c_val = boost::any(p_val);
    else
      throw std::logic_error("action should be called when interacted with");

    return false;
  }

  template <typename T>
      static bool s_exchange_data_rw(T &p_val,boost::any &c_val)
  {
    bool ret = false;

    if(c_val.empty())
      c_val = boost::any(p_val);
    else
    {
      ret   = (p_val != boost::any_cast<T>(c_val));
      p_val = boost::any_cast<T>(c_val);
    }
    return ret;
  }

  template <typename T>
      static bool s_exchange_data_ro(const T &p_val,boost::any &c_val)
  {
    if(c_val.empty())
      c_val = boost::any(p_val);
    else
      throw std::logic_error("read only property cannot write");

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

// for some reason this is not in c++ 99 or 03 .. remove if c++0x
namespace std
{
  template<class In, class Out, class Pred>
  Out copy_if(In first, In last, Out res, Pred p)
  {
          while (first != last) {
                  if (p(*first)) *res++ = *first;
                  ++first;
          }
          return res;
  }
}




#endif
