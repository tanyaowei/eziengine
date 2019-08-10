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
    std::string mObjType;
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

  void write_basic_types(DataStream& data, const Reflection::type& t, const Reflection::variant& var);

  void write_object_types(DataStream& data, const Reflection::type& t, const Reflection::instance& obj);

  void write_sequential_container(DataStream& data, const Reflection::variant_sequential_view& view);

  void write_associative_container(DataStream& data, const Reflection::variant_associative_view& view);

  void write_variant(DataStream& data, const Reflection::type&value_type,const Reflection::variant& var);

  template<typename T>
  void write_datastream(DataStream& data, const T& value)
  {
    Reflection::variant temp(value);

    write_variant(data, temp.get_type() ,temp);
  }

  void read_basic_types(const DataStream& data, const Reflection::type& t, Reflection::variant& var);

  void read_object_types(const DataStream& data, const Reflection::type& t, Reflection::instance& obj);

  void read_sequential_container(const DataStream& data, Reflection::variant_sequential_view& view);

  void read_associative_container(const DataStream& data, Reflection::variant_associative_view& view);

  void read_variant(const DataStream& data, const Reflection::type&value_type,Reflection::variant& var);

  void read_datastream(const DataStream& data, Reflection::instance obj);

  template<typename T>
  void read_datastream(const DataStream& data, T& value)
  {
    Reflection::variant temp(value);

    read_variant(data,temp.get_type() ,temp);
  }

  void printDataStream(const DataStream& value, std::string prefix = "");
}

#endif
#endif