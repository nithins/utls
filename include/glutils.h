#ifndef __GLUTILS_H_INCLUDED
#define __GLUTILS_H_INCLUDED

#include <vector>

#include <boost/shared_ptr.hpp>

#include <GL/gl.h>

#include <n_vector.h>


namespace glutils
{
  typedef unsigned int             idx_t;

  typedef idx_t                    point_idx_t;
  typedef std::vector<point_idx_t> point_idx_list_t;
  typedef n_vector_t<idx_t,2>      line_idx_t;
  typedef std::vector<line_idx_t>  line_idx_list_t;
  typedef n_vector_t<idx_t,3>      tri_idx_t;
  typedef std::vector<tri_idx_t>   tri_idx_list_t;

  typedef n_vector_t<double,3>     vertex_t;
  typedef std::vector<vertex_t>    vertex_list_t;
  typedef n_vector_t<double,3>     color_t;
  typedef std::vector<color_t>     color_list_t;

  typedef std::vector<std::string> string_list_t;


  class buf_obj_t;

  typedef boost::shared_ptr<buf_obj_t> bufobj_ptr_t;

  void init();

  // a class that allows to buffer data on the gpu

  class buf_obj_t
  {
    GLuint        m_id; // vbo/pbo id
    const GLvoid *m_src_ptr;
    GLenum        m_src_type;
    GLuint        m_src_comp;
    GLenum        m_target; // usually GL_ARRAY_BUFFER/GL_ELEMENT_ARRAY_BUFFER for vbo's
    GLuint        m_size; // size in bytes
    GLuint        m_stride; // stride in bytes

    buf_obj_t();

    buf_obj_t
        (
            const GLvoid* src_ptr, // src_ptr
            const GLenum &src_type, // src_type
            const GLuint &src_comp, // src_comp
            const GLenum &target, // target
            const GLuint &size, // size
            const GLuint &stride//stride
            );


  public:

    inline GLuint id()
    {
      return m_id;
    }

    inline const GLvoid *src_ptr()
    {
      return m_src_ptr;
    }

    inline GLenum  src_type()
    {
      return m_src_type;
    }

    inline GLenum target()
    {
      return m_target;
    }

    inline GLuint src_comp()
    {
      return m_src_comp;
    }

    uint     get_num_items() const;

    GLvoid  *get_item_comp_ptr ( const uint &itemno, const uint &compno ) const;

    static bufobj_ptr_t  create_bo
        ( const GLvoid* src_ptr, // src_ptr
          const GLenum &src_type, // src_type
          const GLuint &src_comp, // src_comp
          const GLenum &target, // target
          const GLuint &size, // size
          const GLuint &stride//stride
          );

    static bufobj_ptr_t create_bo();

    ~buf_obj_t();

    void bind_to_vertex_pointer() const;
    void unbind_from_vertex_pointer() const;
    void bind_to_vertex_attrib_pointer ( GLuint ) const;
    void unbind_from_vertex_attrib_pointer ( GLuint ) const;
    void bind_to_color_pointer() const;
    void unbind_from_color_pointer() const;
    void bind_to_normal_pointer() const;
    void unbind_from_normal_pointer() const;

  };

  // create a buffered from list data
  // list can be destroyed after this call.

  bufobj_ptr_t make_buf_obj( const vertex_list_t &);

  bufobj_ptr_t make_buf_obj( const tri_idx_list_t &);

  bufobj_ptr_t make_buf_obj( const line_idx_list_t &);

  bufobj_ptr_t make_buf_obj( const point_idx_list_t &);

  bufobj_ptr_t make_buf_obj();

  void compute_extent ( bufobj_ptr_t v_buf , double *);

  void compute_extent ( const vertex_list_t &, double * );

  class renderable_t
  {
  public:
    virtual int  render() = 0 ;
    virtual bool get_extent ( double * ){return false;}
    virtual ~renderable_t() {}
  };

  renderable_t * create_buffered_text_ren
      (const string_list_t &s,
       const vertex_list_t &p);

  renderable_t * create_buffered_points_ren
      ( bufobj_ptr_t  v,
        bufobj_ptr_t  i,
        bufobj_ptr_t  c);

  renderable_t * create_buffered_lines_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t i,
        bufobj_ptr_t c);

  renderable_t * create_buffered_triangles_ren
      ( bufobj_ptr_t ,
        bufobj_ptr_t ,
        bufobj_ptr_t );

  renderable_t * create_buffered_tristrip_ren
      ( bufobj_ptr_t ,
        bufobj_ptr_t ,
        bufobj_ptr_t  );

  renderable_t * create_buffered_flat_triangles_ren
      ( bufobj_ptr_t ,
        bufobj_ptr_t ,
        bufobj_ptr_t );

  renderable_t * create_buffered_flat_tetrahedrons_ren
      ( bufobj_ptr_t ,
        bufobj_ptr_t ,
        bufobj_ptr_t  );

  void read_off_file
      ( const char *filename,
        vertex_list_t  vlist,
        tri_idx_list_t tlist);

  void read_tri_file
      ( const char *filename,
        vertex_list_t  vlist,
        tri_idx_list_t tlist);

  renderable_t * create_buffered_ren_off_file
      ( const char * filename ,
        bool use_strips = true );

  void render3d_ascii_text(const char * text,int size,vertex_t pos);

  void draw_aabb_line(vertex_t lc,vertex_t uc);

}

void DrawAxes();

void DrawZArrow ();

#endif
