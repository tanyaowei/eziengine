#if 0
#ifndef _EZI_DATASTREAM_H_
#define _EZI_DATASTREAM_H_

#include <map>
#include <vector>
#include <string>
#include <iostream>

namespace EZIEngine
{
  struct DataStream
  {
    enum DATATYPE
    {
      NONE = 0,
      VALUE,
      ARRAY,
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

  void printDataStream(const std::string& name, const DataStream& value, std::string prefix = "");
}

#endif
#endif