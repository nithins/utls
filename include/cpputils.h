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

/*===========================================================================*/
/* Linear Algebra utilities
/*---------------------------------------------------------------------------*/

// This section may be redefined in other projects.
#ifndef __UTL_H_INCLUDED
#include <Eigen/Dense>
#include <math.h>

namespace la
{

  template<typename T,unsigned int N>
  struct vec_t
  {
    typedef Eigen::Matrix<T,1,N> type;
  };

  typedef vec_t<double,2>::type dvec2_t;
  typedef vec_t<double,3>::type dvec3_t;
  typedef vec_t<double,4>::type dvec4_t;

  typedef vec_t<float,2>::type fvec2_t;
  typedef vec_t<float,3>::type fvec3_t;
  typedef vec_t<float,4>::type fvec4_t;

  typedef vec_t<int,2>::type ivec2_t;
  typedef vec_t<int,3>::type ivec3_t;
  typedef vec_t<int,4>::type ivec4_t;

  typedef vec_t<unsigned int,2>::type uivec2_t;
  typedef vec_t<unsigned int,3>::type uivec3_t;
  typedef vec_t<unsigned int,4>::type uivec4_t;

  template<typename T>
  inline typename vec_t<T,2>::type
  make_vec(const T&a , const T&b )
  {
    typename vec_t<T,2>::type v;
    v[0] = a; v[1] = b; return v;
  }

  template<typename T>
  inline typename vec_t<T,3>::type
  make_vec(const T&a , const T&b ,const T&c)
  {
    typename vec_t<T,3>::type v;
    v[0] = a; v[1] = b; v[2] = c; return v;
  }

  template<typename T>
  inline typename vec_t<T,4>::type
  make_vec(const T&a , const T&b ,const T&c,const T&d)
  {
    typename vec_t<T,4>::type v;
    v[0] = a; v[1] = b; v[2] = c; v[3] = d; return v;
  }

  template<typename T>
  inline typename vec_t<T,4>::type
  make_vec(const typename vec_t<T,3>::type &u, const T&d)
  {
    typename vec_t<T,4>::type v;
    v[0] = u[0]; v[1] = u[1]; v[2] = u[2]; v[3] = d; return v;
  }

  template<typename T>
  inline typename vec_t<T,4>::type
  make_vec(const T&a, const typename vec_t<T,3>::type &u)
  {
    typename vec_t<T,4>::type v;
    v[0] = a; v[1] = u[0]; v[2] = u[1]; v[3] = u[2]; return v;
  }


  template<typename T,unsigned int N>
  inline double tri_area(const typename vec_t<T,N>::type &p,
                    const typename vec_t<T,N>::type &q,
                    const typename vec_t<T,N>::type &r)
  {
    double a = (p-q).norm();
    double b = (q-r).norm();
    double c = (r-p).norm();

    double s = (a+b+c)/2.0;

    double area2  = s*(s-a)*(s-b)*(s-c);

    double area =  sqrt(area2);

    return area;
  }
}

#endif

/*===========================================================================*/











/*---------------------------------------------------------------------------*/

#include <vector>
#include <string>
#include <sstream>

