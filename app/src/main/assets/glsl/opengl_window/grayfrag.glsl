#version 300 es
//黑白滤镜
precision highp float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D s_TextureMap;//采样器
void main()
{
    outColor = texture(s_TextureMap, v_texCoord);
    if(v_texCoord.x > 0.5)
        outColor = vec4(vec3(outColor.r*0.299 + outColor.g*0.587 + outColor.b*0.114), outColor.a);
}