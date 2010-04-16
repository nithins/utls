#version 120
#extension GL_EXT_geometry_shader4 : enable


void main()
{

  vec4 diffuse    = gl_LightSource[0].diffuse;
  vec4 ambient    = gl_LightSource[0].ambient;
  ambient        += gl_LightModel.ambient ;
  vec3 lightDir   = normalize(vec3(gl_LightSource[0].position));
  vec3 halfVector = normalize(gl_LightSource[0].halfVector.xyz);

  for(int i=2; i< gl_VerticesIn; i+=3)
  {
    vec4 p1 = gl_PositionIn[i-2];
    vec4 p2 = gl_PositionIn[i-1];
    vec4 p3 = gl_PositionIn[i-0];

    vec3 v1 = (p1.xyz-p2.xyz);
    vec3 v2 = (p1.xyz-p3.xyz);

    vec3 n  = normalize(gl_NormalMatrix*(cross(v1,v2)));

    vec3 halfV,viewV,ldir;
    float NdotL,NdotHV;
    vec4  color  = ambient;


    NdotL = max(dot(n,lightDir),0.0);

    if (NdotL > 0.0) 
    {
      halfV = normalize(halfVector);
      NdotHV = max(dot(n,halfV),0.0);
      if(NdotHV > 0.0)
	color += gl_FrontMaterial.specular * 
	         gl_LightSource[0].specular *
	         pow(NdotHV,gl_FrontMaterial.shininess);
      color += diffuse * NdotL;
    }

    gl_FrontColor = gl_FrontColorIn[i]*color;
 
    gl_Position = gl_ModelViewProjectionMatrix*p1;
    EmitVertex();
    gl_Position = gl_ModelViewProjectionMatrix*p2;
    EmitVertex();
    gl_Position = gl_ModelViewProjectionMatrix*p3;
    EmitVertex();

    EndPrimitive();
  }
}