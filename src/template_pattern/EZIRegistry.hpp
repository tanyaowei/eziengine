#ifndef _EZI_REGISTRY_HPP_
#define _EZI_REGISTRY_HPP_

#include <vector>
#include <string>
#include <typeinfo>
#include <cassert>

#include <template_pattern/EZRegister.h>
#include <common/EZIType.h>

namespace EZIEngine
{
template <typename T>
class IRegistry
{
public:
  IRegistry() = default;
  virtual ~IRegistry()
  {
    mRegisters.clear();
  }

  IRegistry(const IRegistry &) = delete;
  IRegistry &operator=(const IRegistry &) = delete;

  u32 size() const
  {
    return mRegisters.size();
  }

  template <typename U>
  U *get()
  {
    u32 typeindex = U::getTypeIndex();

    if (typeindex == std::numeric_limits<u32>::max())
    {
      typeindex = mRegisters.size();
      mRegisters.emplace_back(new U());
      U::setTypeIndex(typeindex);
    }

    return dynamic_cast<U *>(mRegisters[typeindex]);
  }

private:
  std::vector<std::unique_ptr<BaseRegister>> mRegisters;
};
} // namespace EZIEngine

#endif
