uniform sampler2D u_EnvironmentMap1;
uniform sampler2D u_EnvironmentMap2;
uniform float u_EnvironmentMapBlend;

vec2 sampleEquirect(vec3 dir)
{
    vec3 n = normalize(dir);
    float theta = atan(n.z, n.x);
    float phi = asin(clamp(n.y, -1.0, 1.0));

    float u = 0.5 + theta / (2.0 * PI);
    float v = 0.5 - phi / PI;
    return vec2(u, v);
}

vec4 sampleEnvironmentMap(vec3 equirectDir)
{
    vec2 uv = sampleEquirect(equirectDir);
    vec4 color1 = texture(u_EnvironmentMap1, uv);
    vec4 color2 = texture(u_EnvironmentMap2, uv);
    return mix(color1, color2, u_EnvironmentMapBlend);
}

vec4 sampleEnvironmentMapLOD(vec3 equirectDir, float LOD)
{
    vec2 uv = sampleEquirect(equirectDir);
    vec4 color1 = textureLod(u_EnvironmentMap1, uv, LOD);
    vec4 color2 = textureLod(u_EnvironmentMap2, uv, LOD);
    return mix(color1, color2, u_EnvironmentMapBlend);
}
