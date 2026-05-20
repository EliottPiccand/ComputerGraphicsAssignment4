#version 460 core

struct Particle
{
    vec3 position;
    float life;
    vec3 velocity;
    int isSubjectToGravity;
    vec4 color;
    vec2 scale;
    int isEmissive;
};

layout(location = 0) in vec2 in_Pos;

layout(std430, binding = 0) buffer InstanceBuffer
{
    Particle instances[];
};

uniform mat4 u_View;
uniform mat4 u_Projection;
uniform vec3 u_CameraPosition;
uniform vec3 u_WorldUp;

layout(location = 0) out vec4 out_Color;
layout(location = 1) flat out int out_IsEmissive;
layout(location = 2) out vec3 out_Pos;
layout(location = 3) out vec3 out_ParticleCenter;
layout(location = 4) out vec2 out_LocalPos;

void main()
{
    Particle p = instances[gl_InstanceID];

    mat2 scale = mat2(
        p.scale.x,       0.0,
              0.0, p.scale.y
    );
    vec2 pos2D = scale * in_Pos;
    
    vec3 toCamera = normalize(u_CameraPosition - p.position);
    vec3 right = normalize(cross(toCamera, u_WorldUp));
    vec3 up = cross(right, toCamera);
    vec3 worldPos = p.position + (pos2D.x * right) + (pos2D.y * up);

    gl_Position = u_Projection * u_View * vec4(worldPos, 1.0);

    out_Color = p.color;
    out_IsEmissive = p.isEmissive;
    out_Pos = worldPos;
    out_ParticleCenter = p.position;
    out_LocalPos = in_Pos * 2.0;
}
