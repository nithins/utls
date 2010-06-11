varying vec4 normal_position;

uniform float normal_length;      
 
void main(void)
{
  gl_Position     = gl_ModelViewProjectionMatrix * gl_Vertex;
  normal_position = gl_ModelViewProjectionMatrix * (gl_Vertex + vec4(gl_Normal,0)*normal_length);
  gl_FrontColor   = gl_Color;
}