#if 0
#ifndef _EZI_SERIALIZATION_H_
#define _EZI_SERIALIZATION_H_

#include <string>
#include <serialization/EZIDataStream.h>
#include <reflection/EZIReflection.h>
//#include "jsoncpp\json.h"
//#include "tinyxml\tinyxml2.h"

namespace EZIEngine
{
  class Serializer
  {
  public:
    Serializer(const std::string& filename);
    virtual ~Serializer();

    template<typename T>
    bool serialize(T& object)
    {
      EZIEngine::DataStream temp;

      EZIEngine::MetaData* data = EZIEngine::ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

      data->dataStreamOut(reinterpret_cast<void*>(&object), temp);

      return baseSerialize(temp);
    }

    template<typename T>
    bool deserialize(T& object)
    {
      EZIEngine::DataStream temp;

      bool result = baseDeserialize(temp);

      if (result)
      {

        EZIEngine::MetaData* data = EZIEngine::ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

        data->dataStreamIn(reinterpret_cast<void*>(&object), temp);

      }

      return result;
    }

  protected:
    std::string mFileName;

  private:
    virtual bool baseSerialize(const DataStream& in) = 0;
    virtual bool baseDeserialize(DataStream& out) = 0;
  };

  /*class json_serializer: public serializer
  {
  public:
    json_serializer(const std::string& filename);
    ~json_serializer();

  private:
    bool base_serialize(const datastream& in);
    bool base_deserialize(datastream& out);

    void json_serialize_rec(Json::Value& json_in, const datastream& value);
    void json_deserialize_rec(const Json::Value& json_out, datastream& value);
  };

  class xml_serializer : public serializer
  {
  public:
    xml_serializer(const std::string& filename);
    ~xml_serializer();

  private:
    bool base_serialize(const datastream& in);
    bool base_deserialize(datastream& out);

    tinyxml2::XMLElement* xml_serialize_rec(const datastream& value, const std::string& name = "");
    void xml_deserialize_rec(const tinyxml2::XMLElement* xml_out, datastream& value);

    tinyxml2::XMLDocument m_doc;
  };*/
}

#endif
#endif