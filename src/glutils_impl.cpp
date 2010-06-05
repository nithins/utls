#include <fstream>

#include <GL/glew.h>

#include <glutils_impl.h>
#include <cpputils.h>
#include <logutil.h>
#include <vecutils.h>
#include <limits>

void DrawAxes()
{
  glPushMatrix();

  glColor4f ( 0.1, 0.1, 0.9, 1 );
  DrawZArrow();

  glColor4f ( 0.90, 0.1, 0.1, 1 );
  glRotatef ( 90, 0, 1, 0 );
  DrawZArrow();

  glColor4f ( 0.1, 0.9, 0.1, 1 );
  glRotatef ( -90, 1, 0, 0 );
  DrawZArrow();

  glPopMatrix();

}

void DrawZArrow ()
{

  static glutils::renderable_t * s_z_arrow_ren = NULL;

  if(s_z_arrow_ren == NULL )
  {
    static const double AXES_WIDTH = 0.01;

    static const double a_vert[][3] =
    {
      { -AXES_WIDTH, -AXES_WIDTH, -1.0},
      { AXES_WIDTH, -AXES_WIDTH, -1.0},
      { AXES_WIDTH, AXES_WIDTH, -1.0},
      { -AXES_WIDTH, AXES_WIDTH, -1.0},

      { -AXES_WIDTH, -AXES_WIDTH, 1.0},
      { AXES_WIDTH, -AXES_WIDTH, 1.0},
      { AXES_WIDTH, AXES_WIDTH, 1.0},
      { -AXES_WIDTH, AXES_WIDTH, 1.0},

      { -AXES_WIDTH*3, -AXES_WIDTH*3, 1.0},
      { AXES_WIDTH*3, -AXES_WIDTH*3, 1.0},
      { AXES_WIDTH*3, AXES_WIDTH*3, 1.0},
      { -AXES_WIDTH*3, AXES_WIDTH*3, 1.0},

      { 0, 0, 1.0 + 16*AXES_WIDTH}
    };


    static const uint a_tind[][3] =
    {
      { 0 , 3 , 2 },
      { 0 , 2 , 1 },
      { 4 , 7 , 6 },
      { 4 , 6 , 5 },
      { 8 , 11 , 10 },
      { 8 , 10 , 9 },
      { 1 , 5 , 4 },
      { 1 , 4 , 0 },
      { 2 , 6 , 5 },
      { 2 , 5 , 1 },
      { 3 , 7 , 6 },
      { 3 , 6 , 2 },
      { 0 , 4 , 7 },
      { 0 , 7 , 3 },

      {12, 8, 9},
      {12, 9, 10},
      {12, 10, 11},
      {12, 11, 8}
    };

    s_z_arrow_ren = glutils::create_buffered_triangles_ren
                    ( glutils::buf_obj_t::create_bo
                      (a_vert,GL_DOUBLE,3,
                       GL_ARRAY_BUFFER,sizeof(a_vert),0),
                      glutils::buf_obj_t::create_bo
                      (a_tind,GL_UNSIGNED_INT,3,
                       GL_ELEMENT_ARRAY_BUFFER,sizeof(a_tind),0),
                      glutils::buf_obj_t::create_bo());

  }

  s_z_arrow_ren->render();
}

namespace glutils
{
  void  compute_tri_normal
      ( bufobj_ptr_t v_bo,
        const uint * v,
        double *n )
  {

    double p[3][3];

    for ( uint j = 0 ; j < 3;j++ )
    {
      for ( uint k = 0 ; k < 3;k++ )
      {
        p[j][k] = gl_get_num_val<double>
                  ( v_bo->get_item_comp_ptr ( v[j], k ), v_bo->src_type() );
      }
    }

    NORMALPTPTPT3 ( double, n, ( p[0] ), ( p[1] ), ( p[2] ) );
  }

