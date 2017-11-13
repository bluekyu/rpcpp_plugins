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

#pragma once

#include <dataNode.h>

#include <openvr.h>

namespace rpplugins {

class OpenVRController : public DataNode
{
public:
    OpenVRController(vr::IVRSystem* HMD);

protected:
    void do_transmit_data(DataGraphTraverser* trav,
        const DataNodeTransmit& input,
        DataNodeTransmit& output) override;

private:
    vr::IVRSystem* const HMD_;

public:
    static TypeHandle get_class_type();
    static void init_type();
    TypeHandle get_type() const override;
    TypeHandle force_init_type() override;

private:
    static TypeHandle type_handle_;
};

// ************************************************************************************************

inline OpenVRController::OpenVRController(vr::IVRSystem* HMD) :
    DataNode("OpenVRController"), HMD_(HMD)
{
}

inline TypeHandle OpenVRController::get_class_type()
{
    return type_handle_;
}

inline void OpenVRController::init_type()
{
    DataNode::init_type();
    register_type(type_handle_, "rpplugins::OpenVRController", DataNode::get_class_type());
}

inline TypeHandle OpenVRController::get_type() const
{
    return get_class_type();
}

inline TypeHandle OpenVRController::force_init_type()
{
    init_type();
    return get_class_type();
}

}
