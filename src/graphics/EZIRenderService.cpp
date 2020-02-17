#include <graphics/EZIRenderService.h>

#include <cassert>
#include <algorithm>

namespace EZIEngine
{
void RenderLayer::Sort()
{
    std::sort(mAlphaQueue.begin(), mAlphaQueue.end(),
              [](const RenderObject &lhs, const RenderObject &rhs) {
                  return lhs.mOrderID < rhs.mOrderID;
              });

    std::sort(mSolidQueue.begin(), mSolidQueue.end(),
              [](const RenderObject &lhs, const RenderObject &rhs) {
                  return lhs.mOrderID < rhs.mOrderID;
              });
}
void RenderLayer::Insert(const RenderObject &obj)
{
    if (obj.mOpacity < 1.0f)
    {
        mAlphaQueue.push_back(obj);
    }
    else
    {
        mSolidQueue.push_back(obj);
    }
}
void RenderLayer::Draw()
{
}

void RenderLayer::Clear()
{
    mAlphaQueue.clear();

    mSolidQueue.clear();
}

void RenderService::InitRenderLayers(u32 size)
{
    mList.resize(size);
}

void RenderService::AddToRenderLayer(u32 index, const RenderObject &obj)
{
    assert((index < GetRenderLayersSize()));

    mList[index].Insert(obj);
}

u32 RenderService::GetRenderLayersSize()
{
    return mList.size();
}

void RenderService::ClearRenderLayer(u32 index)
{
    assert((index < GetRenderLayersSize()));

    mList[index].Clear();
}

void RenderService::ClearAllRenderLayers()
{
    for (u32 i = 0; i < GetRenderLayersSize(); ++i)
    {
        ClearRenderLayer(i);
    }
}
} // namespace EZIEngine