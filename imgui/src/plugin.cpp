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

#include "rpplugins/imgui/plugin.hpp"

#include <boost/dll/alias.hpp>
#include <boost/any.hpp>
#include <boost/filesystem/operations.hpp>

#include <fmt/ostream.h>

#include <imgui.h>

#include <mouseButton.h>
#include <colorAttrib.h>
#include <colorBlendAttrib.h>
#include <depthTestAttrib.h>
#include <cullFaceAttrib.h>
#include <geomNode.h>
#include <geomTriangles.h>
#include <graphicsWindow.h>

#include <render_pipeline/rppanda/showbase/showbase.hpp>
#include <render_pipeline/rpcore/loader.hpp>
#include <render_pipeline/rpcore/globals.hpp>
#include <render_pipeline/rpcore/util/rpgeomnode.hpp>
#include <render_pipeline/rppanda/util/filesystem.hpp>

RENDER_PIPELINE_PLUGIN_CREATOR(rpplugins::ImGuiPlugin)

namespace rpplugins {

RENDER_PIPELINE_PLUGIN_DOWNCAST_IMPL(ImGuiPlugin);

const char* ImGuiPlugin::NEW_FRAME_EVENT_NAME = "imgui-new-frame";

rpcore::BasePlugin::RequrieType ImGuiPlugin::require_plugins_;

ImGuiPlugin::ImGuiPlugin(rpcore::RenderPipeline& pipeline): BasePlugin(pipeline, RPPLUGIN_ID_STRING)
{
}

ImGuiPlugin::~ImGuiPlugin() = default;

rpcore::BasePlugin::RequrieType& ImGuiPlugin::get_required_plugins(void) const
{
    return require_plugins_;
}

void ImGuiPlugin::on_pipeline_created()
{
    root_ = rpcore::Globals::base->get_pixel_2d().attach_new_node("imgui-root");

    context_ = ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    // Setup back-end capabilities flags
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    const std::string style_setting = boost::any_cast<std::string>(get_setting("style"));
    if (style_setting == "classic")
        ImGui::StyleColorsClassic();
    else if (style_setting == "light")
        ImGui::StyleColorsLight();
    else
        ImGui::StyleColorsDark();

    setup_geom();
    setup_shader();
    setup_font();
    setup_event();
    on_window_resized();

    add_task(std::bind(&ImGuiPlugin::new_frame, this, std::placeholders::_1), "ImGuiPlugin::new_frame", -30);
}

void ImGuiPlugin::on_window_resized()
{
    ImGuiIO& io = ImGui::GetIO();

    const int w = rpcore::Globals::native_resolution[0];
    const int h = rpcore::Globals::native_resolution[1];

    io.DisplaySize = ImVec2(static_cast<float>(w), static_cast<float>(h));
    io.DisplayFramebufferScale = ImVec2(
        w > 0 ? (static_cast<float>(rpcore::Globals::resolution[0]) / w) : 0,
        h > 0 ? (static_cast<float>(rpcore::Globals::resolution[1]) / h) : 0
    );
}

void ImGuiPlugin::on_unload()
{
    remove_all_tasks();

    ImGui::DestroyContext();
    context_ = nullptr;
}

void ImGuiPlugin::setup_geom()
{
    ImGuiIO& io = ImGui::GetIO();

    PT(GeomVertexArrayFormat) array_format = new GeomVertexArrayFormat(
        InternalName::get_vertex(), 4, Geom::NT_stdfloat, Geom::C_point,
        InternalName::get_color(), 1, Geom::NT_packed_dabc, Geom::C_color
    );

    CPT(GeomVertexFormat) vformat = GeomVertexFormat::register_format(new GeomVertexFormat(array_format));

    PT(GeomVertexData) vdata = new GeomVertexData("imgui-vertex", vformat, GeomEnums::UsageHint::UH_stream);

    // Add indices
    PT(GeomTriangles) prim = new GeomTriangles(GeomEnums::UsageHint::UH_stream);

    static_assert(
        sizeof(ImDrawIdx) == sizeof(uint16_t) ||
        sizeof(ImDrawIdx) == sizeof(uint32_t),
        "Type of ImDrawIdx is not uint16_t or uint32_t. Update below code!"
    );
    if (sizeof(ImDrawIdx) == sizeof(uint16_t))
        prim->set_index_type(GeomEnums::NumericType::NT_uint16);
    else if (sizeof(ImDrawIdx) == sizeof(uint32_t))
        prim->set_index_type(GeomEnums::NumericType::NT_uint32);

    prim->close_primitive();

    PT(Geom) geom = new Geom(vdata);
    geom->add_primitive(prim);

    auto state = RenderState::make(
        ColorAttrib::make_vertex(),
        ColorBlendAttrib::make(ColorBlendAttrib::M_add, ColorBlendAttrib::O_incoming_alpha, ColorBlendAttrib::O_one_minus_incoming_alpha),
        DepthTestAttrib::make(DepthTestAttrib::M_none),
        CullFaceAttrib::make(CullFaceAttrib::M_cull_none)
    );

    PT(GeomNode) geom_node = new GeomNode("imgui-geom");
    geom_node->add_geom(geom, state);

    geom_np_ = root_.attach_new_node(geom_node);
}

void ImGuiPlugin::setup_font()
{
    ImGuiIO& io = ImGui::GetIO();

    auto default_font_path = rppanda::convert_path(Filename(boost::any_cast<std::string>(get_setting("default_font_path"))));
    if (boost::filesystem::exists(default_font_path))
    {
        const float font_size = boost::any_cast<float>(get_setting("default_font_size"));
        io.Fonts->AddFontFromFileTTF(default_font_path.generic_string().c_str(), font_size);
    }
    else
    {
        error(fmt::format("Failed to find font: {}", default_font_path));
        io.Fonts->AddFontDefault();
    }

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    font_texture_ = new Texture("imgui-font-texture");
    font_texture_->setup_2d_texture(width, height, Texture::ComponentType::T_unsigned_byte, Texture::Format::F_red);
    font_texture_->set_minfilter(SamplerState::FilterType::FT_linear);
    font_texture_->set_magfilter(SamplerState::FilterType::FT_linear);

    PTA_uchar ram_image = font_texture_->make_ram_image();
    std::memcpy(ram_image.p(), pixels, width * height * sizeof(decltype(*pixels)));

    geom_np_.set_shader_input("font_texture", font_texture_);
}

void ImGuiPlugin::setup_shader()
{
    geom_np_.set_shader(rpcore::RPLoader::load_shader({ get_shader_resource("imgui.vert.glsl"), get_shader_resource("imgui.frag.glsl") }));
}

void ImGuiPlugin::setup_event()
{
    accept(MouseButton::one().get_name(), [](const Event* ev) { ImGui::GetIO().MouseDown[0] = true; });
    accept(MouseButton::one().get_name() + "-up", [](const Event* ev) { ImGui::GetIO().MouseDown[0] = false; });

    accept(MouseButton::two().get_name(), [](const Event* ev) { ImGui::GetIO().MouseDown[2] = true; });
    accept(MouseButton::two().get_name() + "-up", [](const Event* ev) { ImGui::GetIO().MouseDown[2] = false; });

    accept(MouseButton::three().get_name(), [](const Event* ev) { ImGui::GetIO().MouseDown[1] = true; });
    accept(MouseButton::three().get_name() + "-up", [](const Event* ev) { ImGui::GetIO().MouseDown[1] = false; });

    accept(MouseButton::wheel_up().get_name(), [](const Event* ev) { ImGui::GetIO().MouseWheel += 1; });
    accept(MouseButton::wheel_down().get_name(), [](const Event* ev) { ImGui::GetIO().MouseWheel -= 1; });
    accept(MouseButton::wheel_right().get_name(), [](const Event* ev) { ImGui::GetIO().MouseWheelH += 1; });
    accept(MouseButton::wheel_left().get_name(), [](const Event* ev) { ImGui::GetIO().MouseWheelH -= 1; });
}

AsyncTask::DoneStatus ImGuiPlugin::new_frame(rppanda::FunctionalTask* task)
{
    if (root_.is_hidden())
        return AsyncTask::DS_cont;

    update(task);

    ImGui::NewFrame();

    throw_event_directly(*EventHandler::get_global_event_handler(), NEW_FRAME_EVENT_NAME);

    ImGui::Render();

    render(task);

    return AsyncTask::DS_cont;
}

void ImGuiPlugin::update(rppanda::FunctionalTask* task)
{
    static const int MOUSE_DEVICE_INDEX = 0;

    ImGuiIO& io = ImGui::GetIO();

    io.DeltaTime = static_cast<float>(rpcore::Globals::clock->get_dt());

    auto window = rpcore::Globals::base->get_win();
    if (window->is_of_type(GraphicsWindow::get_class_type()))
    {
        const MouseData& mouse = window->get_pointer(MOUSE_DEVICE_INDEX);
        if (mouse.get_in_window())
        {
            if (io.WantSetMousePos)
            {
                window->move_pointer(MOUSE_DEVICE_INDEX, io.MousePos.x, io.MousePos.y);
            }
            else
            {
                io.MousePos.x = static_cast<float>(mouse.get_x());
                io.MousePos.y = static_cast<float>(mouse.get_y());
            }
        }
        else
        {
            io.MousePos.x = -FLT_MAX;
            io.MousePos.y = -FLT_MAX;
        }
    }
}

void ImGuiPlugin::render(rppanda::FunctionalTask* task)
{
    ImGuiIO& io = ImGui::GetIO();

    auto draw_data = ImGui::GetDrawData();
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    auto geom_node = DCAST(GeomNode, geom_np_.node());
    auto vertex_handle = geom_node->modify_geom(0)->modify_vertex_data()->modify_array_handle(0);
    auto index_handle = geom_node->modify_geom(0)->modify_primitive(0)->modify_vertices(draw_data->TotalIdxCount)->modify_handle();

    if (vertex_handle->get_num_rows() < draw_data->TotalVtxCount)
        vertex_handle->unclean_set_num_rows(draw_data->TotalVtxCount);

    if (index_handle->get_num_rows() < draw_data->TotalIdxCount)
        index_handle->unclean_set_num_rows(draw_data->TotalIdxCount);

    auto vertex_pointer = vertex_handle->get_write_pointer();
    auto index_pointer = index_handle->get_write_pointer();

    size_t accum_vertex_count = 0;
    for (int k = 0; k < draw_data->CmdListsCount; ++k)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[k];

        const size_t vtx_size = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        const size_t idx_size = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);

        std::copy(
            reinterpret_cast<const unsigned char*>(cmd_list->VtxBuffer.Data),
            reinterpret_cast<const unsigned char*>(cmd_list->VtxBuffer.Data) + vtx_size,
            vertex_pointer);

        std::copy(
            reinterpret_cast<const unsigned char*>(cmd_list->IdxBuffer.Data),
            reinterpret_cast<const unsigned char*>(cmd_list->IdxBuffer.Data) + idx_size,
            index_pointer);

        auto index_buffer = reinterpret_cast<uint16_t*>(index_pointer);
        for (size_t i = 0; i < cmd_list->IdxBuffer.Size; ++i)
            index_buffer[i] += accum_vertex_count;

        accum_vertex_count += cmd_list->VtxBuffer.Size;
        vertex_pointer += vtx_size;
        index_pointer += idx_size;
    }
}

}
