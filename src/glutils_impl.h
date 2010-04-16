#if !defined(GL_UTILS_IMPL_INCLUDED)
#define GL_UTILS_IMPL_INCLUDED

#include <glutils.h>

namespace glutils
{
  template<typename get_type_t>

      get_type_t gl_get_num_val ( void *val, const GLenum &type )
  {

    switch ( type )
    {

    case GL_UNSIGNED_INT:
      {
        uint * v = ( uint * ) val;
        return ( get_type_t ) *v;
      }

    case GL_INT:
      {
        int * v = ( int * ) val;
        return ( get_type_t ) *v;
      }

    case GL_UNSIGNED_BYTE:
      {
        unsigned char* v = ( unsigned char* ) val;
        return ( get_type_t ) *v;
      }

    case GL_DOUBLE:
      {
        double * v = ( double * ) val;
        return ( get_type_t ) *v;
      }

    case GL_FLOAT:
      {
        float * v = ( float * ) val;
        return ( get_type_t ) *v;
      }
    }

    return ( get_type_t ) 0;
  }

  double* compute_normals (bufobj_ptr_t , const bufobj_ptr_t );

  void  compute_tri_normal (buf_obj_t &, const uint * , double *normal );

  void init_text_rendering();
}

#endif
