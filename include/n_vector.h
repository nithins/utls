#ifndef N_VECTOR_H_INCLUDED
#define N_VECTOR_H_INCLUDED

#include <string>
#include <sstream>
#include <ostream>

#include <boost/array.hpp>

template<typename T, std::size_t N,bool O= true>
class n_vector_t: public boost::array<T,N>
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

    friend std::ostream &operator<< ( std::ostream &stream, const n_vector_t &e )
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

    n_vector_t()
    {
    }

    template<typename OT,bool OO>
    n_vector_t( const n_vector_t<OT,N,OO> &o)
    {
      for(size_t i = 0 ; i < N;++i)
        (*this)[i] = o[i];
    }

    template<typename OT1,typename OT2>
    n_vector_t( const OT1 &e1, const OT2 &e2 )
    {
      (*this)[0] = e1;
      (*this)[1] = e2;
    }

    template<typename OT1,typename OT2,typename OT3>
    n_vector_t( const OT1 &e1,const OT2 &e2,const OT3 &e3 )
    {
      (*this)[0] = e1;
      (*this)[1] = e2;
      (*this)[2] = e3;
    }

    template<typename OT>
    static n_vector_t s_assign( const OT &o)
    {
      n_vector_t r;

      r.assign((T)o);

      return r;
    }


    inline n_vector_t operator/(const T & s) const
    {
      n_vector_t ret;

      for(size_t i = 0 ; i < N;++i )
        ret[i] = (*this)[i]/s;

      return ret;
    }

    inline n_vector_t operator*(const T & s) const
    {
      n_vector_t ret;

      for(size_t i = 0 ; i < N;++i )
        ret [i]= (*this)[i] * s;

      return ret;
    }

    inline n_vector_t operator-(const n_vector_t & v) const
    {
      n_vector_t ret;

      for(size_t i = 0 ; i < N;++i )
        ret[i] = (*this)[i]-v[i];

      return ret;
    }

    inline n_vector_t operator+(const n_vector_t & v) const
    {
      n_vector_t ret;

      for(size_t i = 0 ; i < N;++i )
        ret[i] = (*this)[i]+v[i];

      return ret;
    }

    friend std::istream &operator>>( std::istream &stream, n_vector_t &e )
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

    static n_vector_t zero;

    static n_vector_t one;
};

template<typename T, std::size_t N,bool O>
T dot_product(const n_vector_t<T,N,O>& v1,const n_vector_t<T,N,O>& v2)
{
  T ret;

  for(size_t i = 0 ; i < N;++i )
    ret += v1[i]*v2[i];

  return ret;
}

template<typename T, std::size_t N,bool O>
n_vector_t<T,N,O> n_vector_t<T,N,O>::zero = n_vector_t<T,N,O>::s_assign(0);

template<typename T, std::size_t N,bool O>
n_vector_t<T,N,O> n_vector_t<T,N,O>::one  = n_vector_t<T,N,O>::s_assign(1);

template<class T, std::size_t N>
bool operator< (const n_vector_t<T,N,false>& x, const n_vector_t<T,N,false>& y)
{
  T ex[N], ey[N];

  std::copy ( x.begin(), x.end(), ex );
  std::copy ( y.begin(), y.end(), ey );

  std::sort ( ex, ex + N );
  std::sort ( ey, ey + N );

  return ( std::lexicographical_compare ( ex, ex + N, ey, ey + N ) );
}


template<class T, std::size_t N>
bool operator== (const n_vector_t<T,N,false>& x, const n_vector_t<T,N,false>& y)
{
  T ex[N], ey[N];

  std::copy ( x.begin(), x.end(), ex );
  std::copy ( y.begin(), y.end(), ey );

  std::sort ( ex, ex + N );
  std::sort ( ey, ey + N );

  return std::equal(ex, ex+N, ey);
}

#endif
