#version 410
in vec2 texCoord;
uniform sampler2D texBuff;

uniform vec3 lightPosKey;
uniform vec3 lightPosFill;
uniform vec3 lightPosBack;

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
    vec3 lightColor = vec3(1.0, 1.0, 1.0);
    vec4 objectColor = texture(texBuff, texCoord);
    vec3 N = normalize(vNormal);
    vec3 V = normalize(camPos - vec3(fragPos));

    vec3 result = vec3(0.0);

    // Função para calcular iluminação
    for(int i = 0; i < 3; ++i) {
        vec3 L;
        if (i == 0 && length(lightPosKey) > 0.0001) {
            L = normalize(lightPosKey - vec3(fragPos));
        }
        else if (i == 1 && length(lightPosFill) > 0.0001) {
            L = normalize(lightPosFill - vec3(fragPos));
        }
        else if (length(lightPosBack) > 0.0001) {
            L = normalize(lightPosBack - vec3(fragPos));
        }

        // Difusa
        float diff = max(dot(N, L), 0.0);
        vec3 diffuse = kd * diff * lightColor;

        // Especular
        vec3 R = reflect(-L, N);
        float spec = pow(max(dot(R, V), 0.0), q);
        vec3 specular = ks * spec * lightColor;

        // Ambiente
        vec3 ambient = ka * lightColor;

        // Acumula resultado
        result += (ambient + diffuse) * vec3(objectColor) + specular;
    }

    color = vec4(result, 1.0);
}