  double* compute_normals ( bufobj_ptr_t v_bo, bufobj_ptr_t t_bo )
  {
    uint num_vers = v_bo->get_num_items();
    uint num_tris = t_bo->get_num_items();

    double * normals = new double[num_vers*3];

    for ( unsigned int i = 0 ; i < num_vers;i++ )
    {
      SETVEC3 ( ( &normals [3*i] ), 0.0, 0.0, 0.0 );
    }

    double n[3];

    uint   v[3];

    for ( unsigned int i = 0 ; i < num_tris;i++ )
    {

      for ( uint j = 0 ; j < 3;j++ )
      {
        v[j] = gl_get_num_val<uint> ( t_bo->get_item_comp_ptr ( i, j ),
                                      t_bo->src_type() );
      }

      compute_tri_normal ( v_bo, v, n );


      for ( uint j = 0 ; j < 3;j++ )
      {
        ADDVECVEC3_ ( ( &normals[3*v[j]] ), n );
      }
    }

    for ( unsigned int i = 0 ; i < num_vers;i++ )
    {
      NORMALIZEVEC3_ ( double, ( &normals[3*i] ) );
    }

    return normals;
  }

  void compute_extent ( bufobj_ptr_t v_buf , double * extent_out )
  {
    uint num_vers = v_buf->get_num_items();

    double extent[] =
    {
      std::numeric_limits<double>::max(), std::numeric_limits<double>::min(),
      std::numeric_limits<double>::max(), std::numeric_limits<double>::min(),
      std::numeric_limits<double>::max(), std::numeric_limits<double>::min()
    };

    for ( uint i = 0 ; i < num_vers;i++ )
    {

      for ( uint k = 0 ; k < 3;k++ )
      {
        double p = gl_get_num_val<double>
                   ( v_buf->get_item_comp_ptr ( i, k ), v_buf->src_type() );

        extent[k*2+0] = std::min ( extent[k*2+0], p );
        extent[k*2+1] = std::max ( extent[k*2+1], p );
      }
    }

    std::copy ( extent, extent + 6, extent_out );
  }

  void compute_extent ( const vertex_list_t &v, double * extent_out)
  {
    double extent[] =
    {
      std::numeric_limits<double>::max(), std::numeric_limits<double>::min(),
      std::numeric_limits<double>::max(), std::numeric_limits<double>::min(),
      std::numeric_limits<double>::max(), std::numeric_limits<double>::min()
    };

    for ( uint i = 0 ; i < v.size();i++ )
    {

      for ( uint k = 0 ; k < 3;k++ )
      {
        extent[k*2+0] = std::min ( extent[k*2+0], v[i][k] );
        extent[k*2+1] = std::max ( extent[k*2+1], v[i][k]  );
      }
    }

    std::copy ( extent, extent + 6, extent_out );
  }

  void read_off_file ( const char *filename, vertex_list_t  &vlist,tri_idx_list_t &tlist)
  {

    std::fstream offFile ( filename, std::ios::in );

    if ( offFile.is_open() == false )
      throw std::runtime_error( "cannot read the file" );

    std::string line;

    getline ( offFile, line );

    line = stripWS ( line );

    if ( line != "OFF" )
      throw std::runtime_error ( "Doesnt appear to be an OFF file" );

    getline ( offFile, line );

    std::stringstream linestream ( stripWS ( line ) );

    uint num_verts,num_tris;

    linestream >> num_verts >> num_tris;

    vlist.resize(num_verts);

    tlist.resize(num_tris);

    uint vpos = 0 ;

    while ( !offFile.eof() )
    {
      if ( vpos == num_verts )
        break;

      getline ( offFile, line );

      std::stringstream linestream ( stripWS ( line ) );

      linestream >> vlist[vpos][0] >>vlist[vpos][1] >> vlist[vpos][2];

      ++vpos;
    }

    if ( num_verts != vpos )
      throw std::runtime_error ( "incorrect num verts read" );

    uint tpos = 0 ;

    while ( !offFile.eof() )
    {
      if ( tpos == num_tris )
        break;

      getline ( offFile, line );

      uint num_points;

      std::stringstream linestream ( stripWS ( line ) );

      linestream >> num_points;

      linestream >> tlist[tpos][0] >>tlist[tpos][1] >> tlist[tpos][2];

      if(num_points != 3)
        throw std::runtime_error ( "can read triangles only" );

      ++tpos;
    }

    if ( num_tris != tpos )
      throw std::runtime_error ( "incorrect num tris read" );
  }

