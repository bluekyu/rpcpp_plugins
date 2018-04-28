#define DEPTH_EPSILON 0.00001f
#define DEPTH_BIAS 0.0001f
#define DISABLE_REMAP 0         // disable re-mapping process

#if STEREO_MODE
uniform sampler2DArray ShadedScene;
uniform sampler2DArray RealDepthMap;        // depth map of real objects on view of HMD
#else
uniform sampler2D ShadedScene;
uniform sampler2D RealDepthMap;
#endif

out vec4 result;

float get_real_depth(vec2 texcoord)
{
#if STEREO_MODE
    return textureLod(RealDepthMap, vec3(texcoord, gl_Layer), 0).x;
#else
    return textureLod(RealDepthMap, texcoord, 0).x;
#endif
}

void main() {
    const vec2 texcoord = get_texcoord();

#if STEREO_MODE
    const vec3 scene_color = textureLod(ShadedScene, vec3(texcoord, gl_Layer), 0).xyz;
#else
    const vec3 scene_color = textureLod(ShadedScene, texcoord, 0).xyz;
#endif

#if DISABLE_COMPOSITE
    result = vec4(scene_color, 1);
    return;
#endif

    const float real_obj_depth = min(get_real_depth(texcoord), 0.999f);

#if DISABLE_REMAP

#if STEREO_MODE
    const float scene_depth = get_depth_at(texcoord, gl_Layer);
#else
    const float scene_depth = get_depth_at(texcoord);
#endif

    // if real object is farther than virtual object
    if (real_obj_depth - scene_depth > DEPTH_EPSILON)
    {
        // then show virtual object.
        result = vec4(scene_color, 1);
    }
    else
    {
    #if ARCAM_TEX_FLIP_VERTICAL
        result = vec4(get_ar_camera_color(vec2(texcoord.x, 1 - texcoord.y), 1));
    #else
        result = vec4(get_ar_camera_color(texcoord, 1));
    #endif
    }
    return;

#endif  // #if DISABLE_REMAP

    // when depth map is not used, do NOT use real_obj_depth by precision error.
#if STEREO_MODE
    const vec4 wpos = vec4(calculate_surface_pos(real_obj_depth, texcoord, gl_Layer), 1.0f);
#else
    const vec4 wpos = vec4(calculate_surface_pos(real_obj_depth, texcoord), 1.0f);
#endif

    vec4 spos = get_ar_screen_pos(wpos);
    spos /= spos.w;

    // check if AR texture coordinate is out of AR texture.
    vec2 ar_texcoord = fma(spos.xy, vec2(0.5), vec2(0.5));
    if ((ar_texcoord.x < 0.0f || ar_texcoord.x > 1.0f) || (ar_texcoord.y < 0.0f || ar_texcoord.y > 1.0f))
    {
        result = vec4(scene_color, 1);
        return;
    }

    // check if the pixel in AR Camera Texture is valid, or not.
    //
    // Both spos.z and ar_camera_depth are depth of real object on view of AR camera.
    // In addition, spos.z is calculated from `real_obj_depth` (the depth on HMD, virtual camera).
    //
    // Therefore, on some fragments, spos.z is different from ar_camera_depth if
    // a point on view of HMD is NOT visible on view of AR camera. (that is, it is occluded.)
    //
    // So, if REMOVE_OCCLUSION is 1, those fragments use pixel on virtual scene.
    //
    // Note that, this calculation is simillar with a calculation in shadow rendering.
    const float ar_camera_depth = get_ar_camera_depth(ar_texcoord);
    const bool ar_camera_depth_valid = ar_camera_depth < 1;
#if REMOVE_OCCLUSION
    const bool is_occluded = ar_camera_depth_valid && (DEPTH_BIAS > ar_camera_depth - spos.z);
#else
    const bool is_occluded = false;
#endif

#if RENDER_ONLY_VALID_AR_DEPTH
    // Use pixel of virtual scene if depth of pixel on AR camera is NOT valid.
    if (!ar_camera_depth_valid || is_occluded)
    {
        result = vec4(scene_color, 1);
        return;
    }
#endif

#if STEREO_MODE
    const float scene_depth = get_depth_at(texcoord, gl_Layer);
#else
    const float scene_depth = get_depth_at(texcoord);
#endif

    if (real_obj_depth - scene_depth > DEPTH_EPSILON || is_occluded)
    {
        result = vec4(scene_color, 1);
    }
    else
    {
    #if ARCAM_TEX_FLIP_VERTICAL
        ar_texcoord.y = 1 - ar_texcoord.y;
    #endif

        result = vec4(get_ar_camera_color(ar_texcoord), 1);
    }
}
