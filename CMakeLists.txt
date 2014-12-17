#Define the project we are working on
project(utls)

#minimum cmake version needed
cmake_minimum_required(VERSION 2.8.8)

#tell cmake where to look for the find files.
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# generate a headerfile/sourcefile containing the shader source code as strings
function(files_to_c_variables headerfilename sourcefilename)
  file(WRITE ${headerfilename} "// this is an automatically generated file\n")
  file(WRITE ${sourcefilename} "// this is an automatically generated file\n")

  foreach(i ${ARGN})
    file(READ ${i} file_str)
    string(REPLACE "\n" "\\n\"\\\n\"" file_str "${file_str}")
    get_filename_component(bn ${i} NAME)
    string(REPLACE "." "_" bn "${bn}")
    string(TOUPPER "${bn}" bn)
    file(APPEND ${headerfilename} "extern const char *${bn}; \n\n")
    file(APPEND ${sourcefilename}        "const char *${bn} = \"${file_str}\";\n\n")
  endforeach(i)
  
  set_source_files_properties(${sourcefilename} PROPERTIES OBJECT_DEPENDS "${ARGN}")
endfunction(files_to_c_variables)

find_package(Eigen3 REQUIRED)

include_directories(
  ${EIGEN3_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/src
  )

set(UTLS_SRCS
  ${UTLS_SRCS}

#  ${CMAKE_CURRENT_SOURCE_DIR}/include/tree.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/vecutils.h

  ${CMAKE_CURRENT_SOURCE_DIR}/include/cpputils.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/cpputils.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/n_vector.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/n_vector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/aabb.h

  ${CMAKE_CURRENT_SOURCE_DIR}/include/logutil.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/logutil.cpp

#  ${CMAKE_CURRENT_SOURCE_DIR}/include/quad_edge.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/src/quad_edge.cpp

#  ${CMAKE_CURRENT_SOURCE_DIR}/include/tri_edge.h
#  ${CMAKE_CURRENT_SOURCE_DIR}/src/tri_edge.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/timer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/src/timer.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/configurable.h
  )

option(NO_OPENGL "build without opengl" OFF)

if(NOT NO_OPENGL)
  find_package(GLEW REQUIRED)

#  find_package(Freetype)

#  if(FREETYPE_FOUND)
#    find_package(FTGL)
#  endif(FREETYPE_FOUND)

#  find_package(GLUT)

#  if(NOT FTGL_FOUND AND NOT GLUT_FOUND)
#    message(FATAL_ERROR "Failed to find glut or FTGL libraries.. quitting")
#  endif(NOT FTGL_FOUND AND NOT GLUT_FOUND)

  files_to_c_variables(
    ${CMAKE_CURRENT_BINARY_DIR}/utls_glsl_shaders.h
    ${CMAKE_CURRENT_BINARY_DIR}/utls_glsl_shaders.cpp
    
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/flat_triangles.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/flat_triangles.geom

    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/flat_tetrahedrons.vert
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/flat_tetrahedrons.geom

    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/normal_ren_vert.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/normal_ren_geom.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/normal_ren_frag.glsl

    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cylinder_vert.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cylinder_geom.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/cylinder_frag.glsl

    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/sphere_vert.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/sphere_geom.glsl
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders/sphere_frag.glsl
  )

  set(UTLS_SRCS ${UTLS_SRCS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include/glutils.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_buffer_object.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_line_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_normal_renderer.cpp
#    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_text_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_point_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_triangle_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_tetrahedron_renderer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/glutils_quad_renderer.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/GLSLProgram.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLSLProgram.cpp

    ${CMAKE_CURRENT_SOURCE_DIR}/include/GLSLShader.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/GLSLShader.cpp

    ${CMAKE_CURRENT_BINARY_DIR}/utls_glsl_shaders.h
    ${CMAKE_CURRENT_BINARY_DIR}/utls_glsl_shaders.cpp
    )


  include_directories(
    ${GLEW_INCLUDE_PATH}
    ${FREETYPE_INCLUDE_DIRS}
    ${FTGL_INCLUDE_PATH}
    ${GLUT_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/tristripper
  )

  set(UTLS_LINKLIBS  ${UTLS_LINKLIBS}
    ${GLEW_LIBRARY}
    ${GLUT_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FTGL_LIBRARY}
    )

endif(NOT NO_OPENGL)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/utls_config.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/utls_config.h
  )

set_source_files_properties(${UTLS_SRCS} PROPERTIES COMPILE_FLAGS -fpic)

add_library(utls-objects OBJECT ${UTLS_SRCS})

set(BUILD_UTLS_LIB ON CACHE BOOL "Build the utls shared lib")
if(BUILD_UTLS_LIB)
    add_library(utls SHARED $<TARGET_OBJECTS:utls-objects>)
    target_link_libraries(utls ${UTLS_LINKLIBS})
    install(TARGETS utls DESTINATION lib)
endif(BUILD_UTLS_LIB)


