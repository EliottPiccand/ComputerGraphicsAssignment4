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
