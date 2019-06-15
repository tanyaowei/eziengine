#ifndef _EZI_REFLECTION_H_
#define _EZI_REFLECTION_H_

#include <map>
#include <list>
#include <array>
#include <vector>
#include <string>
#include <typeinfo>
#include <type_traits>

#include <common/EZIType.h>
#include <serialization/EZIDataStream.h>
#include <template_pattern/EZISingleton.hpp>

namespace EZIEngine
{
using metaid = size_t;

struct MetaData;

class ReflectionManager : public ISingleton<ReflectionManager>
{
public:
  ~ReflectionManager();

  template <typename T, typename U>
  void registerMetaData(metaid id)
  {
    std::map<metaid, MetaData *>::iterator it = mMap.find(id);

    if (it == mMap.end())
    {
      mMap.emplace(id, new U(id, typeid(T).name(), sizeof(T)));
    }
  }

  MetaData *getMetaData(metaid);

private:
  ReflectionManager() = default;
  ReflectionManager(const ReflectionManager &) = default;
  ReflectionManager &operator=(const ReflectionManager &) = default;

  friend ISingleton<ReflectionManager>;

  std::map<metaid, MetaData *> mMap;
};

struct MetaMember
{
  MetaMember(metaid, const std::string &, size_t);

  metaid mMetaID = 0;
  std::string mName;
  size_t mOffset = 0;
};

struct MetaBaseClass
{
  MetaBaseClass(metaid, size_t);

  metaid mMetaID = 0;
  size_t mOffset = 0;
};

typedef std::pair<std::string, i32> MetaEnumValue;

struct MetaData
{
  MetaData(metaid, const std::string &, size_t);
  virtual ~MetaData();

  virtual void dataStreamOut(void *, DataStream &) = 0;
  virtual void dataStreamIn(void *, const DataStream &) = 0;

  template <typename T>
  bool isBasic()
  {
    return (!(std::is_enum<T>::value) && (std::is_fundamental<T>::value ||
                                          std::is_pointer<T>::value || typeid(T) == typeid(std::string)));
  }

  std::vector<MetaBaseClass> mBaseClasses;
  std::vector<MetaMember> mMembers;
  std::vector<MetaEnumValue> mEnumValues;
  std::string mType;
  std::string mClassName;
  size_t mSize = 0;
  metaid mMetaID = 0;
};

struct MetaObject : public MetaData
{
  MetaObject(metaid id, const std::string &type, size_t size)
      : MetaData(id, type, size) {}

  void dataStreamOut(void *ptr, DataStream &out)
  {
    out.mType = DataStream::DATATYPE::OBJECT;

    for (const auto &elem : mBaseClasses)
    {
      MetaData *data = ReflectionManager::getInstance()->getMetaData(elem.mMetaID);

      data->dataStreamOut(reinterpret_cast<u8 *>(ptr) + elem.mOffset, out);
    }

    for (const auto &elem : mMembers)
    {
      MetaData *data = ReflectionManager::getInstance()->getMetaData(elem.mMetaID);

      DataStream temp;

      data->dataStreamOut(reinterpret_cast<u8 *>(ptr) + elem.mOffset, temp);

      out.mMap[mClassName + elem.mName] = temp;
    }
  }

  void dataStreamIn(void *ptr, const DataStream &in)
  {
    for (const auto &elem : mBaseClasses)
    {
      MetaData *data = ReflectionManager::getInstance()->getMetaData(elem.mMetaID);

      data->dataStreamIn(reinterpret_cast<u8 *>(ptr) + elem.mOffset, in);
    }

    for (const auto &elem : mMembers)
    {
      MetaData *data = ReflectionManager::getInstance()->getMetaData(elem.mMetaID);

      std::map<std::string, DataStream>::const_iterator cit = in.mMap.find(mClassName + elem.mName);

      if (cit != in.mMap.end())
      {
        data->dataStreamIn(reinterpret_cast<u8 *>(ptr) + elem.mOffset, cit->second);
      }
    }
  }
};

template <typename T>
struct MetaBasic : public MetaData
{
  MetaBasic(metaid id, const std::string &type, size_t size)
      : MetaData(id, type, size) {}

