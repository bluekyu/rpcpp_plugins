#version 430

#define USE_GBUFFER_EXTENSIONS
#pragma include "render_pipeline_base.inc.glsl"
#pragma include "includes/gbuffer.inc.glsl"

#if STEREO_MODE
uniform sampler2DArray ShadedScene;
#else
uniform sampler2D ShadedScene;
#endif

out vec4 result;

void main() {
    const vec2 texcoord = get_texcoord();

#if STEREO_MODE
    result = textureLod(ShadedScene, vec3(texcoord, gl_Layer), 0);
#else
    result = textureLod(ShadedScene, texcoord, 0);
#endif
}
