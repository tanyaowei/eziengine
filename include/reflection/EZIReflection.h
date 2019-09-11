#ifndef _EZI_REFLECTION_H_
#define _EZI_REFLECTION_H_

#include <rttr/visitor.h>

namespace EZIEngine
{
    class Serializer;
    class Deserializer;
}

RTTR_REGISTER_VISITOR(EZIEngine::Serializer);
RTTR_REGISTER_VISITOR(EZIEngine::Deserializer);

#include <rttr/registration>
#include <rttr/registration_friend>
#include <rttr/enumeration.h>
#include <rttr/type>

#include <ArduinoJson.h>

//#include <math/EZIType.h>
#include <iterator>
#include <iostream>
#include <typeinfo>
#include <forward_list>
#include <stack>

namespace EZIEngine
{
    namespace Reflection = rttr;
}

#define EZIReflection(...)                \
RTTR_REGISTRATION_FRIEND                  \
RTTR_ENABLE(__VA_ARGS__)

#define EZIReflectionRegistration          \
RTTR_REGISTRATION

namespace EZIEngine
{
    class Serializer: public Reflection::visitor
    {
        public:
        Serializer(const void* objptr)
        :mObjPtr(objptr){}

        template<typename Derived>
        void iterate_base_classes()
        {
        }

