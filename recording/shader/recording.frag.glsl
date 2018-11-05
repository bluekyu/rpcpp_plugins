#version 430

uniform sampler2D source_texture;

out vec4 result;

void main()
{
    result = texelFetch(source_texture, ivec2(gl_FragCoord.xy), 0);
}
