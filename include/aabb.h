#ifndef RECT_H_INCLUDED
#define RECT_H_INCLUDED

#include <n_vector.h>

namespace aabb
{
  template <typename coord_type>
      struct aabb_range_t:public n_vector_t<coord_type,2>
  {
    aabb_range_t ( const coord_type &l,const coord_type &u)
    {

      (*this)[0] = std::min ( l,u);
      (*this)[1] = std::max ( l,u);
    }

    aabb_range_t ():n_vector_t<coord_type,2>(0,0){}

    inline bool isInOpen(const coord_type &c) const
    {
      return (( (*this)[0] < c ) && (  c < (*this)[1] ));
    }

    inline bool isInClosed(const coord_type &c) const
    {
      return (( (*this)[0] <= c ) && (  c <= (*this)[1] ));
    }

    inline bool isOnBndry(const coord_type &c) const
    {
      return (( (*this)[0] == c ) || (  c == (*this)[1] ));
    }

    inline bool contains(const aabb_range_t & r) const
    {
      return isInOpen(r[0]) && isInOpen(r[1]);
    }

    inline bool intersects(const aabb_range_t & r) const
    {
      return !((r[0] > (*this)[1]) || ((*this)[0] > r[1]));
    }

    inline aabb_range_t intersection(const aabb_range_t & r) const
    {
      ASSERT(((*this)[0] <= r[1])||(r[0] <= (*this)[1]));

      return aabb_range_t(std::max(r[0],(*this)[0]),std::min(r[1],(*this)[1]));
    }

    inline bool is_valid()
    {
      return ((*this)[0] >= (*this)[1]);
    }

//    inline aabb_range_t range_union(const aabb_range_t & r) const
//    {
//      return aabb_range_t(std::min(r[0],(*this)[0]),std::max(r[1],(*this)[1]));
//    }

    inline coord_type span() const
    {
      return ((*this)[1]-(*this)[0]);
    }
  };

  template <typename coord_type,uint max_dim>
      struct aabb_t: public n_vector_t< aabb_range_t<coord_type> ,max_dim>
  {
    typedef aabb_range_t<coord_type> range_t;

    typedef n_vector_t<range_t,max_dim> base_t;

    typedef n_vector_t<coord_type,max_dim> point_t;

//    aabb_t(const range_t &r1,const range_t &r2,const range_t &r3)
//    {
//      (*this)[0] = r1;
//      (*this)[1] = r2;
//      (*this)[2] = r3;
//    }

//    aabb_t(const range_t &r1,const range_t &r2)
//    {
//      (*this)[0] = r1;
//      (*this)[1] = r2;
//    }

    aabb_t(const point_t &p1,const point_t &p2)
    {
      for(uint i = 0 ; i < base_t::static_size; ++i)
        (*this)[i] = range_t(p1[i],p2[i]);
    }

    aabb_t(){}


    bool isInInterior ( const point_t & p ) const
    {
      bool ret = true;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        ret &= (*this)[i].isInOpen(p[i]);

      return ret;
    }

    bool contains ( const point_t & p ) const
    {
      bool ret = true;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        ret &= (*this)[i].isInClosed(p[i]);

      return ret;
    }

    bool isOnBoundry ( const point_t & p ) const
    {
      return ( contains ( p ) && !isInInterior ( p ) );
    }

    int boundryCount( const point_t & p ) const
    {
      int ret = 0;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        ret += ((*this)[i].isOnBndry(p[i]))?(1):(0);

      return ret;
    }


    bool contains ( const aabb_t &r ) const
    {
      bool ret = true;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        ret &= (*this)[i].contains(r[i]);

      return ret;
    }

    bool intersects ( const aabb_t &r ) const
    {
      bool ret = true;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        ret &= (*this)[i].intersects(r[i]);

      return ret;
    }

    aabb_t intersection(const aabb_t & r) const
    {
      aabb_t ret;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        ret[i] = (*this)[i].intersection(r[i]);

      return ret;
    }

//    aabb_t bounding_box(const aabb_t & r) const
//    {
//      aabb_t ret;

//      for(size_t i = 0 ; i < base_t::static_size;++i )
//        ret[i] = (*this)[i].range_union(r[i]);

//      return ret;
//    }

    point_t lower_corner() const
    {
      point_t c;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        c[i]= (*this)[i][0];

      return c;
    }

    point_t upper_corner() const
    {
      point_t c;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        c[i]= (*this)[i][1];

      return c;
    }

    inline point_t lc() const
    {
      return lower_corner();
    }

    inline point_t uc() const
    {
      return upper_corner();
    }

    inline point_t span() const
    {
      point_t c;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        c[i]= (*this)[i].span();

      return c;
    }

    typedef int64_t offset_t;

    inline offset_t point_offset(const point_t &p) const
    {
      n_vector_t<offset_t,base_t::static_size> slice_size;

      slice_size[0] = 1;

      for(size_t i = 1 ; i < base_t::static_size;++i )
        slice_size[i] = span()[i-1]*slice_size[i-1];

      offset_t o = 0;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        o += slice_size[i]*p[i];

      return o;
    }

    inline offset_t volume() const
    {
      point_t s = span();

      offset_t v = 1;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        v *= s[i];

      return v;
    }


    coord_type eff_dim() const
    {
      coord_type d = 0 ;

      for(size_t i = 0 ; i < base_t::static_size;++i )
        d += ((*this)[i][1] != (*this)[i][0]) ?(1):(0);

      return d;
    }
  };
}

#endif

