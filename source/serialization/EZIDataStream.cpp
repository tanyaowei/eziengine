#include <serialization/EZIDataStream.h>

namespace EZIEngine
{
DataStream::DataStream()
    : mType(DATATYPE::NONE) {}

DataStream::~DataStream() {}

void printDataStream(const std::string &name, const DataStream &value, std::string prefix)
{
  std::cout << prefix << name << ": ";

  switch (value.mType)
  {
  case DataStream::DATATYPE::VALUE:
    std::cout << value.mValue << std::endl;
    break;
  case DataStream::DATATYPE::OBJECT:
    std::cout << "{" << std::endl;
    for (auto &elem : value.mMap)
    {
      printDataStream(elem.first, elem.second, prefix + '\t');
    }
    std::cout << prefix << "}" << std::endl;
    break;
  case DataStream::DATATYPE::ARRAY:
    std::cout << "[" << std::endl;
    for (size_t i = 0; i < value.mMap.size(); ++i)
    {
      std::map<std::string, DataStream>::const_iterator cit = value.mMap.find(std::to_string(i));

      printDataStream(cit->first, cit->second, prefix + '\t');
    }
    std::cout << prefix << "]" << std::endl;
    break;
  default:
    break;
  }
}

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

} // namespace EZIEngine