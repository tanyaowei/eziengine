#include <reflection/EZIReflection.h>
#include <rttr/registration>
using namespace rttr;

namespace EZIEngine
{
MetaMember::MetaMember(metaid id, const std::string &name, size_t offset)
    : mMetaID(id), mName(name), mOffset(offset) {}

MetaBaseClass::MetaBaseClass(metaid id, const size_t offset)
    : mMetaID(id), mOffset(offset) {}

MetaData::MetaData(metaid id, const std::string &type, size_t size)
    : mMetaID(id), mType(type), mSize(size) {}

MetaData::~MetaData() {}

ReflectionManager::~ReflectionManager()
{
  for (const auto &elem : mMap)
  {
    delete elem.second;
  }

  mMap.clear();
}

MetaData *ReflectionManager::getMetaData(metaid id)
{
  std::map<metaid, MetaData *>::iterator it = mMap.find(id);

  if (it == mMap.end())
  {
    return nullptr;
  }

  return it->second;
}
} // namespace EZIEngine