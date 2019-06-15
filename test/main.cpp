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

EZIReflectionEnum(Fruits)
{
  EZIAddEnumValue(APPLE);
  EZIAddEnumValue(ORANGE);
  EZIAddEnumValue(GRAPES);
}

class SubObject
{
public:
  SubObject() :s(3.1417), e(47), g(ORANGE){}
  ~SubObject(){}

  EZIReflectionDeclaration(SubObject)

  double s;
  std::vector<Fruits> e;
  Fruits g;
};

EZIReflectionDefinition(SubObject)
{
  EZIAddVectorMember(e);
  EZIAddBasicMember(s);
  EZIAddEnumMember(g);
}

class SubObject2
{
public:
  SubObject2() :f(1.2345){}
  ~SubObject2(){}

  EZIReflectionDeclaration(SubObject2)
private:
  double f;
};

EZIReflectionDefinition(SubObject2)
{
  EZIAddBasicMember(f);
}

class Object :public SubObject2
{
public:
  Object() :a(0), b(3.14){}
  ~Object(){}
  
  EZIReflectionDeclaration(Object)

  int a;
  double b;
  SubObject d;
};

EZIReflectionDefinition(Object)
{
  EZIAddUserDefineBaseClass(SubObject2);
  EZIAddBasicMember(a);
  EZIAddBasicMember(b);
  EZIAddUserDefineMember(d);
}

#define TYPENAME(TYPE) typeid(TYPE).name()

template<typename T>
void print(const std::string& name, T& object, std::string prefix = "")
{
    EZIEngine::DataStream temp;

    EZIEngine::MetaData* data = EZIEngine::ReflectionManager::getInstance()->getMetaData(typeid(T).hash_code());

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