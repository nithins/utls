#include <GL/glew.h>

#include <stdexcept>
#include <typeinfo>

#include <boost/foreach.hpp>
#include <boost/shared_array.hpp>

#include <glutils.h>


namespace glutils
{

  uint gl_sizeof ( const GLenum &type )
  {
    switch ( type )
    {

    case GL_UNSIGNED_INT:
      return sizeof ( uint );

    case GL_INT:
      return sizeof ( int );

    case GL_UNSIGNED_BYTE:
      return sizeof ( unsigned char );

    case GL_DOUBLE:
      return sizeof ( double );

    case GL_FLOAT:
      return sizeof ( float );
    }

    throw std::invalid_argument("unknown GLEnum type");

    return 0;
  }

  buf_obj_t::buf_obj_t()
  {
    m_target    = GL_ARRAY_BUFFER;
    m_id        = 0;

    m_src_type  = 0;
    m_src_ptr   = NULL;
    m_src_comp  = 0;

    m_size      = 0;
    m_stride    = 0;
  }

  buf_obj_t::buf_obj_t
      (
          const GLvoid* sp, // src_ptr
          const GLenum &st, // src_type
          const GLuint &sc, // src_comp
          const GLenum &t, // target
          const GLuint &siz, // size
          const GLuint &str //stride
          ) : m_id ( 0 ),
      m_src_ptr ( sp ),
      m_src_type ( st ),
      m_src_comp ( sc ),
      m_target ( t ),
      m_size ( siz ),
      m_stride ( str )

  {
    glGenBuffers ( 1, &m_id );

    glBindBuffer ( m_target, m_id );

    glBufferData ( m_target, m_size, m_src_ptr, GL_STATIC_DRAW );

    glBindBuffer ( m_target, 0 );
  }

  bufobj_ptr_t buf_obj_t::create_bo
      (
          const GLvoid* sp, // src_ptr
          const GLenum &st, // src_type
          const GLuint &sc, // src_comp
          const GLenum &t, // target
          const GLuint &siz, // size
          const GLuint &str //stride
          )
  {

    bufobj_ptr_t ret_ptr(new buf_obj_t(sp,st,sc,t,siz,str ));

    return ret_ptr;

  }

  bufobj_ptr_t buf_obj_t::create_bo()
  {
    bufobj_ptr_t ret_ptr(new buf_obj_t());

    return ret_ptr;
  }


  uint buf_obj_t::get_num_items() const
  {
    if ( m_src_comp == 0 )
      return 0;

    uint sizeof_type = gl_sizeof ( m_src_type );

    if ( m_stride == 0 )
      return m_size / ( m_src_comp* sizeof_type );

    return m_size / m_stride;
  }

  GLvoid *buf_obj_t::get_item_comp_ptr ( const uint &itemno, const uint &compno ) const
  {

    if ( m_src_ptr == NULL )
    {
      throw std::underflow_error("src_ptr == NULL");
    }

    uint type_size = gl_sizeof ( m_src_type );

    GLbyte * ret_ptr = ( GLbyte * ) m_src_ptr;

    if ( m_stride == 0 )
      ret_ptr += type_size * m_src_comp * itemno;
    else
      ret_ptr += m_stride  * itemno;

    ret_ptr += compno * type_size;

    return ( GLvoid* ) ret_ptr;
  }

  void buf_obj_t::bind_to_vertex_pointer(int offset) const
  {
    glEnableClientState ( GL_VERTEX_ARRAY );

    glBindBuffer ( m_target, m_id );

    glVertexPointer ( m_src_comp, m_src_type, m_stride,
                     (GLvoid*)(offset*gl_sizeof(m_src_type)*m_src_comp) );
  }

  void buf_obj_t::unbind_from_vertex_pointer() const
  {
    glBindBuffer ( m_target, 0 );

    glDisableClientState ( GL_VERTEX_ARRAY );
  }

  void buf_obj_t::bind_to_vertex_attrib_pointer ( GLuint attribno,int offset) const
  {
    glEnableVertexAttribArray ( attribno );

    glBindBuffer ( m_target, m_id );

    glVertexAttribPointer ( attribno, m_src_comp, m_src_type, false, m_stride,
                           (GLvoid*)(offset*gl_sizeof(m_src_type)*m_src_comp));

  }

  void buf_obj_t::unbind_from_vertex_attrib_pointer ( GLuint attribno ) const
  {
    glBindBuffer ( m_target, 0 );

    glDisableVertexAttribArray ( attribno );
  }


