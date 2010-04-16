#ifndef __VECUTILS_HPP_INCLUDED
#define __VECUTILS_HPP_INCLUDED
#include <math.h>

#define ADDVECVEC3(V,V1,V2)		\
  V[0] = V1[0]+V2[0];			\
  V[1] = V1[1]+V2[1];			\
  V[2] = V1[2]+V2[2];			\

#define ADDVECVEC3_(V,V1)		\
  ADDVECVEC3(V,V,V1);			\

#define ADDVECVECVEC3(V,V1,V2,V3)		\
  V[0] = V1[0]+V2[0]+V3[0];			\
  V[1] = V1[1]+V2[1]+V3[1];			\
  V[2] = V1[2]+V2[2]+V3[2];			\

#define DIVVECSCL3(V,V1,S)				\
  V[0] = V1[0]/(S);					\
  V[1] = V1[1]/(S);					\
  V[2] = V1[2]/(S);					\

#define DIVVECSCL3_(V,S)				\
  DIVVECSCL3(V,V,S);

#define MULVECSCL3(V,V1,S)				\
  V[0] = V1[0]*(S);					\
  V[1] = V1[1]*(S);					\
  V[2] = V1[2]*(S);					\

#define MULVECSCL3_(V,S) MULVECSCL3(V,V,S)

#define MULVECSCL4(V,V1,S)				\
  V[0] = V1[0]*(S);					\
  V[1] = V1[1]*(S);					\
  V[2] = V1[2]*(S);					\
  V[3] = V1[3]*(S);					\

#define MULVECSCL4_(V,S) MULVECSCL4(V,V,S)


#define SUBVECVEC3(V,V1,V2)			\
  V[0] = V1[0]-V2[0];				\
  V[1] = V1[1]-V2[1];				\
  V[2] = V1[2]-V2[2];				\

#define SUBVECVEC3_(V,S) SUBVECVEC3(V,V,S)	\


#define CROSSPROD3(V,A,B)			\
  V[0] = A[1]*B[2]-A[2]*B[1];			\
  V[1] = A[2]*B[0]-A[0]*B[2];			\
  V[2] = A[0]*B[1]-A[1]*B[0];			\

#define DOTPROD3(A,B) ((A)[0]*(B)[0]+(A)[1]*(B)[1]+(A)[2]*(B)[2])

#define SETVEC3(V,v1,v2,v3)			\
  V[0] = v1;					\
  V[1] = v2;					\
  V[2] = v3;					\

#define SETVEC2(V,v1,v2)			\
  V[0] = v1;					\
  V[1] = v2;					\

#define SETVEC4(V,v1,v2,v3,v4)			\
  V[0] = v1;					\
  V[1] = v2;					\
  V[2] = v3;					\
  V[3] = v4;					\

#define COPYVEC3(V1,V2)				\
  V1[0]=V2[0];					\
  V1[1]=V2[1];					\
  V1[2]=V2[2];					\

#define COPYVEC4(V1,V2)				\
  V1[0]=V2[0];					\
  V1[1]=V2[1];					\
  V1[2]=V2[2];					\
  V1[3]=V2[3];					\


#define MAG2VEC3(V) (V[0]*V[0]+V[1]*V[1]+V[2]*V[2])

#define MAGVEC3(V) sqrt(MAG2VEC3(V))

#define NORMALIZEVEC3(T,V,V1)			\
  {						\
    T mtv_mag = MAGVEC3(V1);			\
    if ( mtv_mag != 0 )				\
      {						\
        DIVVECSCL3(V,V1,mtv_mag);		\
      }						\
  }						\

#define NORMALIZEVEC3_(T,V) NORMALIZEVEC3(T,V,V)


#define CENTROIDPTPTPT3(C,P1,P2,P3)		\
  ADDVECVECVEC3(C,P1,P2,P3);			\
  DIVVECSCL3(C,C,3);				\

#define NORMALPTPTPT3(T,N,P1,P2,P3)			\
  {							\
    T mtv_a[3],mtv_b[3];				\
    SUBVECVEC3(mtv_a,P2,P1);				\
    SUBVECVEC3(mtv_b,P3,P2);				\
    CROSSPROD3(N,mtv_a,mtv_b);				\
    NORMALIZEVEC3(T,N,N);				\
  }

#define STREAMVEC3(V) (V)[0]<<","<<(V)[1]<<","<<(V)[2]

#define STREAMVEC4(V) (V)[0]<<","<<(V)[1]<<","<<(V)[2]<<","<<(V)[3]

#define COMMAVEC3(V) V[0],V[1],V[2]

  #define COMMAVEC4(V) V[0],V[1],V[2],V[3]

#define DEFINESETFUNC(func,type,var)			\
  void func(const type v)				\
  {							\
    var = v;						\
  }							\

