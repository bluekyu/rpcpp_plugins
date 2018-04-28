#version 430

#define USE_GBUFFER_EXTENSIONS
#pragma include "render_pipeline_base.inc.glsl"
#pragma include "includes/gbuffer.inc.glsl"
#pragma include "/$$rptemp/$$rpplugins_ar_render.inc.glsl"

uniform sampler2DArray ARCamScene;
uniform sampler2D ARCamDepthMap_left;
uniform sampler2D ARCamDepthMap_right;
uniform mat4 ar_camera_proj_mat_left;
uniform mat4 ar_camera_proj_mat_right;
uniform mat4 trans_world_to_arCameraLeft;
uniform mat4 trans_world_to_arCameraRight;

#define IS_LEFT()   (gl_Layer == 0)

vec3 get_ar_camera_color(vec2 texcoord)
{
#if ARCAM_TEX_USE_BGR
    return textureLod(ARCamScene, vec3(texcoord, gl_Layer), 0).rgb;
#else
    return textureLod(ARCamScene, vec3(texcoord, gl_Layer), 0).bgr;
#endif
}

float get_ar_camera_depth(vec2 texcoord)
{
    return textureLod(IS_LEFT() ? ARCamDepthMap_left : ARCamDepthMap_right, texcoord, 0).x;
}

vec4 get_ar_screen_pos(vec4 world_pos)
{
    return (IS_LEFT() ? ar_camera_proj_mat_left : ar_camera_proj_mat_right) *
        (IS_LEFT() ? trans_world_to_arCameraLeft : trans_world_to_arCameraRight) *
        world_pos;
}

#pragma include "ar_composite.inc.glsl"