namespace utls {

/*---------------------------------------------------------------------------*/

///  \brief A generic to string impl that uses the << operator of type T
template <typename T>
inline std::string to_string (const T& v)
{std::stringstream ss; ss << v ; return ss.str();}

/*---------------------------------------------------------------------------*/

template <typename T1,typename T2>
inline std::string to_string (const std::pair<T1,T2> &p)
{std::stringstream ss; ss <<p.first <<","<< p.second ; return ss.str();}

/*---------------------------------------------------------------------------*/

template <class iter_t>
void argsort(iter_t b, iter_t e, std::vector<size_t>& idxs);

/*---------------------------------------------------------------------------*/

template <class iter_t>
void rearrange(iter_t b, iter_t e,const std::vector<size_t>& idxs);

/*---------------------------------------------------------------------------*/

/// \brief strip front and back whitespaces in given string
void trim(std::string &s);

/*---------------------------------------------------------------------------*/

/// \brief Converts a given string to the template type
template <class T> inline T from_string(std::string s);

/*---------------------------------------------------------------------------*/

///  \brief Converts a given string to the template types
template <typename T1,typename T2>
inline void from_string(std::string s,T1& t1,T2& t2)
{std::stringstream ss(s);ss >> t1 >> t2;}

/*---------------------------------------------------------------------------*/

///  \brief Converts a given string to the template types
template <typename T1,typename T2,typename T3>
inline void from_string(std::string s,T1& t1,T2& t2,T3& t3)
{std::stringstream ss(s);ss >> t1 >> t2 >> t3;}

/*---------------------------------------------------------------------------*/

///  \brief Converts a given string to the template types
template <typename T1,typename T2,typename T3,typename T4>
inline void from_string(std::string s,T1& t1,T2& t2,T3& t3,T4& t4)
{std::stringstream ss(s);ss >> t1 >> t2 >> t3 >> t4;}

/*---------------------------------------------------------------------------*/

}// namespace utl
/*===========================================================================*/






/*===========================================================================*/
/* Misc utility classes
/*---------------------------------------------------------------------------*/

#include <boost/iterator/iterator_facade.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/thread.hpp>

namespace utls {

/*---------------------------------------------------------------------------*/


/**
  \brief A class to iterate over non-comment lines of a file

  \note All parts of a line that succeed the comment char ('#' default) are
  assumed to be a comment and are stripped

  \note Sample usage
  for(file_line_iterator lgen("file.txt"),lend; lgen != lend; )
    cout << *lgen++;

**/

class file_line_iterator
    : public boost::iterator_facade<
    file_line_iterator
    , std::string const
    , boost::forward_traversal_tag
    >
{
public:
  file_line_iterator(const char * f,char c_char='#');
  file_line_iterator(){}

private:
  friend class boost::iterator_core_access;
  void increment();
  bool equal(file_line_iterator const& other) const;
  const std::string &dereference() const;

private:
  boost::shared_ptr<std::ifstream> is;
  std::string                      value;
  char                             c_char;
};

/*---------------------------------------------------------------------------*/

/** \brief Simple stopwatch timer (to measure wall clock time NOT cpu time) **/
class timer
{
 public:
  timer()
  {restart();}

  inline void   restart()
  { _start_time = boost::posix_time::microsec_clock::local_time(); }

  inline double elapsed() const
  {
    boost::posix_time::time_duration td =
        boost::posix_time::microsec_clock::local_time() - _start_time;

    return double(td.total_milliseconds())/1000;
  }

 private:
  boost::posix_time::ptime _start_time;
}; // timer

/*---------------------------------------------------------------------------*/

#ifndef UTL_LOG_LEVEL
#define UTL_LOG_LEVEL 2
#endif

/** \brief A simple multi-threaded logger                                   **/
class logger
{
 public:

  enum severity_level {trace,debug,info,warning,error,fatal};

  inline bool isOpen(severity_level severity)
  {return severity >= UTL_LOG_LEVEL;}

  inline void push_ts(const std::string & log)
  {
    std::string tstr = boost::posix_time::to_simple_string
        (boost::posix_time::microsec_clock::local_time());

    boost::mutex::scoped_lock  lock(s_mutex);
    std::clog << "[" <<tstr <<"] " << log << std::endl;
  }

  inline void push(const std::string & log)
  {
    boost::mutex::scoped_lock  lock(s_mutex);
    std::clog << log;
  }

  static inline logger& get() {return s_logger;}

private:

  static boost::mutex s_mutex;
  static logger       s_logger;
}; // logger

namespace detail
{
/** \brief a small extension of std::pair where only first is initialized   **/
template<class _T1, class _T2> struct pair:public std::pair<_T1,_T2>
{pair(const _T1 &t1){std::pair<_T1,_T2>::first = t1;}};
}//namespace detail

}// namespace utl