  void dataStreamOut(void *ptr, DataStream &out)
  {
    out.mType = DataStream::DATATYPE::VALUE;

    T temp = *(reinterpret_cast<T *>(ptr));

    out.setValue(temp);
  }

  void dataStreamIn(void *ptr, const DataStream &in)
  {
    *(reinterpret_cast<T *>(ptr)) = in.getValue<T>();
  }
};

template <typename>
struct ArraySize;
template <typename T, size_t N>
struct ArraySize<std::array<T, N>>
{
  static size_t const size = N;
};

template <typename T, size_t N>
struct MetaArray : public MetaData
{
  MetaArray(metaid id, const std::string &type, size_t size)
      : MetaData(id, type, size)
  {
    if (MetaData::isBasic<T>())
    {
      //reflectionmgr::get_instance()->register_metadata<T, ezi::metabasic<T>>(typeid(T).hash_code());
    }
  }

  void dataStreamOut(void *ptr, DataStream &out)
  {
    out.mType = DataStream::DATATYPE::ARRAY;

    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    std::array<T, N> &arr = (*reinterpret_cast<std::array<T, N> *>(ptr));

    for (size_t i = 0; i < arr.size(); ++i)
    {
      DataStream temp;

      data->dataStreamOut(reinterpret_cast<void *>(&arr[i]), temp);

      out.mMap[std::to_string(i)] = temp;
    }
  }

  void dataStreamIn(void *ptr, const DataStream &in)
  {
    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    std::array<T, N> &arr = (*reinterpret_cast<std::array<T, N> *>(ptr));

    for (size_t i = 0; i < arr.size(); ++i)
    {
      std::map<std::string, DataStream>::const_iterator cit = in.mMap.find(std::to_string(i));

      if (cit != in.mMap.end())
      {
        data->dataStreamIn(reinterpret_cast<void *>(&arr[i]), cit->second);
      }
    }
  }
};

template <typename T>
struct MetaVector : public MetaData
{
  MetaVector(metaid id, const std::string &type, size_t size)
      : MetaData(id, type, size)
  {
    if (MetaData::isBasic<T>())
    {
      //reflectionmgr::get_instance()->register_metadata<T, ezi::metabasic<T>>(typeid(T).hash_code());
    }
  }

  void dataStreamOut(void *ptr, DataStream &out)
  {
    out.mType = DataStream::DATATYPE::ARRAY;

    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    std::vector<T> &vec = (*reinterpret_cast<std::vector<T> *>(ptr));

    for (size_t i = 0; i < vec.size(); ++i)
    {
      DataStream temp;

      data->dataStreamOut(reinterpret_cast<void *>(&vec[i]), temp);

      out.mMap[std::to_string(i)] = temp;
    }
  }

  void dataStreamIn(void *ptr, const DataStream &in)
  {
    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    std::vector<T> &vec = (*reinterpret_cast<std::vector<T> *>(ptr));

    vec.clear();

    for (size_t i = 0; i < in.mMap.size(); ++i)
    {
      std::map<std::string, DataStream>::const_iterator cit = in.mMap.find(std::to_string(i));

      if (cit != in.mMap.end())
      {
        T temp;

        data->dataStreamIn(reinterpret_cast<void *>(&temp), cit->second);

        vec.push_back(temp);
      }
    }
  }
};

template <typename T>
struct MetaList : public MetaData
{
  MetaList(metaid id, const std::string &type, size_t size)
      : MetaData(id, type, size)
  {
    if (MetaData::isBasic<T>())
    {
      //reflectionmgr::get_instance()->register_metadata<T, ezi::metabasic<T>>(typeid(T).hash_code());
    }
  }

  void dataStreamOut(void *ptr, DataStream &out)
  {
    out.mType = DataStream::DATATYPE::ARRAY;

    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    std::list<T> &list = (*reinterpret_cast<std::list<T> *>(ptr));

    size_t i = 0;

    for (auto &elem : list)
    {
      DataStream temp;

      data->dataStreamOut(reinterpret_cast<void *>(&elem), temp);

      out.mMap[std::to_string(i)] = temp;

      ++i;
    }
  }

