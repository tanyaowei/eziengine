#ifndef _EZI_COMPONENT_HPP_
#define _EZI_COMPONENT_HPP_

#include <vector>
#include <common/EZIType.h>
#include <template_pattern/EZIRegister.hpp>

namespace EZIEngine
{
class IComponent
{
public:
  IComponent() = default;
  virtual ~IComponent() = default;

private:
};

template <typename T>
class IComponentPool;

class ComponentRegistry : public IRegistry<ComponentRegistry>
{
public:
  ComponentRegistry() = default;
  virtual ~ComponentRegistry() = default;

  template <typename U>
  IComponentPool<U> *getComponentPool()
  {
    return get<IComponentPool<U>>();
  }

  template <typename U>
  u32 allocateComponent()
  {
    auto *pool = getComponentPool<U>();

    return pool->allocate();
  }

  template <typename U>
  U *getComponentByID(u32 index)
  {
    return nullptr;
  }

private:
};

template <typename T>
class IComponentPool : public IRegister<ComponentRegistry, IComponentPool<T>>
{
public:
  IComponentPool() = default;
  virtual ~IComponentPool() = default;

  u32 allocate()
  {
    return 0;
  }

private:
};
} // namespace EZIEngine

#endif