#include <iostream>

#include "EZIDataStream.h"
#include "EZIReflection.h"
#include "EZISerialization.h"

enum Fruits
{
  APPLE = 0,
  ORANGE,
  GRAPES,
};

EZIReflectionRegistration
{
  EZIEngine::Reflection::registration::enumeration<Fruits>("Fruits")
  (
    EZIEngine::Reflection::value("APPLE",Fruits::APPLE),
    EZIEngine::Reflection::value("ORANGE",Fruits::ORANGE),
    EZIEngine::Reflection::value("GRAPES",Fruits::GRAPES)
  )
}

class SubObject
{
public:
  SubObject() :s(3.1417), e(47), g(ORANGE){}
  ~SubObject(){}

  EZIReflection()

  double s;
  std::vector<Fruits> e;
  Fruits g;
};

EZIReflectionRegistration
{
   EZIEngine::Reflection::registration::class_<SubObject>("SubObject")
  .constructor<>()
  .property("s", &SubObject::s)
  .property("e", &SubObject::e)
  .property("g", &SubObject::g);
}

class SubObject2
{
public:
  SubObject2() :f(1.2345){}
  ~SubObject2(){}

  EZIReflection()
private:
  double f;
};

EZIReflectionRegistration
{
   EZIEngine::Reflection::registration::class_<SubObject2>("SubObject2")
  .constructor<>()
  .property("f", &SubObject2::f);
}

class Object :public SubObject2
{
public:
  Object() :a(0), b(3.14){}
  ~Object(){}
  
  EZIReflection(SubObject2)

  int a;
  double b;
  SubObject d;
};

EZIReflectionRegistration
{
   EZIEngine::Reflection::registration::class_<Object>("Object")
  .constructor<>()
  .property("a", &Object::a)
  .property("b", &Object::b)
  .property("d", &Object::d);
}

#define TYPENAME(TYPE) typeid(TYPE).name()

template<typename T>
void print(const std::string& name, T& object, std::string prefix = "")
{
    EZIEngine::DataStream datastream;

    EZIEngine::Reflection::type datatype = EZIEngine::Reflection::type::get<T>();

    data->dataStreamOut(reinterpret_cast<void*>(&object), temp);

    EZIEngine::printDataStream(name, temp, prefix);
}

int main()
{
  Object test;

  std::cout << std::is_enum<Fruits>::value << std::endl;

  print("test", test);

  return 0;
}