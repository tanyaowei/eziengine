#ifndef _EZI_MODEL3D_H_
#define _EZI_MODEL3D_H_

#include <string>
#include <vector>

#include <common/EZIType.h>
#include <math/EZIMath.h>

namespace EZIEngine
{
struct Model3DObject
{
    std::vector<f32> mVertices;
    std::vector<f32> mNormals;
    std::vector<f32> mTangent;
    std::vector<f32> mBitangent;
    std::vector<f32> mUVs;
    std::vector<u8> mColors;
    std::vector<u32> mIndices;
};
} // namespace EZIEngine

#endif
