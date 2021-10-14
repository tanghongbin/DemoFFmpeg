#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;

in vec2 out_texCoords;
uniform sampler2D s_TextureMap0;
uniform sampler2D s_TextureMap1;
uniform sampler2D s_TextureMap2;
uniform sampler2D s_LutTexture;
uniform sampler2D s_Filter0;
uniform sampler2D s_Filter1;
uniform sampler2D s_Filter2;
uniform vec2 texSize;
uniform float u_offset;
uniform int fliterType;// 滤镜类型

uniform int samplerType;

// 指定滤镜变量
uniform highp vec2 blurCenter;
uniform highp float blurSize;

vec4 yuv2Rgb(vec2 texCoords,bool isAvarage){
  vec3 yuv;
  yuv.x = texture(s_TextureMap0, texCoords).r;
  yuv.y = texture(s_TextureMap1, texCoords).r - 0.5;
  yuv.z = texture(s_TextureMap2, texCoords).r - 0.5;
  highp vec3 rgb = mat3(1.0,       1.0,     1.0,
  0.0, -0.344, 1.770,
  1.403,  -0.714,     0.0) * yuv;
  float value = (rgb.r + rgb.g + rgb.b) / 3.0 + 0.1;
  vec4 result;
  if (isAvarage) {
    result = vec4(vec3(value),1.0);
  } else {
    result = vec4(rgb, 1.0);
  }
  return result;
}

/// =================================== 滤镜类型 =======================

// 网格滤镜
vec4 filterSquare(){
  vec2 imgTexCoord = out_texCoords * texSize;//将纹理坐标系转换为图片坐标系
  float sideLength = 30.0;//网格的边长
  float maxOffset = 0.15 * sideLength;//设置网格线宽度的最大值
  float x = mod(imgTexCoord.x, floor(sideLength));
  float y = mod(imgTexCoord.y, floor(sideLength));

  float offset = u_offset * maxOffset;
  vec4 colorResult;

  if (offset <= x
  && x <= sideLength - offset
  && offset <= y
  && y <= sideLength - offset){
    colorResult = yuv2Rgb(out_texCoords,false);
  } else {
    colorResult = vec4(1.0, 1.0, 1.0, 1.0);
  }
  return colorResult;
}

// 多屏幕滤镜
vec4 filterMultipleScreen(){
  vec4 colorResult;
  vec2 newTexCood = out_texCoords;
  if (newTexCood.x < 0.5) {
    newTexCood.x = newTexCood.x * 2.0;
  } else {
    newTexCood.x = (newTexCood.x - 0.5) * 2.0;
  }
  if (newTexCood.y < 0.5) {
    newTexCood.y = newTexCood.y * 2.0;
  } else {
    newTexCood.y = (newTexCood.y - 0.5) * 2.0;
  }
  if ((out_texCoords.x < 0.5 && out_texCoords.y < 0.5) || (out_texCoords.x > 0.5 && out_texCoords.y > 0.5)) {
    colorResult = yuv2Rgb(newTexCood,true);
  } else {
    colorResult = yuv2Rgb(newTexCood,false);
  }
  return colorResult;
}

// LUT 滤镜
vec4 filterLUT(){
  vec4 textureColor  = yuv2Rgb(out_texCoords,false);
  float blueColor  = textureColor .b * 63.0;
  vec2 quad1;
  quad1.y = floor(floor(blueColor ) / 8.0);
  quad1.x = floor(blueColor  - (quad1.y * 8.0));

  vec2 quad2;
  quad2.y = floor(floor(blueColor ) / 8.0);
  quad2.x = floor(blueColor  - (quad2.y * 8.0));

  vec2 texPos1;
  texPos1.x = (quad1.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor .r);
  texPos1.y = (quad1.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor .g);

  vec2 texPos2;
  texPos2.x = (quad2.x * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor .r);
  texPos2.y = (quad2.y * 0.125) + 0.5/512.0 + ((0.125 - 1.0/512.0) * textureColor .g);

  //取目标映射对应的像素值
  vec4 newColor1 = texture(s_LutTexture, texPos1);
  vec4 newColor2 = texture(s_LutTexture, texPos2);

  //使用 Mix 方法对 2 个边界像素值进行混合
  vec4 newColor = mix(newColor1, newColor2, fract(blueColor ));
  return mix(textureColor , vec4(newColor.rgb, textureColor .w), 1.0);
}

