#version 300 es
precision mediump float;
layout(location = 0) out vec4 outColor;

void main()
{
     gl_FragDepth = gl_FragCoord.z;
//     outColor = vec4(gl_FragCoord.z,0.0,0.0,1.0);
}