/**
 * MIT License
 * 
 * Copyright (c) 2016-2017 Center of Human-centered Interaction for Coexistence
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#version 430

#define USE_GBUFFER_EXTENSIONS
#pragma include "render_pipeline_base.inc.glsl"
#pragma include "includes/gbuffer.inc.glsl"

#if STEREO_MODE
uniform sampler2DArray ShadedScene;
#else
uniform sampler2D ShadedScene;
#endif

uniform sampler2D background_tex;

uniform bool enabled;

out vec4 result;

#define BACKGROUND_DEPTH 0.9999f

void main() {
    const vec2 texcoord = get_texcoord();

#if STEREO_MODE
    const vec3 scene_color = textureLod(ShadedScene, vec3(texcoord, gl_Layer), 0).xyz;
#else
    const vec3 scene_color = textureLod(ShadedScene, texcoord, 0).xyz;
#endif

    if (enabled)
    {
#if STEREO_MODE
        const float scene_depth = get_depth_at(texcoord, gl_Layer);
#else
        const float scene_depth = get_depth_at(texcoord);
#endif

        if (scene_depth < BACKGROUND_DEPTH)
        {
            result = vec4(scene_color, 1);
        }
        else
        {
#if STEREO_MODE
            const vec3 leye_right = calculate_surface_pos(BACKGROUND_DEPTH, vec2(1, 0.5), 0);
            const vec3 reye_left = calculate_surface_pos(BACKGROUND_DEPTH, vec2(0, 0.5), 1);
            const vec3 reye_right = calculate_surface_pos(BACKGROUND_DEPTH, vec2(1, 0.5), 1);

            const float far_plane_width = distance(reye_right, reye_left);
            const float background_width = distance(leye_right, reye_left);
            const float background_tc_scale = far_plane_width / background_width;

            vec2 background_tc = vec2(background_tc_scale * texcoord.x, texcoord.y);
            if (gl_Layer == 1)
                background_tc.x += background_tc_scale;

            result = vec4(textureLod(background_tex, background_tc, 0).xyz, 1);
#else
            result = vec4(textureLod(background_tex, texcoord, 0).xyz, 1);
#endif
        }
    }
    else
    {
        result = vec4(scene_color, 1);
    }
}
