#ifndef _EZI_REGISTER_HPP_
#define _EZI_REGISTER_HPP_

#include <string>

#include <common/EZIType.h>
#include <template_pattern/EZIRegister.h>

namespace EZIEngine
{
class BaseRegister
{
public:
  BaseRegister() = default;
  virtual ~BaseRegister() = default;

  virtual void reset() = 0;
};

template <typename RegistryType, typename RegisterType>
class IRegister : public BaseRegister
{
public:
  IRegister() = default;
  virtual ~IRegister()
  {
    reset();
  }

  IRegister(const IRegister<RegistryType, RegisterType> &) = delete;
  IRegister &operator=(const IRegister<RegistryType, RegisterType> &) = delete;

  void reset()
  {
    setTypeIndex(std::numeric_limits<u32>::max());
  }

  friend class IRegistry<RegistryType>;

protected:
  static void setTypeIndex(const u32 typeindex)
  {
    sTypeIndex = typeindex;
  }

  static u32 getTypeIndex()
  {
    return sTypeIndex;
  }

private:
  static u32 sTypeIndex = std::numeric_limits<u32>::max();
};
} // namespace EZIEngine

#endif
