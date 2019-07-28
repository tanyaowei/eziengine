#if 0
#include <fstream>
#include <serialization/EZISerialization.h>

namespace EZIEngine
{
Serializer::Serializer(const std::string &filename)
    : mFileName(filename) {}

Serializer::~Serializer() {}

///////////////////JSON//////////////////////

/*json_serializer::json_serializer(const std::string& filename)
    :serializer(filename){}

  json_serializer::~json_serializer(){}

  bool json_serializer::base_serialize(const datastream& in)
  {
    bool result = false;

    std::ofstream fout(m_filename, std::ios_base::out | std::ios_base::trunc);

    if (fout)
    {
      Json::Value root;

      json_serialize_rec(root, in);

      Json::StyledWriter writer;

      fout << writer.write(root);

      result = true;
    }

    fout.close();

    return result;
  }

  bool json_serializer::base_deserialize(datastream& out)
  {
    bool result = false;

    std::ifstream fin(m_filename, std::ios_base::in);

    if (fin)
    {
      Json::Value root;

      Json::Reader reader;

      reader.parse(fin, root);

      json_deserialize_rec(root, out);

      result = true;
    }

    fin.close();

    return result;
  }

  void json_serializer::json_serialize_rec(Json::Value& json_in, const datastream& value)
  {
    switch (value.m_type)
    {
    case datastream::datatype::VALUE:
      json_in = value.m_value;
      break;
    case datastream::datatype::OBJECT:
    {
      json_in = Json::ValueType::objectValue;
      for (auto& elem : value.m_map)
      {
        json_serialize_rec(json_in[elem.first], elem.second);
      }
    }
      break;
    case datastream::datatype::ARRAY:
    {
      json_in = Json::ValueType::arrayValue;
      for (size_t i = 0; i < value.m_map.size(); ++i)
      {
        Json::Value temp;

        std::map<std::string, datastream>::const_iterator cit = value.m_map.find(std::to_string(i));

        json_serialize_rec(temp, cit->second);

        json_in.append(temp);
      }
    }
      break;
    default:
      break;
    }
  }

  void json_serializer::json_deserialize_rec(const Json::Value& json_out, datastream& value)
  {
    if (json_out.isObject())
    {
      value.m_type = datastream::datatype::OBJECT;

      for (const auto& elem : json_out.getMemberNames())
      {
        datastream temp;

        json_deserialize_rec(json_out[elem], temp);

        value.m_map[elem] = temp;
      }
    }
    else if (json_out.isArray())
    {
      value.m_type = datastream::datatype::ARRAY;

      size_t counter = 0;

      for (const auto& elem : json_out)
      {
        datastream temp;

        json_deserialize_rec(elem, temp);

        value.m_map[std::to_string(counter)] = temp;

        ++counter;
      }
    }
    else
    {
      value.m_type = datastream::datatype::VALUE;

      value.m_value = json_out.asString();
    }
  }*/

///////////////////XML//////////////////////

/*xml_serializer::xml_serializer(const std::string& filename)
    :serializer(filename){}

  xml_serializer::~xml_serializer(){}

  bool xml_serializer::base_serialize(const datastream& in)
  {
    m_doc.InsertEndChild(xml_serialize_rec(in));

    bool result = (m_doc.SaveFile(m_filename.c_str()) == tinyxml2::XML_NO_ERROR);

    m_doc.Clear();

    return result;
  }

  bool xml_serializer::base_deserialize(datastream& out)
  {
    bool result = (m_doc.LoadFile(m_filename.c_str()) == tinyxml2::XML_NO_ERROR);

    if (result)
    {
      xml_deserialize_rec(m_doc.RootElement(), out);
    }

    m_doc.Clear();

    return result;
  }

  tinyxml2::XMLElement*  xml_serializer::xml_serialize_rec(const datastream& value, const std::string& name)
  {
    switch (value.m_type)
    {
    case datastream::datatype::VALUE:
    {
      tinyxml2::XMLElement* val = m_doc.NewElement("ezi_value");
      val->SetAttribute("name", name.c_str());
      val->SetText(value.m_value.c_str());
      return val;
    }
      break;
    case datastream::datatype::OBJECT:
    {
      tinyxml2::XMLElement* obj = m_doc.NewElement("ezi_object");
      obj->SetAttribute("name", name.c_str());
      for (auto& elem : value.m_map)
      {
        obj->InsertEndChild(xml_serialize_rec(elem.second, elem.first));
      }
      return obj;
    }
    break;
    case datastream::datatype::ARRAY:
    {
      tinyxml2::XMLElement* arr = m_doc.NewElement("ezi_array");
      arr->SetAttribute("name", name.c_str());
      for (size_t i = 0; i < value.m_map.size(); ++i)
      {
        std::map<std::string, datastream>::const_iterator cit = value.m_map.find(std::to_string(i));

        arr->InsertEndChild(xml_serialize_rec(cit->second, cit->first));
      }
      return arr;
    }
      break;
    default:
      break;
    }
  }

  void xml_serializer::xml_deserialize_rec(const tinyxml2::XMLElement* xml_out, datastream& value)
  {
    if (std::string(xml_out->Name()) == std::string("ezi_object"))
    {
      value.m_type = datastream::datatype::OBJECT;

      const tinyxml2::XMLElement* elem = xml_out->FirstChildElement();

      while (elem)
      {
        datastream temp;

        xml_deserialize_rec(elem, temp);

        value.m_map[elem->Attribute("name")] = temp;

        elem = elem->NextSiblingElement();
      }
    }
    else if (std::string(xml_out->Name()) == std::string("ezi_array"))
    {
      value.m_type = datastream::datatype::ARRAY;

      const tinyxml2::XMLElement* elem = xml_out->FirstChildElement();

      while (elem)
      {
        datastream temp;

        xml_deserialize_rec(elem, temp);

        value.m_map[elem->Attribute("name")] = temp;

        elem = elem->NextSiblingElement();
      }
    }
    else
    {
      value.m_type = datastream::datatype::VALUE;

      value.m_value = xml_out->GetText();
    }
  }*/
} // namespace EZIEngine
#endif