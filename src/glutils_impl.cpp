#include <fstream>
#include <stdexcept>

#include <GL/glew.h>
#include <boost/algorithm/string.hpp>

#include <glutils_impl.h>
#include <cpputils.h>
#include <logutil.h>
#include <vecutils.h>
#include <limits>
#include <numeric>

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
                      glutils::buf_obj_t::create_bo(),
                      glutils::buf_obj_t::create_bo());

  }

  s_z_arrow_ren->render();
}

namespace glutils
{
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

  vertex_t compute_center ( const vertex_list_t &v)
  {
    vertex_t center;

    center[0] = 0;
    center[1] = 0;
    center[2] = 0;

    for ( uint i = 0 ; i < v.size();i++ )
    {
      center += v[i];
    }

    return center/double(v.size());
  }


  void read_off_file ( const char *filename, vertex_list_t  &vlist,tri_idx_list_t &tlist)
  {
    using namespace std;
    namespace ba    = boost::algorithm;

    fstream off_file ( filename, fstream::in);

    ensure(off_file.is_open(),"unable to open off file");

    string ln;
    std::vector<string> strs;

    getline(off_file,ln);
    ensure(ln=="OFF","Doesn't seem to be an OFF FILE");

    getline(off_file,ln);
    ba::split(strs,ln,ba::is_any_of("\t \n"));

    int num_v = atoi(strs[0].c_str());
    int num_t = atoi(strs[1].c_str());

    vlist.resize(num_v);
    tlist.resize(num_t);

    for ( uint i = 0; i < num_v; ++i )
    {
      getline(off_file,ln);
      ba::split(strs,ln,ba::is_any_of("\t \n"));

      vlist[i][0] = atof(strs[0].c_str());
      vlist[i][1] = atof(strs[1].c_str());
      vlist[i][2] = atof(strs[2].c_str());
    }

    for ( uint i = 0; i < num_t; i++ )
    {
      getline(off_file,ln);
      ba::split(strs,ln,ba::is_any_of("\t \n"));

      int ntv     = atoi(strs[0].c_str());
      tlist[i][0] = atoi(strs[1].c_str());
      tlist[i][1] = atoi(strs[2].c_str());
      tlist[i][2] = atoi(strs[3].c_str());

      ensure(ntv == 3,"Mesh contains non-triangle polys");
      ensure(is_in_range(tlist[i][0],0,num_v),"invalid index in file");
      ensure(is_in_range(tlist[i][1],0,num_v),"invalid index in file");
      ensure(is_in_range(tlist[i][2],0,num_v),"invalid index in file");
    }

    off_file.close();
  }

  void read_tri_file
      ( const char *filename,
        vertex_list_t  &vlist)
  {
    uint num_v,num_t;

    std::fstream tri_file ( filename, std::fstream::in );

    if(tri_file.is_open() == false)
      throw std::runtime_error("unable to open tri file");

    tri_file >> num_v >> num_t;

    vlist.resize(num_v);

    for ( uint i = 0; i < num_v; ++i )
      for ( uint j = 0; j < 3; ++j )
        tri_file >> vlist[i][j];

    tri_file.close();
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
      {
        tri_file >> tlist[i][j];

        if(tlist[i][j] >= num_v||tlist[i][j] < 0)
        {
          throw std::runtime_error("invalid index");
        }
      }

    tri_file.close();
  }


  renderable_t * create_buffered_ren_off_file ( const char * filename , bool use_strips )
  {

    vertex_list_t  vlist;
    tri_idx_list_t tlist;

    glutils::read_off_file ( filename, vlist,tlist );

    bufobj_ptr_t v = make_buf_obj(vlist);
    bufobj_ptr_t t = make_buf_obj(tlist);
    bufobj_ptr_t n = make_normals_buf_obj(vlist,tlist);

//    if ( use_strips )
//      return  create_buffered_tristrip_ren ( v,t,c);
//    else
      return create_buffered_triangles_ren ( v,t,n);

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

#ifdef UTLS_ENABLE_TEXTREN
    init_text_rendering();
#endif

#ifdef UTLS_GEN_SHADERS

    init_normal_renderer();

    init_common_shaders();
#endif
  }

  void clear()
  {
#ifdef UTLS_GEN_SHADERS
    clear_normal_renderer();
#endif
  }

  void draw_aabb_line(vertex_t lc,vertex_t uc)
  {

    vertex_t cube_pts []=
    {
      la::make_vec(lc[0],lc[1],lc[2]),
      la::make_vec(uc[0],lc[1],lc[2]),
      la::make_vec(lc[0],uc[1],lc[2]),
      la::make_vec(uc[0],uc[1],lc[2]),
      la::make_vec(lc[0],lc[1],uc[2]),
      la::make_vec(uc[0],lc[1],uc[2]),
      la::make_vec(lc[0],uc[1],uc[2]),
      la::make_vec(uc[0],uc[1],uc[2]),
    };

    glBegin(GL_LINE_LOOP);

    glVertex3dv(&(cube_pts[0][0]));
    glVertex3dv(&(cube_pts[1][0]));
    glVertex3dv(&(cube_pts[3][0]));
    glVertex3dv(&(cube_pts[2][0]));

    glEnd();

    glBegin(GL_LINE_LOOP);

    glVertex3dv(&(cube_pts[4][0]));
    glVertex3dv(&(cube_pts[5][0]));
    glVertex3dv(&(cube_pts[7][0]));
    glVertex3dv(&(cube_pts[6][0]));

    glEnd();

    glBegin(GL_LINES);

    glVertex3dv(&(cube_pts[0][0]));
    glVertex3dv(&(cube_pts[4][0]));

    glVertex3dv(&(cube_pts[1][0]));
    glVertex3dv(&(cube_pts[5][0]));

    glVertex3dv(&(cube_pts[2][0]));
    glVertex3dv(&(cube_pts[6][0]));

    glVertex3dv(&(cube_pts[3][0]));
    glVertex3dv(&(cube_pts[7][0]));

    glEnd();

  }


void smooth_lines(vertex_list_t &vl, const line_idx_list_t &ll,int NITER)
{
  using namespace std;

  int V = vl.size();
  int E = ll.size();

  vector<int> off(V+1,0);

  for(int i = 0 ; i < E; ++i) {off[ll[i][0]] += 1;off[ll[i][1]] += 1;}

  partial_sum(off.begin(),off.end(),off.begin());

  vector<int> adj(off[V-1],-1);

  for(int i = 0 ; i < E; ++i)
  {int u = ll[i][0],v = ll[i][1]; adj[--off[u]] = v;adj[--off[v]] = u;}

  off[V] = adj.size();

  vertex_list_t out_vl(V);

  for( int iteration = 0 ; iteration < NITER; ++iteration)
  {
  for( int i = 0 ; i < V; ++i)
  {
    out_vl[i] = vl[i];

    int n = off[i+1] - off[i];

    if (n != 2) continue;

    for(int b = off[i],e = off[i +1]; b!=e  ;++b)
      out_vl[i] += vl[adj[b]];

    out_vl[i] /= (n+1);
  }
  copy(out_vl.begin(),out_vl.end(),vl.begin());
  }

  return;
}
}