  void dataStreamIn(void *ptr, const DataStream &in)
  {
    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    std::list<T> &list = (*reinterpret_cast<std::list<T> *>(ptr));

    list.clear();

    for (size_t i = 0; i < in.mMap.size(); ++i)
    {
      std::map<std::string, DataStream>::const_iterator cit = in.mMap.find(std::to_string(i));

      if (cit != in.mMap.end())
      {
        T temp;

        data->dataStreamIn(reinterpret_cast<void *>(&temp), cit->second);

        list.push_back(temp);
      }
    }
  }
};

template <typename T, typename U>
struct MetaMap : public MetaData
{
  MetaMap(metaid id, const std::string &type, size_t size)
      : MetaData(id, type, size)
  {
    if (MetaData::isBasic<T>())
    {
      //reflectionmgr::get_instance()->register_metadata<T, ezi::metabasic<T>>(typeid(T).hash_code());
    }

    if (MetaData::isBasic<U>())
    {
      //reflectionmgr::get_instance()->register_metadata<T, ezi::metabasic<U>>(typeid(U).hash_code());
    }
  }

  void dataStreamOut(void *ptr, DataStream &out)
  {
    out.mType = DataStream::DATATYPE::ARRAY;

    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(U).hash_code());

    std::map<T, U> &map = (*reinterpret_cast<std::map<T, U> *>(ptr));

    for (auto &elem : map)
    {
      DataStream temp;

      data->dataStreamOut(&elem.second, temp);

      out.mMap[elem.first] = temp;
    }
  }

  void dataStreamIn(void *ptr, const DataStream &in)
  {
    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(U).hash_code());

    std::map<T, U> &map = (*reinterpret_cast<std::map<T, U> *>(ptr));

    map.clear();

    for (auto &elem : in.mMap)
    {
      std::map<std::string, DataStream>::const_iterator cit = in.mMap.find(elem.first);

      if (cit != in.mMap.end())
      {
        U temp;

        data->dataStreamIn(&temp, elem.second);

        map.insert(elem.first, temp);
      }
    }
  }
};

template <typename T>
struct MetaEnum : public MetaData
{
  MetaEnum(metaid id, const std::string &type, size_t size)
      : MetaData(id, type, size) {}

  void dataStreamOut(void *ptr, DataStream &out)
  {
    out.mType = DataStream::DATATYPE::VALUE;

    i32 temp = (*reinterpret_cast<T *>(ptr));

    for (const auto &elem : mEnumValues)
    {
      if (temp == elem.second)
      {
        out.setValue(elem.first);
        break;
      }
    }
  }

  void dataStreamIn(void *ptr, const DataStream &in)
  {
    std::string temp = in.getValue<std::string>();

    for (const auto &elem : mEnumValues)
    {
      if (temp == elem.first)
      {
        *(reinterpret_cast<T *>(ptr)) = static_cast<T>(elem.second);
        break;
      }
    }
  }
};

template <typename T, typename U>
class Reflection
{
public:
  Reflection(const std::string &classname)
  {
    metaid id = typeid(T).hash_code();

    ReflectionManager::getInstance()->registerMetaData<T, U>(id);

    MetaData *data = ReflectionManager::getInstance()->getMetaData(id);

    data->mClassName = classname + std::string(".");

    initialize();
  }

  template <typename V, typename W>
  static void addMember(const std::string &name, size_t offset)
  {
    metaid tempid = typeid(V).hash_code();

    ReflectionManager::getInstance()->registerMetaData<V, W>(tempid);

    MetaMember temp(tempid, name, offset);

    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    if (data)
    {
      data->mMembers.push_back(temp);
    }
  }

  template <typename V, typename W>
  static void addBaseClass(size_t offset)
  {
    metaid tempid = typeid(V).hash_code();

    ReflectionManager::getInstance()->registerMetaData<V, W>(tempid);

    MetaBaseClass temp(tempid, offset);

    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    if (data)
    {
      data->mBaseClasses.push_back(temp);
    }
  }