        template<typename Derived, typename Base_Class, typename...Base_Classes>
        void iterate_base_classes()
        {
            iterate_base_classes<Derived, Base_Classes...>();

            auto it = mTypeStack.begin();
            for(;it != mTypeStack.end(); ++it)
            {
                if(it->mName == typeid(Base_Class).name())
                {
                    break;
                }
            }
            mTypeStack.erase(it,mTypeStack.end());
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename...Base_Classes>
        void visit_type_begin(const type_info<T>& info)
        {
            std::cout << "{" << std::endl;
            using declaring_type_t = typename type_info<T>::declaring_type;
            iterate_base_classes<declaring_type_t, Base_Classes...>();
            mPtrOffset = 0;
            if(mTypeStack.empty() == false)
            {
                for(const auto& elem: mTypeStack)
                {
                    mPtrOffset+= elem.mSize;
                }
            }
            std::cout << "mPtrOffset: " << mPtrOffset << std::endl;
            ClassEntry temp;
            temp.mName = typeid(declaring_type_t).name();
            temp.mSize = sizeof(declaring_type_t);
            mTypeStack.push_back(temp);
        }

        template<typename T, typename...Base_Classes>
        void visit_type_end(const type_info<T>& info)
        {
            std::cout << "}" << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename...Ctor_Args>
        void visit_constructor(const constructor_info<T>& info)
        {
            std::cout << "visit_constructor: ";
            using declaring_type_t = typename constructor_info<T>::declaring_type;
            std::cout << get_type_name<declaring_type_t>() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void visit_global_method(const method_info<T>& info)
        {
            std::cout << "visit_global_method: ";
            std::cout << info.method_item.get_name().to_string() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void visit_method(const method_info<T>& info)
        {
            std::cout << "visit_method: ";
            std::cout << info.method_item.get_name().to_string() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void visit_property(const property_info<T>& info)
        {
            std::cout << "visit_property: ";
            std::cout << info.property_item.get_name().to_string() << " - ";
            std::cout << info.property_item.get_type().get_name().to_string() << std::endl;

            using declaring_type_t = typename property_info<T>::declaring_type;
            Reflection::type value_type = info.property_item.get_type();
            const char* charptr = reinterpret_cast<const char*>(mObjPtr);
            const auto& accessor = reinterpret_cast<const declaring_type_t*>( charptr + mPtrOffset)->*info.property_accessor;

            write_types(value_type,accessor);
        }  

        template<typename T>
        void visit_getter_setter_property(const property_getter_setter_info<T>& info)
        {
            std::cout << "visit_getter_setter_property: ";
            std::cout << std::string("get_") + info.property_item.get_name().to_string() << " - ";
            std::cout << std::string("set_") + info.property_item.get_name().to_string() << " - ";
            std::cout << info.property_item.get_type().get_name().to_string() << std::endl;

            using declaring_type_t = typename property_getter_setter_info<T>::declaring_type;
            Reflection::type value_type = info.property_item.get_type();
            const char* charptr = reinterpret_cast<const char*>(mObjPtr);
            const auto& getter = (reinterpret_cast<const declaring_type_t*>(charptr + mPtrOffset)->*info.property_getter)();

            write_types(value_type,getter);
        }

        template<typename T>
        void visit_readonly_property(const property_info<T>& info)
        {
            std::cout << "visit_readonly_property: ";
            std::cout << info.property_item.get_name().to_string() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

private:
        template<typename T>
        static std::string get_type_name()
        {
            return Reflection::type::template get<T>().get_name().to_string();
        }

        template<typename U>
        void write_pointer_types(const Reflection::type& value_type, const U& value)
        {
            std::cout << "UNHANDLE!!! write_pointer: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void write_pointer_types(const Reflection::type& value_type,U* value)
        {
            std::cout << "pointer: " << typeid(U).name() << std::endl;

            if(value != nullptr)
            {
                write_types(value_type.get_raw_type(), *value);
            }
        }

        template<typename U>
        void write_pointer_types(const Reflection::type& value_type,const U* value)
        {
            std::cout << "pointer: " << typeid(U).name() << std::endl;

            if(value != nullptr)
            {
                write_types(value_type.get_raw_type(), *value);
            }
        }

        template<typename U>
        void write_wrapper_types(const Reflection::type& value_type,const U& value)
        {
            std::cout << "UNHANDLE!!! write_wrapper: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void write_wrapper_types(const Reflection::type& value_type,std::shared_ptr<U>& value)
        {
            std::cout << "std::shared_ptr: " << typeid(U).name() << std::endl;
            write_types(value_type.get_wrapped_type().get_raw_type(), *value);
        }

        template<typename U>
        void write_wrapper_types(const Reflection::type& value_type,const std::shared_ptr<U>& value)
        {
            std::cout << "std::shared_ptr: " << typeid(U).name() << std::endl;
            write_types(value_type.get_wrapped_type().get_raw_type(), *value);
        }

        template<typename U>
        void write_sequential_range(const Reflection::type& value_type,U start, U last)
        {
            for(auto it = start; it != last; ++it)
            {
                write_types(value_type, *it);
            }
        }

        template<typename U>
        void write_sequential_container(const U& value)
        {
            std::cout << "UNHANDLE!!! write_sequential_container" << std::endl;
        }

        template<typename U, std::size_t SIZE>
        void write_sequential_container(U(*value)[SIZE])
        {
            std::cout << "array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            auto start = std::begin(*value);
            auto last = std::end(*value);
            write_sequential_range(value_type, start,last);
        }

        template<typename U, std::size_t SIZE>
        void write_sequential_container(const std::array<U, SIZE>* value)
        {
            std::cout << "std::array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::vector<U>* value)
        {
            std::cout << "std::vector:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::list<U>* value)
        {
            std::cout << "std::list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::forward_list<U>* value)
        {
            std::cout << "std::forward_list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::deque<U>* value)
        {
            std::cout << "std::deque:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            print_iterator_range(value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_keyvalue_range(const Reflection::type& key_type
                                            , const Reflection::type& value_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                for(auto it = start; it != last; ++it)
                {
                    std::cout << "key: ";
                    write_basic_types(key_type, it->first);
                    std::cout << "value: ";
                    write_types(value_type,it->second);
                }
            }
            else
            {
                std::cout << "write_associative_keyvalue_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

        template<typename U>
        void write_associative_key_range(const Reflection::type& key_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                for(auto it = start; it != last; ++it)
                {
                    std::cout << "key: ";
                    write_basic_types(key_type, *it);
                }
            }
            else
            {
                std::cout << "write_associative_keyvalue_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

       template<typename U>
        void write_associative_container(const U& value)
        {
            std::cout << "UNHANDLE!!! write_associative_container" << std::endl;
        }

        template<typename U,typename V>
        void write_associative_container(const std::map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(const std::multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(const std::unordered_map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(const std::unordered_multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(const std::set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(const std::multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(const std::unordered_set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(const std::unordered_multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_arithmetic_types(const U& value)
        {
            std::cout << "UNHANDLE!!! write_arithmetic_types" << std::endl;
        }

        void write_arithmetic_types(const bool& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const char& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const int8_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const int16_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const int32_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const int64_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const uint8_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const uint16_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const uint32_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const uint64_t& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const float& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const double& value)
        {
            std::cout << value << std::endl;
        }

        template<typename U>
        void write_string_types(const U& value)
        {
            std::cout << "UNHANDLE!!! write_string_types" << std::endl;
        }

        void write_string_types(const std::string& value)
        {
            std::cout << value << std::endl;
        }

        template<typename U>
        void write_basic_types(const Reflection::type& value_type,U& value)
        {
            if (value_type.is_arithmetic())
            {
                std::cout << "is_arithmetic" << std::endl;
                write_arithmetic_types(value);
            }
            else if (value_type.is_enumeration())
            {

                std::cout << "is_enumeration" << std::endl;
                Reflection::enumeration enum_type = value_type.get_enumeration();
                std::string enum_string = enum_type.value_to_name(value).to_string();
                write_string_types(enum_string);
            }
            else if (value_type == Reflection::type::get<std::string>())
            {
                std::cout << "is_string" << std::endl;
                write_string_types(value);
            }
        }

        template<typename U>
        void write_types(const Reflection::type& value_type, const U& value)
        {
            if(value_type.is_pointer())
            {
                std::cout << "is_pointer" << std::endl;
                write_pointer_types(value_type, value);
            }
            else if(value_type.is_wrapper())
            {
                std::cout << "is_wrapper" << std::endl;
                write_wrapper_types(value_type, value);
            }
            else if (  value_type.is_arithmetic() || value_type.is_enumeration()
                || value_type == Reflection::type::get<std::string>())
            {
                write_basic_types(value_type, value); 
            }
            else if(value_type.is_sequential_container())
            {
                std::cout << "is_sequential_container" << std::endl;
                write_sequential_container(&value);
            }
            else if(value_type.is_associative_container())
            {
                std::cout << "is_associative_container" << std::endl;
                write_associative_container(&value);
            }
            else // object
            {
                std::cout << "is_object" << std::endl;
                Serializer serializer(&value);
                serializer.visit(Reflection::instance(value).get_derived_type());
            }
        }

private:
        EZIReflection(visitor)

        struct ClassEntry
        {
            std::string mName;
            size_t mSize = 0;
        };
         
        std::vector<ClassEntry> mTypeStack;

        size_t mPtrOffset = 0;
        const void* mObjPtr = nullptr;
    };

    class Deserializer: public Reflection::visitor
    {
        public:

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename...Base_Classes>
        void visit_type_begin(const type_info<T>& info)
        {
            std::cout << "visit_type_begin" << std::endl;
        }

        template<typename T, typename...Base_Classes>
        void visit_type_end(const type_info<T>& info)
        {
            std::cout << "visit_type_end" << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename...Ctor_Args>
        void visit_constructor(const constructor_info<T>& info)
        {
            std::cout << "visit_constructor: ";
            using declaring_type_t = typename constructor_info<T>::declaring_type;
            std::cout << get_type_name<declaring_type_t>() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void visit_global_method(const method_info<T>& info)
        {
            std::cout << "visit_global_method: ";
            std::cout << info.method_item.get_name().to_string() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void visit_method(const method_info<T>& info)
        {
            std::cout << "visit_method: ";
            std::cout << info.method_item.get_name().to_string() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T>
        void visit_property(const property_info<T>& info)
        {
            std::cout << "visit_property: ";
            std::cout << info.property_item.get_name().to_string() << std::endl;
        }

        template<typename T>
        void visit_getter_setter_property(const property_getter_setter_info<T>& info)
        {
            std::cout << "visit_getter_setter_property: ";
            std::cout << std::string("get_") + info.property_item.get_name().to_string() << std::endl;
            std::cout << std::string("set_") + info.property_item.get_name().to_string() << std::endl;
        }

        template<typename T>
        void visit_readonly_property(const property_info<T>& info)
        {
            std::cout << "visit_readonly_property: ";
            std::cout << info.property_item.get_name().to_string() << std::endl;
        }

    /////////////////////////////////////////////////////////////////////////////////////

private:
        template<typename T>
        static std::string get_type_name()
        {
            return Reflection::type::template get<T>().get_name().to_string();
        }

private:

        EZIReflection(visitor)
    };
}

#endif