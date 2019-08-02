#if 1
#ifndef _EZI_DATASTREAM_H_
#define _EZI_DATASTREAM_H_

#include <map>
#include <vector>
#include <string>
#include <iostream>

#include <reflection/EZIReflection.h>

namespace EZIEngine
{
  struct DataStream
  {
    enum DATATYPE
    {
      NONE = 0,
      VALUE,
      LIST,
      MAP,
      OBJECT
    };

    DataStream();
    ~DataStream();

    template <typename T>
    void setValue(const T& value) 
    {      
      mValue = std::to_string(value);
    }

    template <typename T>
    T getValue() const
    {
      T value;

      if (!mValue.empty())
      {
        if (std::is_integral<T>::value)
        {
          value = (!mValue.empty()) ? std::stoi(mValue) : 0;
        }
        else if (std::is_floating_point<T>::value)
        {
          value = (!mValue.empty()) ? std::stof(mValue) : 0.0f;
        }
      }
      else
      {
        value = T();
      }

      return value;
    }

    bool operator<(const DataStream& rhs ) const
    {
      if(mValue < rhs.mValue) return true;
      if(mList.size() < rhs.mList.size()) return true;
      if(mMap.size() < rhs.mMap.size()) return true;

      return false;
    }

    std::string mValue;
    std::vector<DataStream> mList;
    std::map<DataStream, DataStream> mMap;

    DATATYPE mType = DataStream::DATATYPE::NONE;
  };

  template <>
  void DataStream::setValue<bool>(const bool& value);

  template <>
  void DataStream::setValue<std::string>(const std::string& value);

  template <>
  bool DataStream::getValue<bool>() const;
  
  template <>
  std::string DataStream::getValue<std::string>() const;

  DataStream write_atomic_types(const Reflection::type& t, const Reflection::variant& var);

  DataStream write_object_types(const Reflection::type& t, const Reflection::instance& obj);

  DataStream write_sequential_container(const Reflection::variant_sequential_view& view);

  DataStream write_associative_container(const Reflection::variant_associative_view& view);

  DataStream write_variant(const Reflection::variant& var);

  DataStream write_datastream(Reflection::instance obj);

  void printDataStream(const DataStream& value, std::string prefix = "");
}

#endif
#endif