#define DEFINESETFUNC_(func,type,var,extra)		\
  void func(const type v)				\
  {							\
    var = v;						\
    extra;						\
  }							\

#define DEFINESETFUNC3(func,type,var)			\
  void func(const type *v)				\
  {							\
    COPYVEC3(var,v);					\
  }							\
  void func(const type v1,const type v2,const type v3)	\
  {							\
    SETVEC3(var,v1,v2,v3);				\
  }							\

#define DEFINESETFUNC3_(func,type,var,extra)		\
  void func(const type *v)				\
  {							\
    COPYVEC3(var,v);					\
    extra;						\
  }							\
  void func(const type v1,const type v2,const type v3)	\
  {							\
    SETVEC3(var,v1,v2,v3);				\
    extra;						\
  }							\

#define DEFINESETFUNC4(func,type,var)					\
  void func(const type *v)						\
  {									\
    COPYVEC4(var,v);							\
  }									\
  void func(const type v1,const type v2,const type v3,const type v4)	\
  {									\
    SETVEC4(var,v1,v2,v3,v4);						\
  }

//////////////////////////////////////////////////////////////////////////////////////////

#define DEFINESETVFUNC(func,type,var)			\
  virtual void func(const type v)			\
  {							\
    var = v;						\
  }							\

#define DEFINESETVFUNC_(func,type,var,extra)			\
  virtual void func(const type v)				\
  {								\
    var = v;							\
    extra;							\
  }								\

#define DEFINESETVFUNC3(func,type,var)				\
  virtual void func(const type *v)				\
  {								\
    COPYVEC3(var,v);						\
  }								\
  virtual void func(const type v1,const type v2,const type v3)	\
  {								\
    SETVEC3(var,v1,v2,v3);					\
  }								\

#define DEFINESETVFUNC3_(func,type,var,extra)			\
  virtual void func(const type *v)				\
  {								\
    COPYVEC3(var,v);						\
    extra;							\
  }								\
  virtual void func(const type v1,const type v2,const type v3)	\
  {								\
    SETVEC3(var,v1,v2,v3);					\
    extra;							\
  }								\

#define DEFINESETVFUNC4(func,type,var)					\
  virtual void func(const type *v)					\
  {									\
    COPYVEC4(var,v);							\
  }									\
  virtual void func(const type v1,const type v2,const type v3,const type v4) \
  {									\
    SETVEC4(var,v1,v2,v3,v4);						\
  }									\

#define DEFINESETVFUNC4_(func,type,var,extra)				\
  virtual void func(const type *v)					\
  {									\
    COPYVEC4(var,v);							\
    extra;								\
  }									\
  virtual void func(const type v1,const type v2,const type v3,const type v4) \
  {									\
    SETVEC4(var,v1,v2,v3,v4);						\
    extra;								\
  }									\
                                                                        \


/////////////////////////////////////////////////////////////////////////////////////////

#define DEFINEGETVFUNC(func,type,var)			\
  virtual void func(type &v)				\
  {							\
    v = var;						\
  }							\
  virtual type func() { return var;}			\




#define DEFINEGETVFUNC3(func,type,var)				\
  virtual void func(type *v)					\
  {								\
    COPYVEC3(v,var);						\
  }								\
  virtual void func(type &v1,type &v2,type &v3)			\
  {								\
    v1 = var[0];						\
    v2 = var[1];						\
    v3 = var[2];						\
  }								\

#define DEFINEGETVFUNC4(func,type,var)				\
  virtual void func(type *v)					\
  {								\
    COPYVEC4(v,var);						\
  }								\
  virtual void func(type &v1,type &v2,type &v3,type &v4)	\
  {								\
    v1 = var[0];						\
    v2 = var[1];						\
    v3 = var[2];						\
    v4 = var[3];						\
  }								\

////////////////////////////////////////////////////////////////////////////////////////////
#define DECLAREPVSETFUNC(func,type)			\
  virtual void func(const type)=0;			\


#define DECLAREPVSETFUNC3(func,type)				\
  virtual void func(const type *v)=0;				\
  virtual void func(const type ,const type ,const type )=0;	\


#define DECLAREPVSETFUNC4(func,type)					\
  virtual void func(const type *v)=0;					\
  virtual void func(const type ,const type ,const type ,const type )=0;	\

#define DECLAREPVGETFUNC(func,type)			\
  virtual void func(type &)=0;				\
  virtual type func()=0;				\


#define DECLAREPVGETFUNC3(func,type)			\
  virtual void func(type *v)=0;				\
  virtual void func(type &,type &,type & )=0;		\

#define DECLAREPVGETFUNC4(func,type)			\
  virtual void func(type *v)=0;				\
  virtual void func(type &,type &,type &,type & )=0;	\


#endif//__VECUTILS_HPP_INCLUDED