/*===========================================================================*/



/*===========================================================================*/
/* Binary read and write functions
/*---------------------------------------------------------------------------*/

namespace utl {

/*---------------------------------------------------------------------------*/

template<typename T>
inline void bin_write(std::ostream &os, const T &v)
{os.write((const char*)(const void*)&v,sizeof(T));}

/*---------------------------------------------------------------------------*/

template<typename T>
inline void bin_read(std::istream &is, const T &v)
{is.read((char*)(void*)&v,sizeof(T));}

/*---------------------------------------------------------------------------*/

template<typename T>
inline void bin_read_raw(std::istream &is, T * v, size_t n)
{is.read((char*)(void*)v,n*sizeof(T));}

/*---------------------------------------------------------------------------*/

template<typename T>
inline void bin_write_raw(std::ostream &os, T * v, size_t n)
{os.write((const char*)(const void*)v,n*sizeof(T));}

/*---------------------------------------------------------------------------*/

template<typename T>
inline void bin_read_vec(std::istream &is, std::vector<T> &v)
{
  size_t n;
  bin_read(is,n);
  v.resize(n);
  is.read((char*)(void*)v.data(),v.size()*sizeof(T));
}

/*---------------------------------------------------------------------------*/

template<typename T>
inline void bin_write_vec(std::ostream &os, const std::vector<T> &v)
{
  bin_write<size_t>(os,v.size());
  os.write((const char*)(const void*)v.data(),v.size()*sizeof(T));
}

/*---------------------------------------------------------------------------*/

}
/*===========================================================================*/


/*===========================================================================*/
/* Macro definitions
/*---------------------------------------------------------------------------*/

#define is_in_range(i,b,e) (((b) <= (i)) && ((i) < (e)))
#define two_power(i)   (std::pow(2,(i)))
#define divide_rz(a,b) ((a)/(b))
#define divide_ri(a,b) (((a)+(b)-1)/(b))

/*---------------------------------------------------------------------------*/

#define LOG(lev)  \
  if(utls::logger::get().isOpen(utls::logger::lev))\
  for(utls::detail::pair<bool,std::stringstream> __utl_lm_v__(true); \
      __utl_lm_v__.first ;__utl_lm_v__.first=false,\
  utls::logger::get().push(__utl_lm_v__.second.str())) __utl_lm_v__.second

/*---------------------------------------------------------------------------*/

#define WLOG  \
  if(utls::logger::get().isOpen(utls::logger::warning))\
  for(utls::detail::pair<bool,std::stringstream> __utl_lm_v__(true); \
      __utl_lm_v__.first ;__utl_lm_v__.first=false,\
  utls::logger::get().push_ts(__utl_lm_v__.second.str())) \
  __utl_lm_v__.second \
  <<utls::detail::__classFunction__(__PRETTY_FUNCTION__) << " "

/*---------------------------------------------------------------------------*/

#define ILOG  \
  if(utls::logger::get().isOpen(utls::logger::info))\
  for(utls::detail::pair<bool,std::stringstream> __utl_lm_v__(true); \
      __utl_lm_v__.first ;__utl_lm_v__.first=false,\
  utls::logger::get().push_ts(__utl_lm_v__.second.str())) \
  __utl_lm_v__.second \
  <<utls::detail::__classFunction__(__PRETTY_FUNCTION__) << " "

/*---------------------------------------------------------------------------*/

#define DLOG  \
  if(utls::logger::get().isOpen(utls::logger::debug))\
  for(utls::detail::pair<bool,std::stringstream> __utl_lm_v__(true); \
      __utl_lm_v__.first ;__utl_lm_v__.first=false,\
  utls::logger::get().push_ts(__utl_lm_v__.second.str())) \
  __utl_lm_v__.second \
  <<utls::detail::__classFunction__(__PRETTY_FUNCTION__) << " "