vec4 texturePosition(vec2 texCoords){
  vec3 yuv;
  yuv.x = texture(s_TextureMap0, texCoords).r;
  yuv.y = texture(s_TextureMap1, texCoords).r - 0.5;
  yuv.z = texture(s_TextureMap2, texCoords).r - 0.5;
  highp vec3 rgb = mat3(1.0,       1.0,     1.0,
  0.0, -0.344, 1.770,
  1.403,  -0.714,     0.0) * yuv;
  return vec4(rgb,1.0);
}

// 高斯模糊
vec4 zoomBlurFilter(){
  
  vec2 samplingOffset = 1.0/100.0 * (blurCenter - out_texCoords) * blurSize;

  vec4 fragmentColor = texturePosition( out_texCoords) * 0.18;
  fragmentColor += texturePosition( out_texCoords + samplingOffset) * 0.15;
  fragmentColor += texturePosition( out_texCoords + (2.0 * samplingOffset)) *  0.12;
  fragmentColor += texturePosition( out_texCoords + (3.0 * samplingOffset)) * 0.09;
  fragmentColor += texturePosition( out_texCoords + (4.0 * samplingOffset)) * 0.05;
  fragmentColor += texturePosition( out_texCoords - samplingOffset) * 0.15;
  fragmentColor += texturePosition( out_texCoords - (2.0 * samplingOffset)) *  0.12;
  fragmentColor += texturePosition( out_texCoords - (3.0 * samplingOffset)) * 0.09;
  fragmentColor += texturePosition( out_texCoords - (4.0 * samplingOffset)) * 0.05;

  return fragmentColor;
}

// 普通滤镜
vec4 normalFilter(vec2 texCoords){
  return yuv2Rgb(out_texCoords,false);
}


// gpuimagefilter 的滤镜例子，拿一个来玩玩
vec4 accoFilter(vec2 texCoords){
  vec4 originColor = yuv2Rgb(texCoords,false);
  vec4 texel = yuv2Rgb(texCoords,false);
  vec3 bbTexel = texture(s_Filter0, texCoords).rgb;

  texel.r = texture(s_Filter1, vec2(bbTexel.r, texel.r)).r;
  texel.g = texture(s_Filter1, vec2(bbTexel.g, texel.g)).g;
  texel.b = texture(s_Filter1, vec2(bbTexel.b, texel.b)).b;

  vec4 mapped;
  mapped.r = texture(s_Filter2, vec2(texel.r, .16666)).r;
  mapped.g = texture(s_Filter2, vec2(texel.g, .5)).g;
  mapped.b = texture(s_Filter2, vec2(texel.b, .83333)).b;
  mapped.a = 1.0;
  float strength = 0.5f;
  mapped.rgb = mix(originColor.rgb, mapped.rgb, strength);

  return mapped;
}

void main()
{
  // 1-rgba
  if (samplerType == 1) {
      outColor = texture(s_TextureMap0, out_texCoords);
  } else {
    //2-yuv420p
    if(fliterType == 1){
      // 滤镜方格
      outColor = filterSquare();
    } else if (fliterType == 2) {
      // 分屏显示
      outColor = filterMultipleScreen();
    } else if (fliterType == 3) {
      // LUT 滤镜
      if ( out_texCoords. y < 0.5) {
        outColor = filterLUT();
      } else {
        outColor = yuv2Rgb(out_texCoords,true);
//        outColor = texture(s_LutTexture,out_texCoords);
      }
    } else if (fliterType == 4) {
      // 4- 高斯模糊
      outColor = zoomBlurFilter();
    } else if (fliterType == 5) {
      // 5- 纯滤镜
      outColor = normalFilter(out_texCoords);
    } else if (fliterType == 6){
      outColor = accoFilter(out_texCoords);
    }
  }
}