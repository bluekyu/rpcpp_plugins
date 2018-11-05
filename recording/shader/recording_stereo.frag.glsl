#version 430

uniform sampler2DArray source_texture;

out vec4 result;

void main()
{
    result = texelFetch(source_texture, ivec3(gl_FragCoord.xy, gl_Layer), 0);
}
