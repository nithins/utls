#version 120
#extension GL_EXT_geometry_shader4 : enable

#define NUM_LIGHTS 2

void DirectionalLight( in int i,
           in vec3 normal,
           inout vec4 ambient,
           inout vec4 diffuse,
           inout vec4 specular)
{
  float nDotVP;
  float nDotHV;
  float pf;
  
  nDotVP = max(0.0,dot(normal,normalize(vec3(gl_LightSource[i].position))));
  nDotHV = max(0.0,dot(normal,normalize(vec3(gl_LightSource[i].halfVector))));
  
  if(nDotVP == 0.0)
    pf = 0.0;
  else
    pf = pow(nDotHV,gl_FrontMaterial.shininess);
  
  ambient  += gl_LightSource[i].ambient;
  diffuse  += gl_LightSource[i].diffuse*nDotVP;
  specular += gl_LightSource[i].specular*pf;
}

void PointLight(in int i,
    in vec3 eye,
    in vec3 ecPosition3,
    in vec3 normal,
    inout vec4 ambient,
    inout vec4 diffuse,
    inout vec4 specular)
{
  float nDotVP;
  float nDotHV;
  float pf;
  float attenuation;
  float d;
  vec3  VP;
  vec3  halfVector;
  
  VP = vec3(gl_LightSource[i].position)-ecPosition3;
  
  d  = length(VP);
  
  VP = normalize(VP);
  
  attenuation = 1.0/(gl_LightSource[i].constantAttenuation +
         gl_LightSource[i].linearAttenuation*d+
         gl_LightSource[i].quadraticAttenuation*d*d);
         
  halfVector = normalize(VP+eye);
  
  nDotVP = max(0.0,dot(normal,VP));
  nDotHV = max(0.0,dot(normal,halfVector));
  
  if(nDotVP == 0.0)
    pf = 0.0;
  else
    pf = pow(nDotHV,gl_FrontMaterial.shininess);
  
  ambient  += gl_LightSource[i].ambient*attenuation;
  diffuse  += gl_LightSource[i].diffuse*nDotVP*attenuation;
  specular += gl_LightSource[i].specular*pf*attenuation;  
}

void main()
{
  for(int i=2; i< gl_VerticesIn; i+=3)
  {
    vec4 p1 = gl_PositionIn[i-2];
    vec4 p2 = gl_PositionIn[i-1];
    vec4 p3 = gl_PositionIn[i-0];

    vec3 v1 = (p1.xyz-p2.xyz);
    vec3 v2 = (p1.xyz-p3.xyz);

    vec3 n  = normalize(gl_NormalMatrix*(cross(v1,v2)));

    if(dot(n,vec3(0,0,1))<0) n = -n;

    vec4 amb  = vec4(0.0);
    vec4 diff = vec4(0.0);
    vec4 spec = vec4(0.0);
    
    for(int j = 0 ; j < NUM_LIGHTS; ++j)
    {
      if(gl_LightSource[j].position.w == 0.0)
        DirectionalLight(j,n,amb,diff,spec);
      else
        PointLight(j,vec3(0,0,0),((p1+p2+p3)/3).xyz,n,amb,diff,spec);
    }
    
    vec4 color = vec4(0);
    
    color +=  amb*gl_FrontColorIn[i];
    color += diff*gl_FrontColorIn[i];
    color += spec*gl_FrontMaterial.specular;

    gl_FrontColor = color;
 
    gl_Position = gl_ModelViewProjectionMatrix*p1;
    EmitVertex();
    gl_Position = gl_ModelViewProjectionMatrix*p2;
    EmitVertex();
    gl_Position = gl_ModelViewProjectionMatrix*p3;
    EmitVertex();

    EndPrimitive();
  }
}