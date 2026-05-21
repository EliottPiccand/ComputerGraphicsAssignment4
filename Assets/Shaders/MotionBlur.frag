#version 460 core

layout(location = 0) in vec2 in_UV;

uniform sampler2D u_CurrentColorMap;
uniform sampler2D u_PreviousColorMap;
uniform float u_MotionBlurFactor;

out vec4 out_Color;

void main()
{
    vec2 uv = vec2(in_UV.x, 1.0 - in_UV.y);
    vec4 newColor = texture(u_CurrentColorMap, uv);
    vec4 oldColor = texture(u_PreviousColorMap, uv);
    out_Color = newColor * u_MotionBlurFactor + oldColor * (1.0 - u_MotionBlurFactor);
}
