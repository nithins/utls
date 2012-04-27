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
#include <iostream>

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
namespace utls
{
  template <class T> inline std::string to_string (const T& t)
  {std::stringstream ss;ss << t;return ss.str();}

  template <> inline std::string to_string (const std::string& t){return t;}

  template <typename T,typename U> inline std::string to_string (const std::pair<T,U>& t)
  {return to_string<T>(t.first)+":"+to_string<U>(t.second);}

  template<typename Titer>
  typename std::iterator_traits<Titer>::difference_type count(Titer b, Titer e)
  {
    typename std::iterator_traits<Titer>::difference_type val = 0;
    for( ; b != e; ++b) ++val; return val;
  }

  template<typename T>
  inline void set_vec_value(std::vector<T> & vec, int i,const T& v){vec[i] = v;}

  template<typename T>
  inline const T& get_vec_value(const std::vector<T> & vec, int i){return vec[i];}
}

class assertion_error:public std::exception
{

public:
  /** Takes a character string describing the error.  */
  assertion_error(const std::string& );
  virtual ~assertion_error() throw();

  /** Returns a C-style character string describing the general cause of
   *  the current error (the same string passed to the ctor).  */
  virtual const char*
  what() const throw();

  assertion_error& push(const std::string & s);
};

#define two_power(i)   (std::pow(2,(i)))
#define divide_rz(a,b) ((a)/(b))
#define divide_ri(a,b) (((a)+(b)-1)/(b))

std::string __format_ffl(const char *file,const char* func,int line);

#define FILEFUNCLINE __format_ffl(__FILE__,__func__,__LINE__)

#define SVAR(v)         (std::string(#v)+" = "+utls::to_string(v)+" ")
#define SVAR1(v1)       (SVAR(v1))
#define SVAR2(v1,v2)    (SVAR(v1)+" "+SVAR(v2))
#define SVAR3(v1,v2,v3) (SVAR(v1)+" "+SVAR(v2)+" "+SVAR(v2))

#define PUSHVAR(v) push(SVAR(v))

template<typename T>
inline void __ensure(bool c, const char * s,const char * file, const char *func, const int &line)
{
  if(!c)
    throw T(std::string("\n")+__format_ffl(file,func,line)+"\n"+s);
}

#define ensure(c,s)    __ensure<std::runtime_error>(c,s,__FILE__,__func__,__LINE__)
#define ensure_re(c,s) __ensure<std::runtime_error>(c,s,__FILE__,__func__,__LINE__)
#define ensure_oe(c,s) __ensure<std::overflow_error>(c,s,__FILE__,__func__,__LINE__ )
#define ensure_ia(c,s) __ensure<std::invalid_argument>(c,s,__FILE__,__func__,__LINE__)

#ifndef NDEBUG

inline void __assert(bool c, const char * s,const char * file, const char *func, const int &line)
{
  if(!c)
    throw assertion_error(std::string("\n Assertion failure: ")+__format_ffl(file,func,line)+"\n"+s);
}

#define ASSERT(c) __assert((c),#c,__FILE__,__func__,__LINE__)

#else
#define ASSERT(c)
#endif

#define is_in_range(i,b,e) (((b) <= (i)) && ((i) < (e)))

// for some reason this is not in c++ 99 or 03 .. remove if c++0x
namespace std
{
#ifndef __GXX_EXPERIMENTAL_CXX0X__

  template<class In, class Out, class Pred>
  Out copy_if(In first, In last, Out res, Pred p)
  {
    while (first != last)
    {
      if (p(*first))
        *res++ = *first;
      ++first;
    }
    return res;
  }
#endif

  template <class In1,class In2,class Out,class Pred,class BinOp>
  const Out Transform_if(In1 b1,const In1 e1, In2 b2,Out r,const Pred pred,const BinOp binop)
  {
    while (b1 != e1)
    {
      if(pred(*b1,*b2))
        *r++ = binop(*b1,*b2);
      ++b1;++b2;
    }
    return r;
  }

}
#endif
