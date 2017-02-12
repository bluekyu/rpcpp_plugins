#include <boost/log/trivial.hpp>

#include <geomNode.h>
#include <materialAttrib.h>

#include <render_pipeline/rpcore/globals.h>

#include "restapi/resources/common.hpp"
#include "restapi/resolve_message.hpp"
#include "restapi/restapi_server.hpp"

namespace restapi {

bool resolve_material(const rapidjson::Document& doc)
{
    const std::string& method = doc["method"].GetString();
    if (method == RPEDITOR_API_READ_STRING)
    {
        const NodePath& np = get_nodepath(doc["message"]["path"]);
        if (!np)
            return false;

        GeomNode* gnode = DCAST(GeomNode, np.node());
        if (!gnode)
        {
            BOOST_LOG_TRIVIAL(error) << "This node is NOT GeomNode: " << np;
            return false;
        }

        int geom_index = (std::min)(gnode->get_num_geoms()-1, doc["message"]["index"].GetInt());

        CPT(RenderState) geom_state = gnode->get_geom_state(geom_index);

        const RenderAttrib* attrib = geom_state->get_attrib(MaterialAttrib::get_class_slot());
        if (!attrib)
            return true;

        rapidjson::Document new_doc;
        rapidjson::Value& message = init_document(new_doc, "Material", RPEDITOR_API_UPDATE_STRING);
        auto& allocator = new_doc.GetAllocator();

        message.AddMember("num_geoms", gnode->get_num_geoms(), allocator);
        message.AddMember("index", geom_index, allocator);

        const Material* material = DCAST(MaterialAttrib, attrib)->get_material();

        message.AddMember("name", material->get_name(), allocator);
        message.AddMember("shading_model", int(material->get_emission().get_x()), allocator);

        rapidjson::Value base_color_array(rapidjson::kArrayType);
        const auto& base_color = material->get_base_color();
        base_color_array.PushBack(base_color[0], allocator);
        base_color_array.PushBack(base_color[1], allocator);
        base_color_array.PushBack(base_color[2], allocator);
        message.AddMember("base_color", base_color_array, allocator);

        message.AddMember("roughness", material->get_roughness(), allocator);
        message.AddMember("specular_ior", material->get_refractive_index(), allocator);

        RestAPIServer::get_instance()->broadcast(new_doc);
    }
    else if (method == RPEDITOR_API_UPDATE_STRING)
    {
        const auto& message = doc["message"];
        const NodePath& np = get_nodepath(doc["message"]["path"]);
        if (!np)
            return false;

        GeomNode* gnode = DCAST(GeomNode, np.node());
        if (!gnode)
        {
            BOOST_LOG_TRIVIAL(error) << "This node is NOT GeomNode: " << np;
            return false;
        }

        const int geom_index = message["index"].GetInt();
        if (geom_index >= gnode->get_num_geoms())
        {
            BOOST_LOG_TRIVIAL(warning) << "Out of range of geoms";
            return false;
        }

        CPT(RenderState) geom_state = gnode->get_geom_state(geom_index);

        const RenderAttrib* attrib = geom_state->get_attrib(MaterialAttrib::get_class_slot());
        PT(Material) material = new Material;
        if (attrib)
            *material = *DCAST(MaterialAttrib, attrib)->get_material();

        const auto& emission = material->get_emission();
        material->set_emission(LColor(message["shading_model"].GetInt(), emission.get_y(), emission.get_z(), emission.get_w()));

        const auto& base_color = message["base_color"];
        material->set_base_color(LColorf(base_color[0].GetFloat(), base_color[1].GetFloat(), base_color[2].GetFloat(), 1.0f));

        material->set_roughness(message["roughness"].GetFloat());
        material->set_refractive_index(message["speuclar_ior"].GetFloat());

        gnode->set_geom_state(geom_index, geom_state->set_attrib(MaterialAttrib::make(material)));
    }
    else
    {
        BOOST_LOG_TRIVIAL(error) << "Unknown method: " << method;
        return false;
    }

    return true;
}

// ************************************************************************************************

ConfigureStaticInit(Material)
{
    auto& resolver_map = get_resolver_map();
    resolver_map["Material"] = resolve_material;
}

}   // namespace restapi
