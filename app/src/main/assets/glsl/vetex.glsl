attribute vec4 a_Position;
attribute vec2 a_TexCoordinate;
varying vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoordinate;
    gl_Position = a_Position;
}