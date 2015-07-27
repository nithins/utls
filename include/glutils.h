#ifndef __GLUTILS_H_INCLUDED
#define __GLUTILS_H_INCLUDED

#include <vector>

#include <boost/shared_ptr.hpp>

#include <GL/gl.h>

#include <cpputils.h>

namespace glutils
{
  typedef unsigned int             idx_t;

  typedef idx_t                        point_idx_t;
  typedef std::vector<point_idx_t>     point_idx_list_t;
  typedef la::uivec2_t                 line_idx_t;
  typedef std::vector<line_idx_t>      line_idx_list_t;
  typedef la::uivec4_t                 line_adj_idx_t;
  typedef std::vector<line_adj_idx_t>  line_adj_idx_list_t;
  typedef la::uivec3_t                 tri_idx_t;
  typedef std::vector<tri_idx_t>       tri_idx_list_t;
  typedef la::uivec4_t                 quad_idx_t;
  typedef std::vector<quad_idx_t>      quad_idx_list_t;

  typedef la::dvec3_t              vertex_t;
  typedef std::vector<vertex_t>    vertex_list_t;
  typedef la::dvec3_t              color_t;
  typedef std::vector<color_t>     color_list_t;
  typedef la::dvec3_t              normal_t;
  typedef std::vector<vertex_t>    normal_list_t;

  typedef la::fvec4_t      color4f_t;
  typedef la::fvec4_t      vertex4f_t;

  typedef std::vector<std::string> string_list_t;

  class buf_obj_t;

  typedef boost::shared_ptr<buf_obj_t> bufobj_ptr_t;

  void init();

  void clear();

  // a class that allows to buffer data on the gpu

  class buf_obj_t
  {
  protected:
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

    inline GLenum size()
    {
      return m_size;
    }

    inline GLuint stride()
    {
      return m_stride;
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

    void bind_to_vertex_pointer(int offset=0) const;
    void unbind_from_vertex_pointer() const;
    void bind_to_vertex_attrib_pointer ( GLuint,int offset=0) const;
    void unbind_from_vertex_attrib_pointer ( GLuint ) const;
    void bind_to_color_pointer(int offset=0) const;
    void unbind_from_color_pointer() const;
    void bind_to_normal_pointer(int offset=0) const;
    void unbind_from_normal_pointer() const;

  };

  struct material_properties_t
  {
    color4f_t  ambient;
    color4f_t  diffuse;
    color4f_t  specular;
    color4f_t  emission;
    int        shininess;

    // face is GL_FRONT or GL_BACK or GL_FRONT_AND_BACK
    inline void render_all(GLenum face) const
    {
      glMaterialfv ( face, GL_AMBIENT, &ambient[0] );
      glMaterialfv ( face, GL_DIFFUSE, &diffuse[0] );
      glMaterialfv ( face, GL_SPECULAR, &specular[0] );
      glMaterialfv ( face, GL_EMISSION, &emission[0] );
      glMateriali  ( face, GL_SHININESS, shininess );
    }

    // face is GL_FRONT or GL_BACK
    inline void read_all(GLenum face)
    {
      glGetMaterialfv( face, GL_AMBIENT, &ambient[0] );
      glGetMaterialfv( face, GL_DIFFUSE, &diffuse[0] );
      glGetMaterialfv( face, GL_SPECULAR, &specular[0] );
      glGetMaterialfv( face, GL_EMISSION, &emission[0] );
      glGetMaterialiv( face, GL_SHININESS, &shininess );
    }
  };

  struct light_properties_t
  {
    color4f_t   ambient;
    color4f_t   diffuse;
    color4f_t   specular;
    vertex4f_t  position;

    float       att_constant;
    float       att_linear;
    float       att_quadratic;

    bool        enabled;

    inline void render(int n) const
    {
      if(enabled)
        glEnable(GL_LIGHT0+n);
      else
        glDisable(GL_LIGHT0+n);

      glLightfv(GL_LIGHT0+n, GL_AMBIENT,  &ambient[0]);
      glLightfv(GL_LIGHT0+n, GL_DIFFUSE,  &diffuse[0]);
      glLightfv(GL_LIGHT0+n, GL_SPECULAR, &specular[0]);
      glLightfv(GL_LIGHT0+n, GL_POSITION, &position[0]);
      glLightf( GL_LIGHT0+n, GL_CONSTANT_ATTENUATION, att_constant);
      glLightf( GL_LIGHT0+n, GL_LINEAR_ATTENUATION, att_linear);
      glLightf( GL_LIGHT0+n, GL_QUADRATIC_ATTENUATION, att_quadratic);
    }
  };

