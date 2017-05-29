// This code contains NVIDIA Confidential Information and is disclosed to you
// under a form of NVIDIA software license agreement provided separately to you.
//
// Notice
// NVIDIA Corporation and its licensors retain all intellectual property and
// proprietary rights in and to this software and related documentation and
// any modifications thereto. Any use, reproduction, disclosure, or
// distribution of this software and related documentation without an express
// license agreement from NVIDIA Corporation is strictly prohibited.
//
// ALL NVIDIA DESIGN SPECIFICATIONS, CODE ARE PROVIDED "AS IS.". NVIDIA MAKES
// NO WARRANTIES, EXPRESSED, IMPLIED, STATUTORY, OR OTHERWISE WITH RESPECT TO
// THE MATERIALS, AND EXPRESSLY DISCLAIMS ALL IMPLIED WARRANTIES OF NONINFRINGEMENT,
// MERCHANTABILITY, AND FITNESS FOR A PARTICULAR PURPOSE.
//
// Information and code furnished is believed to be accurate and reliable.
// However, NVIDIA Corporation assumes no responsibility for the consequences of use of such
// information or for any infringement of patents or other rights of third parties that may
// result from its use. No license is granted by implication or otherwise under any patent
// or patent rights of NVIDIA Corporation. Details are subject to change without notice.
// This code supersedes and replaces all information previously supplied.
// NVIDIA Corporation products are not authorized for use as critical
// components in life support devices or systems without express written approval of
// NVIDIA Corporation.
//
// Copyright (c) 2013-2017 NVIDIA Corporation. All rights reserved.

#include <luse.h>

#include <rpflex/flex_buffer.hpp>

void GetParticleBounds(rpflex::FlexBuffer& buffer, LVecBase3f& lower, LVecBase3f& upper)
{
    lower = LVecBase3f(FLT_MAX);
    upper = LVecBase3f(-FLT_MAX);

    for (int i=0, i_end=buffer.positions_.size(); i < i_end; ++i)
    {
        lower = buffer.positions_[i].get_xyz().fmin(lower);
        upper = buffer.positions_[i].get_xyz().fmax(upper);
    }
}

// calculates local space positions given a set of particles and rigid indices
void CalculateRigidLocalPositions(const LVecBase4f* restPositions, int numRestPositions, const int* offsets, const int* indices, int numRigids, LVecBase3f* localPositions)
{
    // To improve the accuracy of the result, first transform the restPositions to relative coordinates (by finding the mean and subtracting that from all points)
    // Note: If this is not done, one might see ghost forces if the mean of the restPositions is far from the origin.

    // Calculate mean
    LVecBase3f shapeOffset(0.0f);

    for (int i = 0; i < numRestPositions; i++)
    {
        shapeOffset += LVecBase3f(restPositions[i].get_xyz());
    }

    shapeOffset /= float(numRestPositions);

    int count = 0;

    for (int r=0; r < numRigids; ++r)
    {
        const int startIndex = offsets[r];
        const int endIndex = offsets[r+1];

        const int n = endIndex-startIndex;

        assert(n);

        LVecBase3f com;

        for (int i=startIndex; i < endIndex; ++i)
        {
            const int r = indices[i];

            // By substracting meshOffset the calculation is done in relative coordinates
            com += LVecBase3f(restPositions[r].get_xyz()) - shapeOffset;
        }

        com /= float(n);

        for (int i=startIndex; i < endIndex; ++i)
        {
            const int r = indices[i];

            // By substracting meshOffset the calculation is done in relative coordinates
            localPositions[count++] = (LVecBase3f(restPositions[r].get_xyz()) - shapeOffset) - com;
        }
    }
}
