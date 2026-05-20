#version 460 core

layout(location = 0) in vec4 in_Color;
layout(location = 1) flat in int in_IsEmissive;
layout(location = 2) in vec3 in_Pos;
layout(location = 3) in vec3 in_ParticleCenter;
layout(location = 4) in vec2 in_LocalPos;

uniform vec3 u_CameraPosition;
uniform vec3 u_WorldUp;

out vec4 out_Color;
out vec4 out_Normal;

vec3 sampleAverageSkyColor()
{
    const vec3 dirs[6] = vec3[6](
        vec3( 1.0,  0.0,  0.0),
        vec3(-1.0,  0.0,  0.0),
        vec3( 0.0,  1.0,  0.0),
        vec3( 0.0, -1.0,  0.0),
        vec3( 0.0,  0.0,  1.0),
        vec3( 0.0,  0.0, -1.0)
    );

    vec3 avg = vec3(0.0);
    for (int i = 0; i < 6; ++i)
    {
        avg += sampleEnvironmentMap(dirs[i]).rgb;
    }
    return avg / 6.0;
}

void main()
{
    vec3 baseColor = in_Color.rgb;

    // Fake sphere shading in billboard space.
    vec2 sphereXY = in_LocalPos;
    float r2 = dot(sphereXY, sphereXY);
    if (r2 > 1.0)
        discard;

    float z = sqrt(max(0.0, 1.0 - r2));

    vec3 toCamera = normalize(u_CameraPosition - in_ParticleCenter);
    vec3 upAxis = (abs(dot(toCamera, u_WorldUp)) > 0.999) ? vec3(1.0, 0.0, 0.0) : u_WorldUp;
    vec3 right = normalize(cross(toCamera, upAxis));
    vec3 up = cross(right, toCamera);

    vec3 N = normalize(sphereXY.x * right + sphereXY.y * up + z * toCamera);

    if (in_IsEmissive != 0)
    {
        out_Color = vec4(baseColor, in_Color.a);
        out_Normal = vec4(N, 0.0);
        return;
    }
    
    // Ambient only (no diffuse/specular direct lights), from sky average.
    vec3 color = baseColor * sampleAverageSkyColor();

    out_Color = vec4(toneMapping(color), in_Color.a);
    out_Normal = vec4(N, 0.0);
}