/*---------------------------------------------------------------------------*/

#define TLOG  \
  if(utls::logger::get().isOpen(utls::logger::trace))\
  for(utls::detail::pair<bool,std::stringstream> __utl_lm_v__(true); \
      __utl_lm_v__.first ;__utl_lm_v__.first=false,\
  utls::logger::get().push_ts(__utl_lm_v__.second.str())) \
  __utl_lm_v__.second << utls::detail::__trace_indenter_t__::get_indent()\
  <<utls::detail::__classFunction__(__PRETTY_FUNCTION__) << " "


#define TLOG_INDENT utls::detail::__trace_indenter_t__ __trace_indenter__; \

/*---------------------------------------------------------------------------*/

#define LOG_TS(lev)  \
  if(utls::logger::get().isOpen(utls::logger::lev))\
  for(utls::detail::pair<bool,std::stringstream> __utl_lm_v__(true); \
      __utl_lm_v__.first ;__utl_lm_v__.first=false,\
  utls::logger::get().push_ts(__utl_lm_v__.second.str())) __utl_lm_v__.second

/*---------------------------------------------------------------------------*/

#define SVAR(VAR) #VAR << " = "<< (VAR) << " "

/*---------------------------------------------------------------------------*/

#ifndef ASSERT
#ifndef NDEBUG
#define ASSERT(cond) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to assert condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
  ;throw std::runtime_error(ss.str());}
#else  //ifndef NDEBUG
#define ASSERT(cond)
#endif // ifndef NDEBUG
#endif // ifndef ASSERT

/*---------------------------------------------------------------------------*/

#ifndef ASSERTV
#ifndef NDEBUG
#define ASSERTV(cond,var1) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to assert condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    << #var1 <<" = "<< (var1) << "\n"\
  ;throw std::runtime_error(ss.str());}
#else  //ifndef NDEBUG
#define ASSERTV(cond,var1)
#endif // ifndef NDEBUG
#endif // ifndef ASSERTV

/*---------------------------------------------------------------------------*/

#ifndef ASSERTV2
#ifndef NDEBUG
#define ASSERTV2(cond,var1,var2) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to assert condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    << #var1 <<" = "<< (var1) << "\n"\
    << #var2 <<" = "<< (var2) << "\n"\
  ;throw std::runtime_error(ss.str());}
#else  //ifndef NDEBUG
#define ASSERTV2(cond,var1,var2)
#endif // ifndef NDEBUG
#endif // ifndef ASSERTV2

/*---------------------------------------------------------------------------*/

#ifndef ASSERTV3
#ifndef NDEBUG
#define ASSERTV3(cond,var1,var2,var3) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to assert condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    << #var1 <<" = "<< (var1) << "\n"\
    << #var2 <<" = "<< (var2) << "\n"\
    << #var3 <<" = "<< (var3) << "\n"\
  ;throw std::runtime_error(ss.str());}
#else  //ifndef NDEBUG
#define ASSERTV3(cond,var1,var2)
#endif // ifndef NDEBUG
#endif // ifndef ASSERTV2

/*---------------------------------------------------------------------------*/

#ifndef ASSERTS
#ifndef NDEBUG
#define ASSERTS(cond)\
  if(!(cond)) \
  for(std::stringstream ss ; true ; throw std::runtime_error(ss.str())) \
  ss<<"Failed to ensure condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "
#else  //ifndef NDEBUG
#define ASSERTS(cond)\
if(false) \
std::stringstream()
#endif // ifndef NDEBUG
#endif // ifndef ASSERT


/*---------------------------------------------------------------------------*/

#define ENSURE(cond,mesg) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to ensure condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    <<"Message : " << #mesg << "\n"\
  ;throw std::runtime_error(ss.str());}

/*---------------------------------------------------------------------------*/

