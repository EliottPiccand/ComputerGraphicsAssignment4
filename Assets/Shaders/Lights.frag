uniform vec3 u_AmbientLight;

uniform vec3 u_DirectionalLightDirection;
uniform vec3 u_DirectionalLightColor;

struct PointLight
{
    vec3 position;
    vec3 color;
    float intensity;
};
uniform int u_PointLightCount;
uniform PointLight u_PointLights[MAX_POINT_LIGHTS];

vec3 directionaLight(vec3 direction, vec3 color, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic, vec3 F0)
{
    vec3 L = normalize(-direction);
    vec3 H = normalize(V + L);

    float NdotL = saturate(dot(N, L));

    float D = CalculateNormalDistributionGGX(roughness, saturate(dot(N, H)));
    float G = CalculateSmithGGXGeometryTerm(roughness, saturate(dot(N, L)), saturate(dot(N, V)));
    vec3  F = CalculateSchlickFresnelReflectance(saturate(dot(H, V)), F0);

    vec3  numerator = D * G * F;
    float denominator = 4.0 * saturate(dot(N, V)) * NdotL + 0.0001;
    vec3  specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);

    vec3 diffuse = kD * albedo / PI;

    vec3 radiance = color;
    return (diffuse + specular) * radiance * NdotL; // Lo
}

vec3 pointLight(PointLight light, vec3 fragmentPosition, vec3 V, vec3 N, vec3 albedo, float roughness,
                float metallic, vec3 F0)
{
    vec3 toLight = light.position - fragmentPosition;
    float distanceToLight = length(toLight);
    vec3 L = toLight / max(distanceToLight, 0.0001);
    vec3 H = normalize(V + L);

    float NdotL = saturate(dot(N, L));

    float D = CalculateNormalDistributionGGX(roughness, saturate(dot(N, H)));
    float G = CalculateSmithGGXGeometryTerm(roughness, saturate(dot(N, L)), saturate(dot(N, V)));
    vec3  F = CalculateSchlickFresnelReflectance(saturate(dot(H, V)), F0);

    vec3  numerator = D * G * F;
    float denominator = 4.0 * saturate(dot(N, V)) * NdotL + 0.0001;
    vec3  specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);

    vec3 diffuse = kD * albedo / PI;

    float attenuation = light.intensity / (4.0 * PI * distanceToLight * distanceToLight) * 100.0;
    vec3 radiance = light.color * attenuation;
    return (diffuse + specular) * radiance * NdotL;
}

vec3 directionaLightPhong(vec3 direction, vec3 color, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic)
{
    vec3 L = normalize(-direction);
    vec3 R = reflect(-L, N);

    float NdotL = saturate(dot(N, L));
    float shininess = clamp(1.0 / max(roughness * roughness, 0.001) * 8.0, 8.0, 512.0);
    float specFactor = pow(saturate(dot(R, V)), shininess);

    vec3 diffuse = albedo / PI;
    vec3 specular = mix(vec3(0.04), albedo, metallic) * specFactor;

    return (diffuse + specular) * color * NdotL;
}

vec3 pointLightPhong(PointLight light, vec3 fragmentPosition, vec3 V, vec3 N, vec3 albedo, float roughness,
                     float metallic)
{
    vec3 toLight = light.position - fragmentPosition;
    float distanceToLight = length(toLight);
    vec3 L = toLight / max(distanceToLight, 0.0001);
    vec3 R = reflect(-L, N);

    float NdotL = saturate(dot(N, L));
    float shininess = clamp(1.0 / max(roughness * roughness, 0.001) * 8.0, 8.0, 512.0);
    float specFactor = pow(saturate(dot(R, V)), shininess);

    vec3 diffuse = albedo / PI;
    vec3 specular = mix(vec3(0.04), albedo, metallic) * specFactor;

    float attenuation = light.intensity / (4.0 * PI * distanceToLight * distanceToLight) * 100.0;
    vec3 radiance = light.color * attenuation;
    return (diffuse + specular) * radiance * NdotL;
}

uniform int u_ShadingMode; // 0 = GGXTrowbridgeReitz, 1 = Gouraud, 2 = Phong

// Returns direct lighting (diffuse+specular) for the fragment. If Gouraud mode is active,
// the interpolated per-vertex precomputed direct lighting is returned instead.
vec3 computeDirectLighting(vec3 fragmentPosition, vec3 V, vec3 N, vec3 albedo, float roughness, float metallic, vec3 F0, vec3 gouraudDirect)
{
    if (u_ShadingMode == 1)
    {
        return gouraudDirect;
    }

    if (u_ShadingMode == 2)
    {
        vec3 Lo = vec3(0.0);

        Lo += directionaLightPhong(u_DirectionalLightDirection, u_DirectionalLightColor, V, N, albedo, roughness, metallic);

        for (int i = 0; i < u_PointLightCount; i++)
        {
            PointLight light = u_PointLights[i];
            Lo += pointLightPhong(light, fragmentPosition, V, N, albedo, roughness, metallic);
        }

        return Lo;
    }

    vec3 Lo = vec3(0.0);
    // Directional
    Lo += directionaLight(u_DirectionalLightDirection, u_DirectionalLightColor, V, N, albedo, roughness, metallic, F0);

    // Points
    for (int i = 0; i < u_PointLightCount; i++)
    {
        PointLight light = u_PointLights[i];
        Lo += pointLight(light, fragmentPosition, V, N, albedo, roughness, metallic, F0);
    }

    return Lo;
}
