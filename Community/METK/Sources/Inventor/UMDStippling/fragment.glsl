varying vec3 vPosition;
varying vec3 vNormal;
varying float lightIntensity;

uniform float Cubesize;
uniform float Gamma;
uniform float Transparency;
uniform vec3  Color;
uniform vec3  PointColor;

uniform sampler2D Texture0;

/* RP: uniforms, die von SoShadingMap gesetzt werden
 * shadingMapSize - aktuelle Größe der Shadingtextur
 * shadingMap - Sampler mit der Shadingtextur
 * (Name muss auch im Interface von SoShadingMap gesetzt werden)
 */
uniform vec2 shadingMapSize;
uniform sampler2D shadingMap;

vec3 positionInCube(vec3 pos_, float size_){

	vec3 newPos_ = vec3(0.0,0.0,0.0);
      	newPos_.x = pos_.x - size_ * floor(pos_.x/size_);
      	newPos_.y = pos_.y - size_ * floor(pos_.y/size_);
   	newPos_.z = pos_.z - size_ * floor(pos_.z/size_);
        
        return newPos_;
         
}

//Schnittpunkt von der Normalen mit einer achsenparallelen Ebene 
//und daraus die Texturkoordinaten berechnen
vec3 TextureCoordinates(vec3 position,vec3 Cubemin, vec3 Cubemax, vec3 planeNormal_,float Cubeside_)
{
	vec3 planePoint = vec3(0.0,0.0,0.0);
	vec3 textureIndex = vec3(0.0,0.0,0.0);
      	vec3 textureCoords = vec3(0.0,0.0,0.0);
   
      //Punkt auf der Ebene = Cubemax wenn normale in richtung positiver seite zeigt, 
      //ansonsten cubemin
      	if(Cubeside_ >= 0.0)
      	{
            	planePoint = Cubemax;
      
      	}else{
      		planeNormal_ = planeNormal_ * vec3(-1.0,-1.0,-1.0);
              	planePoint = Cubemin;
         }
           
      	//vec3 D =  planePoint * planeNormal;
      	vec3 D =  planePoint * planeNormal_;
   
      	// t = d - Position * nEbene / (vNormal) * nEbene
      	// Cubeside gibt hier die Komponente des Vektors an (vNormal.x , y oder z),
      	// die bei der Multiplikation mit der Ebenennormalen übrig bleibt
      	float divisor = Cubeside_;
      	vec3 k = (D - (position * planeNormal_))/ divisor; 

      	// Schnittpunkt berechnen mit entsprechendem t in geradengleichung einsetzten
      	vec3 iPoint_ = position + k * vNormal;
       
      	//TexCoord bestimmen
      	textureIndex.x = ((iPoint_.x - Cubemin.x)/ (Cubemax.x - Cubemin.x))/ Cubemax.x;
      	textureIndex.y = ((iPoint_.y - Cubemin.y)/ (Cubemax.y - Cubemin.y))/ Cubemax.y;
      	textureIndex.z = ((iPoint_.z - Cubemin.z)/ (Cubemax.z - Cubemin.z))/ Cubemax.z;  
   
      	textureCoords.xyz = textureIndex;
        textureCoords.x = textureIndex.x * Cubesize;
        textureCoords.y = textureIndex.y * Cubesize;
        textureCoords.z = textureIndex.z * Cubesize;
      
      	return textureCoords;
}

void main(void)
{

   //Texturierung----------------------------------------------------------------
    
    //Textur-Würfelgrenzen
      	vec3 Cubemin = vec3(0.0,0.0,0.0);
      	vec3 Cubemax = vec3(Cubesize,Cubesize,Cubesize);
      
      	//variablendeklaration
      	vec3 texCoord = vec3(0.0,0.0,0.0);
      	vec3 posInCube = positionInCube(vPosition, Cubesize);

      	//Anhand des größten Wertes des Normalenvektors wird die zu betrachtende Seite bestimmt
      	float maxWert = max(abs(vNormal.x), abs(vNormal.y));
      	maxWert = max(maxWert, abs(vNormal.z));
   
   
      	//Unterscheidung welche Seite betroffen ist anhand der Richtung der Normalen
      	//dann Funkion zur Berechnung der TexCoord aufrufen
      	if (maxWert == abs(vNormal.x))
      	{
      		texCoord.st = TextureCoordinates(posInCube, Cubemin, Cubemax, vec3(1.0,0.0,0.0), vNormal.x).yz;
     
         }else if (maxWert == abs(vNormal.y)){

      		texCoord.st = TextureCoordinates(posInCube, Cubemin, Cubemax, vec3(0.0,1.0,0.0), vNormal.y).xz;
      
         }else{
               texCoord.st = TextureCoordinates(posInCube, Cubemin, Cubemax, vec3(0.0,0.0,1.0), (vNormal.z *(-1.0))).xy;
         } 
         
         //Texturwerte
         
         // Verwendung einer einzelnen Textur
         vec4 Texel = texture2D(Texture0,texCoord.st);
         //gl_FragColor = Texel;
         
         /*if (lightIntensity <0.4){
         	lightIntensity = lightIntensity + 0.07;
         }*/
         
         /* RP: Falls Modul SoShadingMap angeschlossen ist (shadingMapSize größer 0),
          * wird Helligkeitswert aus der Shadingtextur gelesen und 
          * zur Berechnung der Pixelfarbe genommen,
          * ansonsten die vorher berechnete lightIntensity
          */
		 float referenceIntensity;
		 if (shadingMapSize.x > 0.0) {
			vec2 shadingTexCoord = gl_FragCoord.xy/shadingMapSize;
			float mapIntensity = texture2D(shadingMap, shadingTexCoord)[0];
			referenceIntensity = mapIntensity;
		 } else {
			referenceIntensity = lightIntensity;
		 }
         
          // Verschiedene Helligkeiten in einer Textur
          //Gammakorrektur mit lightIntensity
         if(Texel.x >= pow(referenceIntensity ,Gamma))
         {
            gl_FragColor = vec4(PointColor.r, PointColor.g, PointColor.b, 1.0);
         }else{
            gl_FragColor = vec4(Color.r, Color.g, Color.b, Transparency);
         }
         
	
	//zusätzliche Darstellung der Würfel auf der Oberfläche
	/*vec3 newPosition = positionInCube(vPosition, Cubesize);

	if((newPosition.x < 0.01)&&(newPosition.x > 0.0))
	{
		gl_FragColor.xyz = vec3(1.0,0.0,0.0);
	 }
	 if((newPosition.y < 0.01)&&(newPosition.y > 0.0))
	 {
		gl_FragColor.xyz = vec3(0.0,1.0,0.0);
	 }           
	 if((newPosition.z < 0.01)&&(newPosition.z > 0.0))
	 {
		gl_FragColor.xyz = vec3(0.0,0.0,1.0);
	 }else{
	 	gl_FragColor = Texel;
	 }*/
	         
}