  static void addEnumValue(const std::string &name, size_t value)
  {
    MetaData *data = ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

    if (data)
    {
      data->mEnumValues.push_back(MetaEnumValue(name, value));
    }
  }

  static T *nullCast()
  {
    return reinterpret_cast<T *>(static_cast<void *>(nullptr));
  }

  static T *oneCast()
  {
    return reinterpret_cast<T *>(1);
  }

  static void initialize();

  friend T;
};

} // namespace EZIEngine

#define EZIReflectionDeclaration(TYPE) \
  friend EZIEngine::Reflection<TYPE, EZIEngine::MetaObject>;

#define EZIReflectionDefinition(TYPE)                                                              \
  EZIEngine::Reflection<TYPE, EZIEngine::MetaObject> __FILE__##__LINE__##TYPE(std::string(#TYPE)); \
  template <>                                                                                      \
  void EZIEngine::Reflection<TYPE, EZIEngine::MetaObject>::initialize()

//#define eziReflectionBasic(TYPE) \
//  ezi::reflection<TYPE,ezi::metabasic<TYPE>> __FILE__##__LINE__##TYPE(std::string(#TYPE));\
//  void ezi::reflection<TYPE,ezi::metabasic<TYPE>>::initialize(){}

#define EZIReflectionEnum(NAME)                                                                        \
  EZIEngine::Reflection<NAME, EZIEngine::MetaEnum<NAME>> __FILE__##__LINE__##NAME(std::string(#NAME)); \
  template <>                                                                                          \
  void EZIEngine::Reflection<NAME, EZIEngine::MetaEnum<NAME>>::initialize()

#define EZIAddUserDefineBaseClass(TYPE) \
  addBaseClass<TYPE, EZIEngine::MetaObject>(reinterpret_cast<size_t>(dynamic_cast<TYPE *>(oneCast())) - 1);

#define EZIAddEnumValue(MEMBER) \
  addEnumValue(std::string(#MEMBER), MEMBER);

#define EZIAddBasicMember(MEMBER) \
  addMember<decltype(nullCast()->MEMBER), EZIEngine::MetaBasic<decltype(nullCast()->MEMBER)>>(std::string(#MEMBER), reinterpret_cast<size_t>(&(nullCast()->MEMBER)));

#define EZIAddEnumMember(MEMBER) \
  addMember<decltype(nullCast()->MEMBER), EZIEngine::MetaEnum<decltype(nullCast()->MEMBER)>>(std::string(#MEMBER), reinterpret_cast<size_t>(&(nullCast()->MEMBER)));

#define EZIAddUserDefineMember(MEMBER) \
  addMember<decltype(nullCast()->MEMBER), EZIEngine::MetaObject>(std::string(#MEMBER), reinterpret_cast<size_t>(&(nullCast()->MEMBER)));

#define EZIAddEZIEngineMember(MEMBER) \
  EZIAddUserDefineMember(MEMBER)

#define EZIAddArrayMember(MEMBER) \
  addMember<decltype(nullCast()->MEMBER), EZIEngine::MetaArray<decltype(nullCast()->MEMBER)::value_type, array_size<decltype(nullCast()->MEMBER)>::size>>(std::string(#MEMBER), reinterpret_cast<size_t>(&(null_cast()->MEMBER)));

#define EZIAddVectorMember(MEMBER) \
  addMember<decltype(nullCast()->MEMBER), EZIEngine::MetaVector<decltype(nullCast()->MEMBER)::value_type>>(std::string(#MEMBER), reinterpret_cast<size_t>(&(nullCast()->MEMBER)));

#define EZIAddListMember(MEMBER) \
  addMember<decltype(nullCast()->MEMBER), EZIEngine::MetaList<decltype(nullCast()->MEMBER)::value_type>>(std::string(#MEMBER), reinterpret_cast<size_t>(&(nullCast()->MEMBER)));

#define EZIAddMapMember(MEMBER) \
  addMember<decltype(nullCast()->MEMBER), EZIEngine::MetaMap<decltype(nullCast()->MEMBER)::key_type, decltype(nullCast()->MEMBER)::mapped_type>>(std::string(#MEMBER), reinterpret_cast<size_t>(&(null_cast()->MEMBER)));

#endif