#define ENSUREV(cond,mesg,var1) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to ensure condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    <<"Message : " << #mesg << "\n"\
    << #var1 <<" = "<< (var1) << "\n"\
  ;throw std::runtime_error(ss.str());}

/*---------------------------------------------------------------------------*/

#define ENSUREV2(cond,mesg,var1,var2) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to ensure condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    <<"Message : " << #mesg << "\n"\
    << #var1 <<" = "<< (var1) << "\n"\
    << #var2 <<" = "<< (var2) << "\n";\
  ;throw std::runtime_error(ss.str());}

/*---------------------------------------------------------------------------*/

#define ENSUREV3(cond,mesg,var1,var2,var3) if (!(cond))\
{ std::stringstream ss; \
  ss<<"Failed to ensure condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    <<"Message : " << #mesg << "\n"\
    << #var1 <<" = "<< (var1) << "\n"\
    << #var2 <<" = "<< (var2) << "\n"\
    << #var3 <<" = "<< (var3) << "\n";\
  ;throw std::runtime_error(ss.str());}

/*---------------------------------------------------------------------------*/

#define ENSURES(cond) \
  if(!(cond)) \
  for(std::stringstream ss ; true ; throw std::runtime_error(ss.str())) \
  ss<<"Failed to ensure condition " << #cond <<"\n" \
    <<"at ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "

/*---------------------------------------------------------------------------*/

#define BTRACE_ERROR(func) \
try { func ; } catch (const std::exception &e) {\
  if(utls::logger::get().isOpen(utls::logger::error)){\
    std::stringstream ss; ss \
    <<"From: ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n ";\
    utls::logger::get().push(ss.str());}\
  throw;}

/*---------------------------------------------------------------------------*/

#define CHECK_ERRORV1(func,var1) \
try { func ; } catch (const std::exception &e) {\
  if(utls::logger::get().isOpen(utls::logger::error)){\
    std::stringstream ss; ss \
    <<"From: ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    << #var1 <<" = "<< (var1) << "\n";\
    utls::logger::get().push(ss.str());}\
  throw;}

/*---------------------------------------------------------------------------*/

#define CHECK_ERRORV2(func,var1,var2) \
try { func ; } catch (const std::exception &e) {\
  if(utls::logger::get().isOpen(utls::logger::error)){\
    std::stringstream ss; ss \
    <<"From: ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    << #var1 <<" = "<< (var1) << "\n"\
    << #var2 <<" = "<< (var2) << "\n";\
    utls::logger::get().push(ss.str());}\
  throw;}

/*---------------------------------------------------------------------------*/

#define CHECK_ERRORV3(func,var1,var2,var3) \
try { func ; } catch (const std::exception &e) {\
  if(utls::logger::get().isOpen(utls::logger::error)){\
    std::stringstream ss; ss \
    <<"From: ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    << #var1 <<" = "<< (var1) << "\n"\
    << #var2 <<" = "<< (var2) << "\n"\
    << #var3 <<" = "<< (var3) << "\n";\
    utls::logger::get().push(ss.str());}\
  throw;}

/*---------------------------------------------------------------------------*/

#define CHECK_ERRORS(func,strm) \
try { func ; } catch (const std::exception &e) {\
  if(utls::logger::get().isOpen(utls::logger::error)){\
    std::stringstream ss; ss \
    <<"From: ("<<__FILE__<<","<<__func__<<","<<__LINE__<<") \n "\
    strm <<std::endl;\
    utls::logger::get().push(ss.str());}\
  throw;}

/*---------------------------------------------------------------------------*/

namespace utls{ namespace detail{

std::string __classFunction__(const std::string& prettyFunction);

class __trace_indenter_t__
{
  static int   s_indent;
public:
  __trace_indenter_t__()   {++s_indent;}
  ~__trace_indenter_t__()  {--s_indent;}
  static std::string get_indent();
};

}}


/*===========================================================================*/



#endif
