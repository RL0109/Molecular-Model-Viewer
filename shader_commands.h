const char* instancingVertexShader = R"(
#version 330
in vec3 vertexPosition;
in vec2 vertexTexCoord;
in vec3 vertexNormal;
in vec4 vertexColor;
in mat4 instanceTransform; 
uniform mat4 mvp;

out vec3 fragPosition;
out vec2 fragTexCoord;
out vec4 fragColor;
out vec3 fragNormal;

void main() {
    vec4 wPos = instanceTransform * vec4(vertexPosition, 1.0);
    fragPosition = wPos.xyz;
    fragTexCoord = vertexTexCoord;
    fragColor = vertexColor;
    
    mat3 normalMatrix = transpose(inverse(mat3(instanceTransform)));
    fragNormal = normalize(normalMatrix * vertexNormal);

    gl_Position = mvp * wPos;
}
)";

// 2. EMBED THE FRAGMENT SHADER
const char* instancingFragmentShader = R"(
#version 330
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

uniform vec4 colDiffuse;
out vec4 finalColor;

void main() {
    vec3 lightDir = normalize(vec3(0.5, 1.0, 1.0));
    float light = max(dot(normalize(fragNormal), lightDir), 0.3);
    finalColor = colDiffuse * vec4(vec3(light), 1.0);
}
)";