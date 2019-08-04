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

  void write_basic_types(DataStream& data, const Reflection::type& t, const Reflection::variant& var)
  {
      data.mType = DataStream::DATATYPE::VALUE;

      if (t.is_arithmetic())
      {
        if (t == Reflection::type::get<bool>())
            data.setValue(var.to_bool());
        else if (t == Reflection::type::get<char>())
            data.setValue(var.to_int8());
        else if (t == Reflection::type::get<int8_t>())
            data.setValue(var.to_int8());
        else if (t == Reflection::type::get<int16_t>())
            data.setValue(var.to_int16());
        else if (t == Reflection::type::get<int32_t>())
            data.setValue(var.to_int32());
        else if (t == Reflection::type::get<int64_t>())
            data.setValue(var.to_int64());
        else if (t == Reflection::type::get<uint8_t>())
            data.setValue(var.to_uint8());
        else if (t == Reflection::type::get<uint16_t>())
            data.setValue(var.to_uint16());
        else if (t == Reflection::type::get<uint32_t>())
            data.setValue(var.to_uint32());
        else if (t == Reflection::type::get<uint64_t>())
            data.setValue(var.to_uint64());
        else if (t == Reflection::type::get<float>())
            data.setValue(var.to_float());
        else if (t == Reflection::type::get<double>())
            data.setValue(var.to_double());
      }
      else if (t.is_enumeration())
      {
        bool ok = false;
        auto enumstr = var.to_string(&ok);
        if (ok)
        {
            data.setValue(enumstr);
        }
        else
        {
            ok = false;
            auto value = var.to_uint64(&ok);
            if (ok)
                data.setValue(value);
            else
                data.setValue(std::string());
        }
      }
      else if (t == Reflection::type::get<std::string>())
      {
        data.setValue(var.to_string());
      }
  }

  void write_object_types(DataStream& data, const Reflection::type& t, const Reflection::instance & obj)
  {
    data.mType = DataStream::DATATYPE::OBJECT;

    for (const auto& prop : t.get_properties())
    {
      if (prop.get_metadata("NO_SERIALIZE")) continue;

      Reflection::variant prop_value = prop.get_value(obj);

      // cannot serialize, because we cannot retrieve the value
      if (!prop_value) continue; 

      DataStream key, val;

      key.setValue(prop.get_name().to_string());

      write_variant(val, prop_value.get_type(), prop_value);

      data.mMap.emplace(key,val);
    }
  }

  void write_sequential_container(DataStream& data, const Reflection::variant_sequential_view& view)
  {
    data.mType = DataStream::DATATYPE::LIST;

    for(const auto& item : view)
    {
      DataStream temp;
      
      write_variant(temp, item.get_type(), item);

      data.mList.push_back(temp);
    }
  }

  void write_associative_container(DataStream& data, const Reflection::variant_associative_view& view)
  {
    if (view.is_key_only_type())
    {
      data.mType = DataStream::DATATYPE::LIST;

      for(const auto& item :view)
      {
        DataStream temp;
        
        write_variant(temp, item.first.get_type(), item.first);

        data.mList.push_back(temp);
      }
    }
    else
    {
      data.mType = DataStream::DATATYPE::MAP;
      
      for(const auto& item :view)
      {
        DataStream key, val;
        
        write_variant(key, item.first.get_type(),item.first);
        write_variant(val, item.second.get_type(),item.second);

        data.mMap.emplace(key,val);
      }
    }
  }

  void write_variant(DataStream& data, const Reflection::type&value_type, const Reflection::variant& var)
  {
    if(value_type.is_pointer())
    {
      write_variant(data, value_type.get_raw_type(),var);
    }
    else if(value_type.is_wrapper())
    {
      write_variant(data, value_type.get_wrapped_type(),var.extract_wrapped_value());
    }
    else if (value_type.is_arithmetic() || value_type.is_enumeration()
          || value_type == Reflection::type::get<std::string>() )
    {
      Reflection::variant temp(var);

      if(temp.convert(value_type))
      {
        write_basic_types(data, value_type, var); 
      }
    }
    else if(value_type.is_sequential_container())
    {
      write_sequential_container(data, var.create_sequential_view());
    }
    else if(value_type.is_associative_container())
    {
      write_associative_container(data, var.create_associative_view());
    }
    else // object
    {
      Reflection::instance obj = Reflection::instance(var);

      Reflection::type obj_type = obj.get_derived_type();

      auto child_props = obj_type.get_properties();

      if(!child_props.empty())
      {
        write_object_types(data, obj_type,obj);
      }
    }
  }

  void write_datastream(DataStream& data, Reflection::instance obj)
  {
    write_object_types(data, obj.get_derived_type(),obj);
  }

  void read_basic_types(const DataStream& data, const Reflection::type& t, Reflection::variant& var)
  {
      if (t.is_arithmetic())
      {
        if (t == Reflection::type::get<bool>())
            var = data.getValue<bool>();
        else if (t == Reflection::type::get<char>())
            var = data.getValue<char>();
        else if (t == Reflection::type::get<int8_t>())
            var = data.getValue<int8_t>();
        else if (t == Reflection::type::get<int16_t>())
            var = data.getValue<int16_t>();
        else if (t == Reflection::type::get<int32_t>())
            var = data.getValue<int32_t>();
        else if (t == Reflection::type::get<int64_t>())
            var = data.getValue<int64_t>();
        else if (t == Reflection::type::get<uint8_t>())
            var = data.getValue<uint8_t>();
        else if (t == Reflection::type::get<uint16_t>())
            var = data.getValue<uint16_t>();
        else if (t == Reflection::type::get<uint32_t>())
            var = data.getValue<uint32_t>();
        else if (t == Reflection::type::get<uint64_t>())
            var = data.getValue<uint64_t>();
        else if (t == Reflection::type::get<float>())
            var = data.getValue<float>();
        else if (t == Reflection::type::get<double>())
            var = data.getValue<double>();
      }
      else if (t.is_enumeration())
      {
        bool ok = false;
        if (ok)
        {
            var = data.getValue<std::string>();
        }
        else
        {
            ok = false;
            if (ok)
                var = data.getValue<uint64_t>();
            else
                var = data.getValue<std::string>();
        }
      }
      else if (t == Reflection::type::get<std::string>())
      {
        var = data.getValue<std::string>();
      }
  }

  void read_object_types(const DataStream& data, const Reflection::type& t,  Reflection::instance& obj)
  {
    for (const auto& prop : t.get_properties())
    {
      if (prop.get_metadata("NO_SERIALIZE")) continue;

      DataStream key;

      key.setValue(prop.get_name().to_string());

      auto it = data.mMap.find(key);

      if(it != data.mMap.end())
      {
        Reflection::variant prop_value;

        read_variant(it->second, prop_value.get_type(), prop_value);

        // cannot serialize, because we cannot retrieve the value
        if (!prop_value) continue; 

        prop.set_value(obj,prop_value);
      }
    }
  }

  void read_sequential_container(const DataStream& data,  Reflection::variant_sequential_view& view)
  {
    view.set_size(data.mList.size());
    const Reflection::type value_type = view.get_rank_type(1);
    for(size_t i = 0; i < data.mList.size(); ++i)
    {
      Reflection::variant temp;
      read_variant(data.mList[i], value_type,temp);
      view.set_value(i, temp);
    }
  }

  void read_associative_container(const DataStream& data,  Reflection::variant_associative_view& view)
  {
    switch (data.mType)
    {
    case DataStream::DATATYPE::LIST:
    {
      const Reflection::type key_type = view.get_key_type();
      for(size_t i = 0; i < data.mList.size(); ++i)
      {
        Reflection::variant key;
        read_variant(data.mList[i], key_type,key);
        view.insert(key);
      }
    }
    break;
    case DataStream::DATATYPE::MAP:
    {
      const Reflection::type key_type = view.get_key_type();
      const Reflection::type value_type = view.get_value_type();
      for(const auto& elem: data.mMap)
      {
        Reflection::variant key, val;
        read_variant(elem.first, key_type,key);
        read_variant(elem.second, value_type,val);
        view.insert(key,val);
      }
    }
    break;
    }
  }

  void read_variant(const DataStream& data, const Reflection::type&value_type, Reflection::variant& var)
  {
    if(value_type.is_pointer())
    {
      read_variant(data, value_type.get_raw_type(),var);

      var.convert(value_type);
    }
    else if(value_type.is_wrapper())
    {
      read_variant(data, value_type.get_wrapped_type(),var);

      var.convert(value_type);
    }
    else if (value_type.is_arithmetic() || value_type.is_enumeration()
          || value_type == Reflection::type::get<std::string>() )
    {
      read_basic_types(data, value_type, var); 
    }
    else if(value_type.is_sequential_container())
    {
      Reflection::variant_sequential_view view = var.create_sequential_view();

      read_sequential_container(data, view);
    }
    else if(value_type.is_associative_container())
    {
      Reflection::variant_associative_view view = var.create_associative_view();

      read_associative_container(data, view);
    }
    else // object
    {
      Reflection::instance obj = Reflection::instance(var);

      Reflection::type obj_type = obj.get_derived_type();

      auto child_props = obj_type.get_properties();

      if(!child_props.empty())
      {
        read_object_types(data, obj_type,obj);
      }
    }
  }

  void read_datastream(const DataStream& data, Reflection::instance obj)
  {
    read_object_types(data,obj.get_derived_type(),obj);
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