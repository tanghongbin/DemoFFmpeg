#version 300 es                                     
precision mediump float;                            
in vec2 v_texCoord;
in float outFactor;
layout(location = 0) out vec4 outColor;             
uniform sampler2D s_TextureMap;
const float offset = 1.0f / 300.0;

void main()                                         
{
    vec2 offsets[9] = vec2[](
    vec2(-offset,  offset), // 左上
    vec2( 0.0f,    offset), // 正上
    vec2( offset,  offset), // 右上
    vec2(-offset,  0.0f),   // 左
    vec2( 0.0f,    0.0f),   // 中
    vec2( offset,  0.0f),   // 右
    vec2(-offset, -offset), // 左下
    vec2( 0.0f,   -offset), // 正下
    vec2( offset, -offset)  // 右下
    );

    float value = 16.0;
    float kernel[9] = float[](
    1.0 / value, 2.0 / value, 1.0 / value,
    2.0 / value, 4.0 / value, 2.0 / value,
    1.0 / value, 2.0 / value, 1.0 / value
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(s_TextureMap, v_texCoord.st + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++) {
        col += sampleTex[i] * kernel[i];
    }
    outColor = vec4(col, 1.0);
}