#include <iostream>

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
  ~SubObject(){}

  EZIReflection()

  double s;
  std::vector<Fruits> e;
  Fruits g;
};

class SubObject2: public SubObject
{
public:
  SubObject2() :f(1.2345){}
  ~SubObject2(){}

private:
  EZIReflection(SubObject)
  double f;
};

class SubObject3
{
  public:
  SubObject3() :mPtr(new int(2345)){}
  ~SubObject3(){}

private:
  EZIReflection()
  std::unique_ptr<int> mPtr;
};

class Object :public SubObject2, public SubObject3
{
public:
  Object() :a(0), b{1,2,3}, d(new SubObject()){}
  ~Object(){ delete d; }
  
  EZIReflection(SubObject2, SubObject3)

  int a;
  double b[3];
  SubObject *d;
};

EZIReflectionRegistration
{
  EZIEngine::Reflection::registration::enumeration<Fruits>("Fruits")
  (
    EZIEngine::Reflection::value("APPLE",Fruits::APPLE),
    EZIEngine::Reflection::value("ORANGE",Fruits::ORANGE),
    EZIEngine::Reflection::value("GRAPES",Fruits::GRAPES)
  );

   EZIEngine::Reflection::registration::class_<SubObject>("SubObject")
  .constructor<>()
  .property("s", &SubObject::s)
  .property("e", &SubObject::e)
  .property("g", &SubObject::g);

   EZIEngine::Reflection::registration::class_<SubObject2>("SubObject2")
  .constructor<>()
  .property("f", &SubObject2::f);

   EZIEngine::Reflection::registration::class_<Object>("Object")
  .constructor<>()
  .property("a", &Object::a)
  .property("b", &Object::b)
  .property("d", &Object::d);
}

#define TYPENAME(TYPE) typeid(TYPE).name()

template<typename T>
void print(T object, std::string prefix = "")
{
    EZIEngine::DataStream datastream = EZIEngine::write_variant(EZIEngine::Reflection::variant(object));

    EZIEngine::printDataStream(datastream, prefix);
}

int main()
{
  Object test;
  SubObject* temp = &test;
  print(temp);

  return 0;
}