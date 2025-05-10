#version 410
 in vec2 texCoord;
 uniform sampler2D texBuff;
 uniform vec3 lightPos;
 uniform vec3 camPos;
 uniform float ka;
 uniform float kd;
 uniform float ks;
 uniform float q;
 out vec4 color;
 in vec4 fragPos;
 in vec3 vNormal;
 void main()
 {
 
     vec3 lightColor = vec3(1.0,1.0,1.0);
     vec4 objectColor = texture(texBuff,texCoord);
    //  vec4 objectColor = vColor;
 
     //Coeficiente de luz ambiente
     vec3 ambient = ka * lightColor;
 
     //Coeficiente de reflexão difusa
     vec3 N = normalize(vNormal);
     vec3 L = normalize(lightPos - vec3(fragPos));
     float diff = max(dot(N, L),0.0);
     vec3 diffuse = kd * diff * lightColor;
 
     //Coeficiente de reflexão especular
     vec3 R = normalize(reflect(-L,N));
     vec3 V = normalize(camPos - vec3(fragPos));
     float spec = max(dot(R,V),0.0);
     spec = pow(spec,q);
     vec3 specular = ks * spec * lightColor; 
 
     vec3 result = (ambient + diffuse) * vec3(objectColor) + specular;
     color = vec4(result,1.0);
 
 }
