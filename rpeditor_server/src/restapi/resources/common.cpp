#include "restapi/resources/common.hpp"

#include <render_pipeline/rpcore/globals.h>
#include <render_pipeline/rpcore/rpobject.h>

namespace rpeditor {

NodePath get_nodepath(const rapidjson::Value& scene_path)
{
    if (!scene_path.IsArray())
    {
        rpcore::RPObject::global_error("plugin::" PLUGIN_ID_STRING, "Scene path is NOT array of indices.");
        return NodePath();
    }

    NodePath np = rpcore::Globals::render;
    for (const auto& index: scene_path.GetArray())
    {
        if (index.GetInt() < np.get_num_children())
        {
            np = np.get_child(index.GetInt());
        }
        else
        {
            rpcore::RPObject::global_error("plugin::" PLUGIN_ID_STRING, "Out of range of children.");
            return NodePath();
        }
    }

    return np;
}

}	// namespace rpeditor
