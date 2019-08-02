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

class SubObject
{
public:
  SubObject() :s(3.1417), e(5), g(ORANGE){}
  virtual ~SubObject(){}

  EZIReflection()

  double s;
  std::vector<Fruits> e;
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

class SubObject3
{
  public:
  SubObject3() :mPtr(new SubObject2()){}
  ~SubObject3(){}

  void setPtr(SubObject* val){ mPtr.reset(val); }
  SubObject* getPtr()const { return mPtr.get(); }

private:
  EZIReflection()
  std::unique_ptr<SubObject> mPtr;
};

class Object :public SubObject2, public SubObject3
{
public:
  Object() :a(7), b{1,2,3}{}
  ~Object(){ }
  
  EZIReflection(SubObject2, SubObject3)

  int a;
  double b[3];
};

template<typename T>
T conv_type(T* value, bool&ok)
{
    std::cout << "conv_type: " << *value << std::endl;
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

EZIReflectionRegistration
{
  EZIEngine::Reflection::type::register_converter_func(conv_type<bool>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<char>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<int8_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<int16_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<int32_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<int64_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<uint8_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<uint16_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<uint32_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<uint64_t>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<float>);
  EZIEngine::Reflection::type::register_converter_func(conv_type<double>);

  EZIEngine::Reflection::registration::enumeration<Fruits>("Fruits")
  (
    EZIEngine::Reflection::value("APPLE",Fruits::APPLE),
    EZIEngine::Reflection::value("ORANGE",Fruits::ORANGE),
    EZIEngine::Reflection::value("GRAPES",Fruits::GRAPES)
  );

   EZIEngine::Reflection::registration::class_<SubObject>("SubObject")
  .constructor<>()
  .property("SubObject::s", &SubObject::s)
  .property("SubObject::e", &SubObject::e)
  .property("SubObject::g", &SubObject::g);

   EZIEngine::Reflection::registration::class_<SubObject2>("SubObject2")
  .constructor<>()
  .property("SubObject2::f", &SubObject2::f);

  EZIEngine::Reflection::registration::class_<SubObject3>("SubObject3")
  .constructor<>()
  .property("SubObject3::mPtr", &SubObject3::getPtr, &SubObject3::setPtr);

   EZIEngine::Reflection::registration::class_<Object>("Object")
  .constructor<>()
  .property("Object::a", &Object::a)
  .property("Object::b", &Object::b);
}

#define TYPENAME(TYPE) typeid(TYPE).name()

template<typename T>
void print(T object, std::string prefix = "")
{
    EZIEngine::DataStream datastream = EZIEngine::write_datastream(object);

    EZIEngine::printDataStream(datastream, prefix);
}

int main()
{
  std::unique_ptr<SubObject> test = std::make_unique<Object>();
  print(test.get());

  return 0;
}