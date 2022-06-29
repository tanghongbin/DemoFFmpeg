#version 300 es
// rgba 纹理通过采样存储到yuv420p里边
#extension GL_OES_EGL_image_external_essl3 : require
precision mediump float;
layout(location = 0) out vec4 outColor;
in vec2 out_texCoords;
uniform sampler2D s_TextureMap;
uniform float u_Offset;  //偏移量 1.0/width
uniform vec2 u_ImgSize;//图像尺寸
const vec3 COEF_Y = vec3( 0.299,  0.587,  0.114);
const vec3 COEF_U = vec3(-0.147, -0.289,  0.436);
const vec3 COEF_V = vec3( 0.615, -0.515, -0.100);
const float U_DIVIDE_LINE = 2.0 / 3.0;
const float V_DIVIDE_LINE = 1.25 / 1.5;

void main()
{
//  vec4 color = texture(yuvTexSampler, out_texCoords);
//  float value = (color.r+color.g+color.b)/3.0;
//  outColor = vec4(vec3(value),1.0);
//  outColor = vec4(0.5,1.0,1.0,1.0);
  vec2 texelOffset = vec2(u_Offset, 0.0);
  if(out_texCoords.y <= U_DIVIDE_LINE) {
    //在纹理坐标 y < (2/3) 范围，需要完成一次对整个纹理的采样，
    //一次采样（加三次偏移采样）4 个 RGBA 像素（R,G,B,A）生成 1 个（Y0,Y1,Y2,Y3），整个范围采样结束时填充好 width*height 大小的缓冲区；

    vec2 texCoord = vec2(out_texCoords.x, out_texCoords.y * 3.0 / 2.0);
    vec4 color0 = texture(s_TextureMap, texCoord);
    vec4 color1 = texture(s_TextureMap, texCoord + texelOffset);
    vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 2.0);
    vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 3.0);

    float y0 = dot(color0.rgb, COEF_Y);
    float y1 = dot(color1.rgb, COEF_Y);
    float y2 = dot(color2.rgb, COEF_Y);
    float y3 = dot(color3.rgb, COEF_Y);
    outColor = vec4(y0, y1, y2, y3);
  } else if(out_texCoords.y <= V_DIVIDE_LINE){

    //当纹理坐标 y > (2/3) 且 y < (5/6) 范围，一次采样（加三次偏移采样）8 个 RGBA 像素（R,G,B,A）生成（U0,U1,U2,U3），
    //又因为 U plane 缓冲区的宽高均为原图的 1/2 ，U plane 在垂直方向和水平方向的采样都是隔行进行，整个范围采样结束时填充好 width*height/4 大小的缓冲区。

    float offsetY = 1.0 / 3.0 / u_ImgSize.y;
    vec2 texCoord;
    if(out_texCoords.x <= 0.5) {
      texCoord = vec2(out_texCoords.x * 2.0, (out_texCoords.y - U_DIVIDE_LINE) * 2.0 * 3.0);
    }
    else {
      texCoord = vec2((out_texCoords.x - 0.5) * 2.0, ((out_texCoords.y - U_DIVIDE_LINE) * 2.0 + offsetY) * 3.0);
    }

    vec4 color0 = texture(s_TextureMap, texCoord);
    vec4 color1 = texture(s_TextureMap, texCoord + texelOffset * 2.0);
    vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 4.0);
    vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 6.0);

    float u0 = dot(color0.rgb, COEF_U) + 0.5;
    float u1 = dot(color1.rgb, COEF_U) + 0.5;
    float u2 = dot(color2.rgb, COEF_U) + 0.5;
    float u3 = dot(color3.rgb, COEF_U) + 0.5;
    outColor = vec4(u0, u1, u2, u3);
  }
  else {
    //当纹理坐标 y > (5/6) 范围，一次采样（加三次偏移采样）8 个 RGBA 像素（R,G,B,A）生成（V0,V1,V2,V3），
    //同理，因为 V plane 缓冲区的宽高均为原图的 1/2 ，垂直方向和水平方向都是隔行采样，整个范围采样结束时填充好 width*height/4 大小的缓冲区。

    float offsetY = 1.0 / 3.0 / u_ImgSize.y;
    vec2 texCoord;
    if(out_texCoords.x <= 0.5) {
      texCoord = vec2(out_texCoords.x * 2.0, (out_texCoords.y - V_DIVIDE_LINE) * 2.0 * 3.0);
    }
    else {
      texCoord = vec2((out_texCoords.x - 0.5) * 2.0, ((out_texCoords.y - V_DIVIDE_LINE) * 2.0 + offsetY) * 3.0);
    }

    vec4 color0 = texture(s_TextureMap, texCoord);
    vec4 color1 = texture(s_TextureMap, texCoord + texelOffset * 2.0);
    vec4 color2 = texture(s_TextureMap, texCoord + texelOffset * 4.0);
    vec4 color3 = texture(s_TextureMap, texCoord + texelOffset * 6.0);

    float v0 = dot(color0.rgb, COEF_V) + 0.5;
    float v1 = dot(color1.rgb, COEF_V) + 0.5;
    float v2 = dot(color2.rgb, COEF_V) + 0.5;
    float v3 = dot(color3.rgb, COEF_V) + 0.5;
    outColor = vec4(v0, v1, v2, v3);
  }
}
