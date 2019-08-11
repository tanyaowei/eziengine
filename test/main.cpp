#include <iostream>
#include <memory>

#include "serialization/EZIDataStream.h"
#include "reflection/EZIReflection.h"
#include "serialization/EZISerialization.h"

enum Fruits
{
  APPLE = 0,
  ORANGE,
  GRAPES,
};

class SubObject3
{
  public:
  SubObject3() :mPtr(new Fruits(Fruits::APPLE)){}
  ~SubObject3(){}

  std::shared_ptr<Fruits> mPtr;

private:
  EZIReflection()

};

class SubObject
{
public:
  SubObject() :s(3.1417), e(5), g(ORANGE){}
  virtual ~SubObject(){}

  EZIReflection()

  void setE(std::vector<SubObject3*> val)
  { 
    e.clear();
    for(auto& elem: val)
    {
      e.emplace_back(elem);
    }
  }
  std::vector<SubObject3*> getE() const 
  { 
    std::vector<SubObject3*> result;
    for(auto& elem: e)
    {
      result.push_back(elem.get());
    }
    return result; 
  }
public:
  double s;
  std::vector<std::unique_ptr<SubObject3>> e;
  Fruits g;
};

class SubObject2: public SubObject
{
public:
  SubObject2() :f(1.2345){}
  virtual ~SubObject2(){}

private:
  EZIReflection(SubObject)
  double f;
};

class Object :public SubObject2, public SubObject3
{
public:
  Object() :a(7), b{1,2,3}{}
  ~Object(){ }
  
  EZIReflection(SubObject2, SubObject3)
public:
  int a;
  double b[3];
};

template<typename T>
T conv_ptr_type(T* value, bool&ok)
{
    if (value)
    {
        ok = true;
        return *value;
    }
    else
    {
        ok = false;
        return T();
    }
}

template<typename T>
T* conv_val_type(T value, bool&ok)
{
  ok = true;
  return new T(value);
}

EZIReflectionRegistration
{
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<bool>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<char>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<int8_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<int16_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<int32_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<int64_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<uint8_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<uint16_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<uint32_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<uint64_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<float>);
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<double>);

  /*EZIEngine::Reflection::type::register_converter_func(conv_val_type<bool>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<char>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<int8_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<int16_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<int32_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<int64_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<uint8_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<uint16_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<uint32_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<uint64_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<float>);
  EZIEngine::Reflection::type::register_converter_func(conv_val_type<double>);*/

  EZIEngine::Reflection::registration::enumeration<Fruits>("Fruits")
  (
    EZIEngine::Reflection::value("APPLE",Fruits::APPLE),
    EZIEngine::Reflection::value("ORANGE",Fruits::ORANGE),
    EZIEngine::Reflection::value("GRAPES",Fruits::GRAPES)
  );
  EZIEngine::Reflection::type::register_converter_func(conv_ptr_type<Fruits>);

   EZIEngine::Reflection::registration::class_<SubObject>("SubObject")
  .constructor<>()
  .property("SubObject::s", &SubObject::s)
  .property("SubObject::e", &SubObject::getE, &SubObject::setE)
  .property("SubObject::g", &SubObject::g);

   EZIEngine::Reflection::registration::class_<SubObject2>("SubObject2")
  .constructor<>()
  .property("SubObject2::f", &SubObject2::f);

  EZIEngine::Reflection::registration::class_<SubObject3>("SubObject3")
  .constructor<>()
  .property("SubObject3::mPtr", &SubObject3::mPtr);

   EZIEngine::Reflection::registration::class_<Object>("Object")
  .constructor<>()
  .property("Object::a", &Object::a)
  .property("Object::b", &Object::b);
}

#define TYPENAME(TYPE) typeid(TYPE).name()

template<typename T>
void print(T object, std::string prefix = "")
{
  EZIEngine::DataStream datastream;
  
  EZIEngine::write_datastream(datastream, object);

  EZIEngine::printDataStream(datastream, prefix);
}

int main()
{
  Object* obj = new Object();
  obj->mPtr.reset(new Fruits(Fruits::GRAPES));
  obj->a = 47;
  obj->e.clear();
  obj->e.emplace_back(new SubObject3());
  obj->e.emplace_back(new SubObject3());
  std::unique_ptr<SubObject> test(obj);

  //EZIEngine::DataStream datastream;
  //EZIEngine::write_datastream(datastream, test.get());
  //SubObject* temp = nullptr;
  //EZIEngine::read_datastream(datastream, temp);

  print(test.get());

  return 0;
}