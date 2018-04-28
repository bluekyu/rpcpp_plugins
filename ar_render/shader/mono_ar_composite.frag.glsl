#version 430

#define USE_GBUFFER_EXTENSIONS
#pragma include "render_pipeline_base.inc.glsl"
#pragma include "includes/gbuffer.inc.glsl"
#pragma include "/$$rptemp/$$rpplugins_ar_render.inc.glsl"

uniform sampler2D ARCamScene;
uniform sampler2D ARCamDepthMap;
uniform mat4 ar_camera_proj_mat;
uniform mat4 trans_world_to_arCamera;

vec3 get_ar_camera_color(vec2 texcoord)
{
#if ARCAM_TEX_USE_BGR
    return textureLod(ARCamScene, texcoord, 0).rgb;
#else
    return textureLod(ARCamScene, texcoord, 0).bgr;
#endif
}

float get_ar_camera_depth(vec2 texcoord)
{
    return textureLod(ARCamDepthMap, texcoord, 0).x;
}

vec4 get_ar_screen_pos(vec4 world_pos)
{
    return ar_camera_proj_mat * trans_world_to_arCamera * world_pos;
}

#pragma include "ar_composite.inc.glsl"