  // create a buffered from list data
  // list can be destroyed after this call.

  bufobj_ptr_t recast_buf_obj_num_components( bufobj_ptr_t, GLuint nc);

  bufobj_ptr_t make_buf_obj( const vertex_list_t &);

  bufobj_ptr_t make_buf_obj( const tri_idx_list_t &);

  bufobj_ptr_t make_buf_obj( const line_idx_list_t &);

  bufobj_ptr_t make_buf_obj( const point_idx_list_t &);

  bufobj_ptr_t make_buf_obj( const quad_idx_list_t &);

  bufobj_ptr_t make_normals_buf_obj( const vertex_list_t &,const tri_idx_list_t &);

  bufobj_ptr_t make_normals_buf_obj( const vertex_list_t &v,const quad_idx_list_t &t);

  void compute_line_adj
    (const line_idx_list_t &ll ,
     line_adj_idx_list_t &ladj,
     vertex_list_t &vl);

  void compute_vertex_normals
      ( const vertex_list_t &vlist,
        const tri_idx_list_t &tlist,
        normal_list_t & nlist);

  void compute_vertex_normals
      ( const vertex_list_t   &vlist,
        const quad_idx_list_t &qlist,
        normal_list_t & nlist);

  void compute_extent ( const vertex_list_t &, double * );
  vertex_t compute_center ( const vertex_list_t &);

  class renderable_t
  {
  public:
    virtual int    render() = 0 ;
    virtual void   gl_init(){}
    virtual void   animate(double dt){}
    virtual bool   isAnimation(){return false;}
    virtual bool   processKey(int key, int modifiers){return false;}

    virtual bool get_extent ( double * ){return false;}
    virtual ~renderable_t() {}
  };

  typedef boost::shared_ptr<renderable_t> renderable_ptr_t;

  renderable_t * create_buffered_points_ren
      ( bufobj_ptr_t  v,
        bufobj_ptr_t  i = bufobj_ptr_t(),
        bufobj_ptr_t  c = bufobj_ptr_t());

  renderable_t * create_buffered_lines_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t i = bufobj_ptr_t(),
        bufobj_ptr_t c = bufobj_ptr_t());

  renderable_t * create_buffered_triangles_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t i,
        bufobj_ptr_t n,
        bufobj_ptr_t c = bufobj_ptr_t()
        );

  renderable_t * create_buffered_quads_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t i,
        bufobj_ptr_t n,
        bufobj_ptr_t c = bufobj_ptr_t()
        );

  renderable_t * create_buffered_flat_triangles_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t c = bufobj_ptr_t());

  renderable_t * create_buffered_normals_ren
      (bufobj_ptr_t v,
       bufobj_ptr_t i,
       bufobj_ptr_t c,
       bufobj_ptr_t n,
       double n_len = 1.0);

  renderable_t * create_buffered_flat_tetrahedrons_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t c = bufobj_ptr_t() );

  renderable_t * create_lines_adj_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t l = bufobj_ptr_t(),
        bufobj_ptr_t c = bufobj_ptr_t());


#ifdef UTLS_ENABLE_TEXTREN
  renderable_t * create_buffered_text_ren
      (const string_list_t &s,
       const vertex_list_t &p);
#endif

#ifdef UTLS_USE_TRISTRIPPER

  renderable_t * create_buffered_tristrip_ren
      ( bufobj_ptr_t v,
        bufobj_ptr_t t,
        bufobj_ptr_t c = bufobj_ptr_t() );

#endif


  void read_off_file
      ( const char *filename,
        vertex_list_t  &vlist,
        tri_idx_list_t &tlist);

  void read_tri_file
      ( const char *filename,
        vertex_list_t  &vlist,
        tri_idx_list_t &tlist);

  void read_tri_file
      ( const char *filename,
        vertex_list_t  &vlist);


  renderable_t * create_buffered_ren_off_file
      ( const char * filename ,
        bool use_strips = true );

  void render3d_ascii_text(const char * text,int size,vertex_t pos);

  void draw_aabb_line(vertex_t lc,vertex_t uc);

  void smooth_lines(vertex_list_t &vl, const line_idx_list_t &ll,int NITER = 2);

  inline vertex_t cross_product(const vertex_t &u,const vertex_t &v)
  {
    vertex_t p;

    p[0] = u[1]*v[2]-v[1]*u[2];
    p[1] = u[2]*v[0]-v[2]*u[0];
    p[2] = u[0]*v[1]-v[0]*u[1];

    return p;
  }
}

void DrawAxes();

void DrawZArrow ();

#endif
