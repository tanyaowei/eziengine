#ifndef _EZI_SINGLETON_HPP_
#define _EZI_SINGLETON_HPP_

namespace EZIEngine
{
template <typename T>
class ISingleton
{
protected:
  ISingleton() = default;
  ISingleton(const ISingleton &) = default;
  ISingleton &operator=(const ISingleton &) = default;

public:
  virtual ~ISingleton() = default;

  static T *getInstance()
  {
    static T instance;
    return &instance;
  }
};
} // namespace EZIEngine

#endif
