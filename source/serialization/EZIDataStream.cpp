#if 1
#include <serialization/EZIDataStream.h>

namespace EZIEngine
{
DataStream::DataStream()
    : mType(DATATYPE::NONE) {}

DataStream::~DataStream() {}

template <>
void DataStream::setValue<bool>(const bool &value)
{
  mValue = std::to_string(value);
}

template <>
void DataStream::setValue<std::string>(const std::string &value)
{
  mValue = value;
}

template <>
bool DataStream::getValue<bool>() const
{
  return std::stoi(mValue);
}

template <>
std::string DataStream::getValue<std::string>() const
{
  return mValue;
}

  DataStream write_atomic_types(const Reflection::type& t, const Reflection::variant& var)
  {
      DataStream result;

      result.mType = DataStream::DATATYPE::VALUE;

      if (t.is_arithmetic())
      {
        if (t == Reflection::type::get<bool>())
            result.setValue(var.to_bool());
        else if (t == Reflection::type::get<char>())
            result.setValue(var.to_bool());
        else if (t == Reflection::type::get<int8_t>())
            result.setValue(var.to_int8());
        else if (t == Reflection::type::get<int16_t>())
            result.setValue(var.to_int16());
        else if (t == Reflection::type::get<int32_t>())
            result.setValue(var.to_int32());
        else if (t == Reflection::type::get<int64_t>())
            result.setValue(var.to_int64());
        else if (t == Reflection::type::get<uint8_t>())
            result.setValue(var.to_uint8());
        else if (t == Reflection::type::get<uint16_t>())
            result.setValue(var.to_uint16());
        else if (t == Reflection::type::get<uint32_t>())
            result.setValue(var.to_uint32());
        else if (t == Reflection::type::get<uint64_t>())
            result.setValue(var.to_uint64());
        else if (t == Reflection::type::get<float>())
            result.setValue(var.to_double());
        else if (t == Reflection::type::get<double>())
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
                result.setValue(std::string());
        }
      }
      else if (t == Reflection::type::get<std::string>())
      {
        result.setValue(var.to_string());
      }

      return result;
  }

  DataStream write_object_types(const Reflection::type& t, const Reflection::variant& var)
  {
    DataStream result;

    result.mType = DataStream::DATATYPE::OBJECT;

    for (const auto& prop : t.get_properties())
    {
      if (prop.get_metadata("NO_SERIALIZE")) continue;

      Reflection::variant prop_value = prop.get_value(var);

      // cannot serialize, because we cannot retrieve the value
      if (!prop_value) continue; 

      DataStream key;

      key.mValue = prop.get_name().to_string();

      DataStream val = write_variant(prop_value);

      result.mMap.insert(std::make_pair(key,val));
    }

    return result;
  }

  DataStream write_sequential_container(const Reflection::variant_sequential_view& view)
  {
    DataStream result;

    result.mType = DataStream::DATATYPE::LIST;

    for(const auto& item : view)
    {
      DataStream temp = write_variant(item.extract_wrapped_value());

      result.mList.push_back(temp);
    }

    return result;
  }

  DataStream write_associative_container(const Reflection::variant_associative_view& view)
  {
    DataStream result;

    if (view.is_key_only_type())
    {
      result.mType = DataStream::DATATYPE::LIST;

      for(const auto& item :view)
      {
        DataStream temp = write_variant(item.first);

        result.mList.push_back(temp);
      }
    }
    else
    {
      result.mType = DataStream::DATATYPE::MAP;
      
      for(const auto& item :view)
      {
        DataStream key = write_variant(item.first);

        DataStream val = write_variant(item.second);

        result.mMap.emplace(key,val);
      }
    }

    return result;
  }

  DataStream write_variant(const Reflection::variant& var)
  {
    auto value_type = var.get_type();

    if(value_type.is_wrapper() == true)
    {
      return write_variant(var.extract_wrapped_value());
    }
    else if (value_type.is_arithmetic() || value_type.is_enumeration()
          || value_type == Reflection::type::get<std::string>() )
    {
      return write_atomic_types(value_type, var);
    }
    else // list, map, object
    {
      if(var.is_sequential_container())
      {
        return write_sequential_container(var.create_sequential_view());
      }
      else if(var.is_associative_container())
      {
        return write_associative_container(var.create_associative_view());
      }
      else // object
      {
        return write_object_types(value_type,var);
      }
    }
    
    return DataStream();
  }

  void printDataStream(const DataStream &value, std::string prefix)
  {
    switch (value.mType)
    {
    case DataStream::DATATYPE::VALUE:
      std::cout << prefix << value.mValue;
      break;
    case DataStream::DATATYPE::OBJECT:
      std::cout << "{" << std::endl;
      for (auto &elem : value.mMap)
      {
        std::cout << prefix << elem.first.mValue << ": ";

        printDataStream(elem.second, prefix + '\t');

        std::cout << std::endl;
      }
      std::cout << prefix << "}" << std::endl;
      break;
    case DataStream::DATATYPE::LIST:
      std::cout << "[" << std::endl;
      for (size_t i = 0; i < value.mList.size(); ++i)
      {
        printDataStream(value.mList[i], prefix + '\t');

        std::cout << std::endl;
      }
      std::cout << prefix << "]" << std::endl;
      break;
    case DataStream::DATATYPE::MAP:
      std::cout << "[" << std::endl;
      if(value.mList.empty() == false)
      {
        for (size_t i = 0; i < value.mList.size(); ++i)
        {
          printDataStream(value.mList[i], prefix + '\t');

          std::cout << std::endl;
        }
      }
      else
      {
        for (auto &elem : value.mMap)
        {
          std::cout << "{" << std::endl;
          std::cout << prefix << "KEY: ";
          printDataStream(elem.first, prefix + '\t');
          std::cout << std::endl;
          std::cout << prefix << "VALUE: ";
          printDataStream(elem.second, prefix + '\t');
          std::cout << std::endl;
          std::cout << prefix << "}" << std::endl;
        }
      }
      std::cout << prefix << "]" << std::endl;
      break;
    default:
      break;
    }
  }

} // namespace EZIEngine
#endif