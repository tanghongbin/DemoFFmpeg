#version 300 es
precision highp float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_TextureMap;//采样器
uniform float u_Offset;
uniform vec2 u_TexSize;
void main()
{
    vec2 imgTexCoord = v_texCoord * u_TexSize;
    float sideLength = u_TexSize.y / 6.0;
    float maxOffset = 0.15 * sideLength;
    float x = mod(imgTexCoord.x, floor(sideLength));
    float y = mod(imgTexCoord.y, floor(sideLength));

    float offset = u_Offset * maxOffset;

    if(offset <= x
    && x <= sideLength - offset
    && offset <= y
    && y <= sideLength - offset)
    {
        outColor = texture(s_TextureMap, v_texCoord);
    }
    else
    {
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}
