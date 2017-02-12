#include "restapi/resources/common.hpp"

#include <render_pipeline/rpcore/globals.h>

#include <boost/log/trivial.hpp>

namespace restapi {

NodePath get_nodepath(const rapidjson::Value& scene_path)
{
    if (!scene_path.IsArray())
    {
        BOOST_LOG_TRIVIAL(error) << "Scene path is NOT array of indices.";
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
            BOOST_LOG_TRIVIAL(error) << "Out of range of children.";
            return NodePath();
        }
    }

    return np;
}

}	// namespace restapi