  void read_tri_file
      ( const char *filename,
        vertex_list_t  &vlist,
        tri_idx_list_t &tlist)
  {
    uint num_v,num_t;

    std::fstream tri_file ( filename, std::fstream::in );

    if(tri_file.is_open() == false)
      throw std::runtime_error("unable to open tri file");

    tri_file >> num_v >> num_t;

    vlist.resize(num_v);
    tlist.resize(num_t);

    for ( uint i = 0; i < num_v; ++i )
      for ( uint j = 0; j < 3; ++j )
        tri_file >> vlist[i][j];

    for ( uint i = 0; i < num_t; i++ )
      for ( uint j = 0; j < 3; ++j )
        tri_file >> tlist[i][j];

    tri_file.close();
  }


  renderable_t * create_buffered_ren_off_file ( const char * filename , bool use_strips )
  {

    vertex_list_t  vlist;
    tri_idx_list_t tlist;

    glutils::read_off_file ( filename, vlist,tlist );

    bufobj_ptr_t v = make_buf_obj(vlist);
    bufobj_ptr_t t = make_buf_obj(tlist);
    bufobj_ptr_t c = make_buf_obj();

    if ( use_strips )
      return  create_buffered_tristrip_ren ( v,t,c);
    else
      return create_buffered_triangles_ren ( v,t,c);

  }

  void init()
  {
    glewInit();

    if ( glewIsSupported ( "GL_VERSION_2_0" ) )
    {
      if ( ! ( GLEW_ARB_vertex_shader &&
               GLEW_ARB_fragment_shader &&
               GL_EXT_geometry_shader4 &&
               GL_TEXTURE_RECTANGLE_ARB ) )
      {
        _ERROR ( "Some extensions are not supported" );
      }

      if ( ! glewIsSupported ( "GL_ARB_pixel_buffer_object" ) )
      {
        _ERROR ( "ERROR: Support for necessary OpenGL extensions missing." );
      }
    }
    else
    {
      _ERROR ( "No support for opengl 2.0" );
    }

    init_text_rendering();
  }

  void draw_aabb_line(vertex_t lc,vertex_t uc)
  {

    vertex_t cube_pts []=
    {
      vertex_t(lc[0],lc[1],lc[2]),
      vertex_t(uc[0],lc[1],lc[2]),
      vertex_t(lc[0],uc[1],lc[2]),
      vertex_t(uc[0],uc[1],lc[2]),
      vertex_t(lc[0],lc[1],uc[2]),
      vertex_t(uc[0],lc[1],uc[2]),
      vertex_t(lc[0],uc[1],uc[2]),
      vertex_t(uc[0],uc[1],uc[2]),
    };

    glBegin(GL_LINE_LOOP);

    glVertex3dv(cube_pts[0].data());
    glVertex3dv(cube_pts[1].data());
    glVertex3dv(cube_pts[3].data());
    glVertex3dv(cube_pts[2].data());

    glEnd();

    glBegin(GL_LINE_LOOP);

    glVertex3dv(cube_pts[4].data());
    glVertex3dv(cube_pts[5].data());
    glVertex3dv(cube_pts[7].data());
    glVertex3dv(cube_pts[6].data());

    glEnd();

    glBegin(GL_LINES);

    glVertex3dv(cube_pts[0].data());
    glVertex3dv(cube_pts[4].data());

    glVertex3dv(cube_pts[1].data());
    glVertex3dv(cube_pts[5].data());

    glVertex3dv(cube_pts[2].data());
    glVertex3dv(cube_pts[6].data());

    glVertex3dv(cube_pts[3].data());
    glVertex3dv(cube_pts[7].data());

    glEnd();

  }
}
