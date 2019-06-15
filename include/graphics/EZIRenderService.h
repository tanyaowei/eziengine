#ifndef _EZI_RENDERSREVICE_H_
#define _EZI_RENDERSREVICE_H_

#include <string>
#include <vector>

#include <common/EZIType.h>
#include <math/EZIMath.h>
#include <template_pattern/EZISingleton.hpp>

namespace EZIEngine
{
struct RenderObject
{
  i32 mOrderID = 0;
  f32 mOpacity = 1.0f;
  Mat4 mTransform = Mat4(1.0f);
};

class RenderLayer
{
public:
  void Sort();
  void Insert(const RenderObject &obj);
  void Draw();
  void Clear();

private:
  std::vector<RenderObject> mAlphaQueue;
  std::vector<RenderObject> mSolidQueue;
};

class RenderService : public ISingleton<RenderService>
{
public:
  void InitRenderLayers(u32 size);

  void AddToRenderLayer(u32 index, const RenderObject &obj);

  u32 GetRenderLayersSize();

  void ClearRenderLayer(u32 index);

  void ClearAllRenderLayers();

private:
  std::vector<RenderLayer> mList;
};
} // namespace EZIEngine

#endif
