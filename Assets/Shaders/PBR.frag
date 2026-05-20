#version 460 core

layout(location = 0) in vec3 in_Pos;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;

uniform bool u_UseAlbedoTextures;
uniform bool u_UseNormalMaps;

uniform vec3 u_CameraPosition;

uniform vec4 u_AlbedoColor;
uniform sampler2D u_AlbedoTexture;

/// 0 → 1 = Dielectric → Metal
uniform float u_MetallicFactor;
/// 0 → 1 = Mirror → Matte
uniform float u_RoughnessFactor;
/// R: unused | G: roughness | B: metallic
uniform sampler2D u_MetallicRoughnessTexture;

uniform sampler2D u_NormalMap;

uniform vec3 u_EmissiveColor;
uniform sampler2D u_EmissiveTexture;

out vec4 out_Color;
out vec4 out_Normal;

vec3 computeNormal()
{
    vec3 N = normalize(in_Normal);

    vec3 dp1  = dFdx(in_Pos);
    vec3 dp2  = dFdy(in_Pos);
    vec2 duv1 = dFdx(in_UV);
    vec2 duv2 = dFdy(in_UV);

    float det = duv1.x * duv2.y - duv1.y * duv2.x;
    if (abs(det) < 1e-5)
        return N;

    float invDet = 1.0 / det;

    vec3 T   = normalize((dp1 * duv2.y - dp2 * duv1.y) * invDet);
    vec3 B = normalize((dp2 * duv1.x - dp1 * duv2.x) * invDet);

    T = normalize(T - dot(T, N) * N);
    B = cross(N, T);

    if (dot(cross(T, B), N) < 0.0)
        T = -T;

    mat3 TBN = mat3(T, B, N);

    vec3 normalMap = u_UseNormalMaps ? texture(u_NormalMap, in_UV).rgb : vec3(0.0, 0.0, 1.0);
    normalMap = normalize(normalMap * 2.0 - 1.0);

    return normalize(TBN * normalMap);
}

void main()
{
    // Albedo
    vec4 albedoWithAlpha = (u_UseAlbedoTextures ? texture(u_AlbedoTexture, in_UV) : vec4(1.0)) * u_AlbedoColor;
    vec3 albedo = albedoWithAlpha.rgb;

    // Metallic / Roughness
    vec2 mr = texture(u_MetallicRoughnessTexture, in_UV).gb;
    float roughness = max(mr.r * u_RoughnessFactor, 0.04);
    float metallic  =     mr.g * u_MetallicFactor;

    // Emissive
    vec3 emissive = texture(u_EmissiveTexture, in_UV).rgb * u_EmissiveColor;

    // Lights
    vec3 N = computeNormal();
    vec3 V = normalize(u_CameraPosition - in_Pos);
    
    vec3 F0 = mix(vec3(0.04), albedo, metallic);
    vec3 Lo = vec3(0.0);

    // - Directional
    Lo += directionaLight(u_DirectionalLightDirection, u_DirectionalLightColor, V, N, albedo, roughness, metallic, F0);

    // - Points
    for (int i = 0; i < u_PointLightCount; i++)
    {
        PointLight light = u_PointLights[i];
        Lo += pointLight(light, in_Pos, V, N, albedo, roughness, metallic, F0);
    }

    // Ambient
    vec3 R = reflect(-V, N);
    vec3 equiR = vec3(R.x, R.z, -R.y);
    float lod = roughness * 10.0;
    vec3 envColor = sampleEnvironmentMapLOD(equiR, lod).rgb;

    vec3 F_amb = CalculateSchlickFresnelRoughness(saturate(dot(N, V)), F0, roughness);
    vec3 kD_amb = (1.0 - F_amb) * (1.0 - metallic);

    vec3 ambient_diffuse = kD_amb * albedo * u_AmbientLight;
    vec3 ambient_specular = F_amb * envColor * (1.0 - roughness);
    vec3 ambient = ambient_diffuse + ambient_specular;

    // Composition
    vec3 color = ambient + Lo + emissive;

    out_Color = vec4(toneMapping(color), albedoWithAlpha.a);
    out_Normal = vec4(N, 0.0);
}
