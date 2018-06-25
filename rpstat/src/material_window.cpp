/**
 * MIT License
 *
 * Copyright (c) 2018 Younguk Kim (bluekyu)
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

#pragma once

#include "material_window.hpp"

#include <fmt/ostream.h>

#include <paramNodePath.h>

#include <render_pipeline/rpcore/util/rpmaterial.hpp>
#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/globals.hpp>

namespace rpplugins {

MaterialWindow::MaterialWindow() : WindowInterface("Material: None", "###Material")
{
    rppanda::Messenger::get_global_instance()->accept(
        MATERIAL_SELECTED_EVENT_NAME,
        [this](const Event* ev)
        {
            material_ = DCAST(Material, ev->get_parameter(0).get_ptr());
        }
    );
}

void MaterialWindow::draw()
{
    if (material_)
        title_ = fmt::format("Material: {}", material_->get_name());
    else
        title_ = "Material: None";

    WindowInterface::draw();
}

void MaterialWindow::draw_contents()
{
    if (!material_)
        return;

    rpcore::RPMaterial mat(material_);

    static const char* shading_models[] = { "Default", "Emissive", "Clearcoat", "Transparent", "Skin", "Foliage" };
    int shading_model = static_cast<int>(mat.get_shading_model());
    bool changed = ImGui::ListBox("Render Mode\n(single select)", &shading_model, shading_models, std::extent<decltype(shading_models)>::value, 5);

    mat.set_shading_model(static_cast<rpcore::RPMaterial::ShadingModel>(shading_model));

    auto base_color = mat.get_base_color();
    if (ImGui::ColorEdit4("Base Color", &base_color[0]))
        mat.set_base_color(base_color);

    float normal_factor = mat.get_normal_factor();
    if (ImGui::SliderFloat("Normal Factor", &normal_factor, 0, 1))
        mat.set_normal_factor(normal_factor);

    float roughness = mat.get_roughness();
    if (ImGui::SliderFloat("Roughness", &roughness, 0, 1))
        mat.set_roughness(roughness);

    float specular_ior = mat.get_specular_ior();
    if (ImGui::SliderFloat("Specular IOR", &specular_ior, 1, 2.51f))
        mat.set_specular_ior(specular_ior);

    float metallic = mat.get_metallic();
    if (ImGui::SliderFloat("Metallic", &metallic, 0, 1))
        mat.set_metallic(metallic);

    if (mat.get_shading_model() == rpcore::RPMaterial::ShadingModel::TRANSPARENT_MODEL)
    {
        bool use_alpha_texture = mat.is_alpha_texture_mode();
        if (ImGui::Checkbox("Use alpha texture", &use_alpha_texture))
        {
            if (use_alpha_texture)
                mat.set_alpha_texture_mode();
            else
                mat.set_alpha(1.0f);
        }

        if (!use_alpha_texture)
        {
            float alpha = mat.get_alpha();
            if (ImGui::SliderFloat("Alpha", &alpha, 0, 1))
                mat.set_alpha(alpha);
        }
    }
}

}
