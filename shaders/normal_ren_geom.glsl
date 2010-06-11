#version 120
#extension GL_EXT_geometry_shader4 : enable         
 
varying in vec4 normal_position[];      
 
void main()
{
  for(int i = 0; i < gl_VerticesIn; ++i)
  {
    if(dot(vec3(0,0,1),(gl_PositionIn[i] - normal_position[i]).xyz) < 0.0)
      continue;

    gl_Position   = gl_PositionIn[i];
    gl_FrontColor = gl_FrontColorIn[i];
    EmitVertex();      

    gl_Position   = normal_position[i];
    gl_FrontColor = gl_FrontColorIn[i];
    EmitVertex();      

    EndPrimitive();
  }
}
