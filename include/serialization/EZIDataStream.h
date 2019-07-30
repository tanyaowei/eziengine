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

    std::string mValue;
    std::vector<DataStream> mList;
    std::map<std::string, DataStream> mMap;

    DATATYPE mType;
  };

  template <>
  void DataStream::setValue<bool>(const bool& value);

  template <>
  void DataStream::setValue<std::string>(const std::string& value);

  template <>
  bool DataStream::getValue<bool>() const;
  
  template <>
  std::string DataStream::getValue<std::string>() const;

DataStream write_atomic_types(const EZIEngine::Reflection::type& t, const EZIEngine::Reflection::variant& var)
{
    DataStream result;

    result.mType = DataStream::DATATYPE::VALUE;

    if (t.is_arithmetic())
    {
      if (t == EZIEngine::Reflection::type::get<bool>())
          result.setValue(var.to_bool());
      else if (t == EZIEngine::Reflection::type::get<char>())
          result.setValue(var.to_bool());
      else if (t == EZIEngine::Reflection::type::get<int8_t>())
          result.setValue(var.to_int8());
      else if (t == EZIEngine::Reflection::type::get<int16_t>())
          result.setValue(var.to_int16());
      else if (t == EZIEngine::Reflection::type::get<int32_t>())
          result.setValue(var.to_int32());
      else if (t == EZIEngine::Reflection::type::get<int64_t>())
          result.setValue(var.to_int64());
      else if (t == EZIEngine::Reflection::type::get<uint8_t>())
          result.setValue(var.to_uint8());
      else if (t == EZIEngine::Reflection::type::get<uint16_t>())
          result.setValue(var.to_uint16());
      else if (t == EZIEngine::Reflection::type::get<uint32_t>())
          result.setValue(var.to_uint32());
      else if (t == EZIEngine::Reflection::type::get<uint64_t>())
          result.setValue(var.to_uint64());
      else if (t == EZIEngine::Reflection::type::get<float>())
          result.setValue(var.to_double());
      else if (t == EZIEngine::Reflection::type::get<double>())
          result.setValue(var.to_double());
    }
    else if (t.is_enumeration())
    {
      bool ok = false;
      auto enumstr = var.to_string(&ok);
      if (ok)
      {
          result.setValue(enumstr);
      }
      else
      {
          ok = false;
          auto value = var.to_uint64(&ok);
          if (ok)
              result.setValue(value);
          else
              result.setValue("");
      }
    }
    else if (t == EZIEngine::Reflection::type::get<std::string>())
    {
      result.setValue(var.to_string());
    }

    return result;
}

  template<typename T>
  DataStream write_variant(const EZIEngine::Reflection::variant& var)
  {
    auto value_type = var.get_type();

    if(value_type.is_wrapper() == true)
    {
      return write_variant(var.extract_wrapped_value());
    }
    else if (value_type.is_arithmetic() || value_type == type::get<std::string>() || value_type.is_enumeration())
    {
      return write_atomic_types(value_type, prop_value);
    }
    else // list, map, object
    {
      if(var.is_sequential_container())
      {
        DataStream result;
        for(const auto& item : var.create_sequential_view())
        {
          DataStream temp = write_variant(item.extract_wrapped_value());

          result.mList.push_back(temp);
        }
        return result;
      }
      else if(var.is_associative_container())
      {

      }
      else // object
      {
        DataStream result;

        for (const auto& prop : value_type.get_properties())
        {
          if (prop.get_metadata("NO_SERIALIZE")) continue;

          EZIEngine::Reflection::variant prop_value = prop.get_value(var);

          // cannot serialize, because we cannot retrieve the value
          if (!prop_value) continue; 

          DataStream temp = write_variant(prop_value);

          result.mMap.emplace(prop.get_name(),temp);
        }

        return result;
      }
    }
    
    return DataStream();
  }

  void printDataStream(const std::string& name, const DataStream& value, std::string prefix = "");
}

#endif
#endif