#version 460 core

layout(location = 0) in vec3 in_WorldDir;

uniform bool u_UseAlbedoTextures;
uniform sampler2D u_EnvironmentMap;

out vec4 out_Color;
out vec4 out_Normal;

void main()
{
    vec3 dir = in_WorldDir;
    vec3 equirectDir = vec3(dir.x, dir.z, -dir.y);
    vec4 color = u_UseAlbedoTextures ? texture(u_EnvironmentMap, sampleEquirect(equirectDir)) : vec4(1.0);
    out_Color = vec4(toneMapping(color.rgb), color.a);
    out_Normal = vec4(0.0);
}