  void buf_obj_t::bind_to_color_pointer(int offset) const
  {
    glEnableClientState ( GL_COLOR_ARRAY );

    glBindBuffer ( m_target, m_id );

    glColorPointer ( m_src_comp, m_src_type, m_stride,
                    (GLvoid*)(offset*gl_sizeof(m_src_type)*m_src_comp));
  }

  void buf_obj_t::unbind_from_color_pointer() const
  {
    glBindBuffer ( m_target, 0 );

    glDisableClientState ( GL_COLOR_ARRAY );
  }

  void buf_obj_t::bind_to_normal_pointer(int offset) const
  {
    glEnableClientState ( GL_NORMAL_ARRAY );

    glBindBuffer ( m_target, m_id );

    glNormalPointer ( m_src_type, m_stride,
                     (GLvoid*)(offset*gl_sizeof(m_src_type)*m_src_comp));
  }

  void buf_obj_t::unbind_from_normal_pointer() const
  {
    glBindBuffer ( m_target, 0 );

    glDisableClientState ( GL_NORMAL_ARRAY );
  }

  buf_obj_t::~buf_obj_t()
  {
    if ( m_id )
    {
      glBindBuffer ( m_target, m_id );

      glDeleteBuffers ( 1, &m_id );

      glBindBuffer ( m_target, 0 );

      m_id = 0;
    }
  }

  template <typename T,int N>  inline boost::shared_array<T> to_carray
  (const std::vector<bnu::bounded_vector<T,N> > &vl)
  {
    boost::shared_array<T> ol(new T[vl.size()*N]);

    for(int i = 0 ; i < int(vl.size()) ; ++i)
    {
      for(int k =0 ;k < N; ++k)
        (ol)[N*i+k] = vl[i][k];
    }

    return ol;
  }

  bufobj_ptr_t make_buf_obj( const vertex_list_t &vec)
  {
    BOOST_AUTO(vl,(to_carray <double,3>(vec)));

    return buf_obj_t::create_bo
        ( vl.get() ,GL_DOUBLE,3,GL_ARRAY_BUFFER,
          vec.size()*sizeof(double)*3,0);
  }

  bufobj_ptr_t make_buf_obj( const quad_idx_list_t &vec)
  {
    BOOST_AUTO(vl,(to_carray <idx_t,4>(vec)));

    return buf_obj_t::create_bo
        ( vl.get() ,GL_UNSIGNED_INT,4,GL_ELEMENT_ARRAY_BUFFER,
          vec.size()*sizeof(unsigned int)*4,0);
  }

  bufobj_ptr_t make_buf_obj( const tri_idx_list_t &vec)
  {
    BOOST_AUTO(vl,(to_carray <idx_t,3>(vec)));

    return buf_obj_t::create_bo
        ( vl.get() ,GL_UNSIGNED_INT,3,GL_ELEMENT_ARRAY_BUFFER,
          vec.size()*sizeof(unsigned int)*3,0);
  }

  bufobj_ptr_t make_buf_obj( const line_idx_list_t &vec)
  {
    BOOST_AUTO(vl,(to_carray <idx_t,2>(vec)));

    return buf_obj_t::create_bo
        ( vl.get() ,GL_UNSIGNED_INT,2,GL_ELEMENT_ARRAY_BUFFER,
          vec.size()*sizeof(unsigned int)*2,0);

  }

  bufobj_ptr_t make_buf_obj( const point_idx_list_t &vec)
  {
    return buf_obj_t::create_bo
        ( &vec[0] ,GL_UNSIGNED_INT,1,GL_ELEMENT_ARRAY_BUFFER,
          vec.size()*sizeof(unsigned int)*1,0);
  }

  class buf_obj_recast_t:public buf_obj_t
  {
    bufobj_ptr_t m_orig;
  public:

    buf_obj_recast_t(bufobj_ptr_t ptr ,const GLuint nc)
    {
      m_id       = ptr->id();
      m_src_ptr  = ptr->src_ptr();
      m_src_type = ptr->src_type();
      m_src_comp = nc;
      m_target   = ptr->target();
      m_size     = ptr->size();
      m_stride   = ptr->stride();

      m_orig     = ptr;// hold on to this till you die.
    }
  };

  bufobj_ptr_t recast_buf_obj_num_components(bufobj_ptr_t optr, GLuint nc)
  {
    bufobj_ptr_t ptr(new buf_obj_recast_t(optr,nc));

    return ptr;
  }
}
