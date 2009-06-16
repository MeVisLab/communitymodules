varying vec3 vPosition;
varying vec3 vNormal;
varying float lightIntensity;

uniform float Cubesize;

const float specularTerm = 0.2 ;
const float diffuseTerm  = 1.0 - specularTerm;

void main(void)
{
   // da betrachter in (0,0,0) ist Oberflächenpunkt auch vektor 
   //vec3 eyeVertexPosition = vec3 (gl_ModelViewMatrix * gl_Vertex);
   vec3 eyeSpacePos   = vec3(gl_ModelViewMatrix * gl_Vertex);
   vec3 transNormal  = normalize(gl_NormalMatrix * gl_Normal);
   vPosition = vec3(gl_Vertex);
   vNormal = vec3 (normalize(gl_Normal)); 
   
   //Beleuchtungsberechnungen
   //lightVec = Vektor vom Vertex zur Lichtquelle
   //reflectVec = reflektierter Vektor
   //viewVec = Vektor vom Betrachter zu vPosition
   vec3 lightVec = normalize(vec3(gl_LightSource[0].position) - eyeSpacePos);
   vec3 reflectVec   = reflect(-lightVec, transNormal);
   vec3 viewVec      = normalize(- eyeSpacePos);
   float diffuse     = max(dot(lightVec, transNormal), 0.0);
   float spec = 0.0;
   
   if(diffuse > 0.0){
      spec = max(dot(reflectVec, viewVec), 0.0);
      spec = pow(spec, 8.0);
   }
   
   lightIntensity = diffuseTerm * diffuse + specularTerm * spec;   
   
   //lightIntensity = max(dot(lightVec, transNormal), 0.0);
   
   
   gl_Position = ftransform(); 
 
 }