#ifndef _EZI_REFLECTION_H_
#define _EZI_REFLECTION_H_

#include <rttr/visitor.h>

namespace EZIEngine
{
    class JsonSerializer;
    class JsonDeserializer;
}

RTTR_REGISTER_VISITOR(EZIEngine::JsonSerializer);
RTTR_REGISTER_VISITOR(EZIEngine::JsonDeserializer);

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
    class JsonSerializer: public Reflection::visitor
    {
        public:
        JsonSerializer(JsonObject jsonowner,const void* objptr)
        :mJsonOwner(jsonowner),mObjPtr(objptr){}

        template<typename Derived>
        void iterate_base_classes(JsonArray array)
        {
        }

        template<typename Derived, typename Base_Class, typename...Base_Classes>
        void iterate_base_classes(JsonArray array)
        {
            iterate_base_classes<Derived, Base_Classes...>(array);

            auto it = mTypeStack.begin();
            for(;it != mTypeStack.end(); ++it)
            {
                if(it->mName == get_type_name<Base_Class>())
                {
                    array.add(it->mJsonDoc.as<JsonObject>());
                    break;
                }
            }
            mTypeStack.erase(it);
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename...Base_Classes>
        void visit_type_begin(const type_info<T>& info)
        {
            using declaring_type_t = typename type_info<T>::declaring_type;

            ClassEntry temp;
            temp.mName = get_type_name<declaring_type_t>();
            temp.mSize = sizeof(declaring_type_t);
            temp.mJsonDoc["CLASS_TYPE_NAME"] = get_type_name<declaring_type_t>(); 
            JsonArray array = temp.mJsonDoc.createNestedArray("BASE_CLASSES");
            temp.mJsonDoc.createNestedObject("PROPERTIES");
            iterate_base_classes<declaring_type_t, Base_Classes...>(array);
            mPtrOffset = 0;
            if(mTypeStack.empty() == false)
            {
                for(const auto& elem: mTypeStack)
                {
                    mPtrOffset+= elem.mSize;
                }
            }
            mTypeStack.push_back(temp);
            mJsonObj = mTypeStack.back().mJsonDoc.as<JsonObject>();
        }

        template<typename T, typename...Base_Classes>
        void visit_type_end(const type_info<T>& info)
        {
            mJsonOwner.set(mJsonObj);
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

            std::string var_name = info.property_item.get_name().to_string();

            JsonObject obj = mJsonObj["PROPERTIES"].as<JsonObject>();

            write_types(obj[var_name].to<JsonVariant>() ,value_type,accessor);
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

            std::string var_name = info.property_item.get_name().to_string();

            JsonObject obj = mJsonObj["PROPERTIES"].as<JsonObject>();

            write_types(obj[var_name].to<JsonVariant>(),value_type,getter);
        }

        template<typename T>
        void visit_readonly_property(const property_info<T>& info)
        {
            std::cout << "visit_readonly_property: ";
            std::cout << info.property_item.get_name().to_string() << std::endl;

            using declaring_type_t = typename property_info<T>::declaring_type;
            Reflection::type value_type = info.property_item.get_type();
            const char* charptr = reinterpret_cast<const char*>(mObjPtr);
            const auto& accessor = reinterpret_cast<const declaring_type_t*>( charptr + mPtrOffset)->*info.property_accessor;

            std::string var_name = info.property_item.get_name().to_string();

            JsonObject obj = mJsonObj["PROPERTIES"].as<JsonObject>();

            write_types(obj[var_name].to<JsonVariant>() ,value_type,accessor);
        }

    /////////////////////////////////////////////////////////////////////////////////////

private:
        template<typename T>
        static std::string get_type_name()
        {
            return Reflection::type::template get<T>().get_name().to_string();
        }

        template<typename U>
        void write_pointer_types(JsonVariant var, const Reflection::type& value_type, const U& value)
        {
            std::cout << "UNHANDLE!!! write_pointer_types: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void write_pointer_types(JsonVariant var, const Reflection::type& value_type,const U* value)
        {
            std::cout << "pointer: " << typeid(U).name() << std::endl;

            if(value != nullptr)
            {
                write_types(var, value_type.get_raw_type(), *value);
            }
        }

        void write_pointer_types(JsonVariant var, const Reflection::type& value_type,const char* value)
        {
            if(value != nullptr)
            {
                write_string_types(var, value);
            }
        }

        template<typename U>
        void write_wrapper_types(JsonVariant var, const Reflection::type& value_type,const U& value)
        {
            std::cout << "UNHANDLE!!! write_wrapper: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void write_wrapper_types(JsonVariant var, const Reflection::type& value_type,const std::shared_ptr<U>& value)
        {
            std::cout << "std::shared_ptr: " << typeid(U).name() << std::endl;
            write_types(var,value_type.get_wrapped_type().get_raw_type(), *value);
        }

        template<typename U>
        void write_sequential_range(JsonVariant var, const Reflection::type& value_type,U start, U last)
        {
            JsonArray array = var.to<JsonArray>();

            size_t index = 0;

            for(auto it = start; it != last; ++it)
            {
                array.add(0);

                write_types(array[index],value_type, *it);

                ++index;
            }
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! write_sequential_container" << std::endl;
        }

        template<typename U, std::size_t SIZE>
        void write_sequential_container(JsonVariant var, const U(*value)[SIZE])
        {
            std::cout << "array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            auto start = std::begin(*value);
            auto last = std::end(*value);
            write_sequential_range(var,value_type, start,last);
        }

        template<typename U, std::size_t SIZE>
        void write_sequential_container(JsonVariant var, const std::array<U, SIZE>* value)
        {
            std::cout << "std::array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(var,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::vector<U>* value)
        {
            std::cout << "std::vector:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(var,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::list<U>* value)
        {
            std::cout << "std::list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(var,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::forward_list<U>* value)
        {
            std::cout << "std::forward_list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(var,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::deque<U>* value)
        {
            std::cout << "std::deque:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            write_sequential_range(var,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_keyvalue_range(JsonVariant var
                                            , const Reflection::type& key_type
                                            , const Reflection::type& value_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                JsonArray array = var.to<JsonArray>();

                for(auto it = start; it != last; ++it)
                {
                    JsonObject obj = array.createNestedObject();
                    write_basic_types(obj["KEY"].to<JsonVariant>(),key_type, it->first);
                    write_types(obj["VALUE"].to<JsonVariant>(),value_type,it->second);
                }
            }
            else
            {
                std::cout << "write_associative_keyvalue_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

        template<typename U>
        void write_associative_key_range(JsonVariant var, const Reflection::type& key_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                JsonArray array = var.to<JsonArray>();

                for(auto it = start; it != last; ++it)
                {
                    JsonObject obj = array.createNestedObject();
                    write_basic_types(obj["KEY"].to<JsonVariant>(),key_type, *it);
                }
            }
            else
            {
                std::cout << "write_associative_key_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

       template<typename U>
        void write_associative_container(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! write_associative_container" << std::endl;
        }

        template<typename U,typename V>
        void write_associative_container(JsonVariant var, const std::map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(var,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(JsonVariant var, const std::multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(var,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(JsonVariant var, const std::unordered_map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(var,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(JsonVariant var, const std::unordered_multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            write_associative_keyvalue_range(var,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(var,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(var,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::unordered_set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(var,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::unordered_multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            write_associative_key_range(var,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_arithmetic_types(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! write_arithmetic_types" << std::endl;
        }

        void write_arithmetic_types(JsonVariant var, const bool& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const char& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const int8_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const int16_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const int32_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const int64_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const uint8_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const uint16_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const uint32_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const uint64_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const float& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void write_arithmetic_types(JsonVariant var, const double& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        template<typename U>
        void write_string_types(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! write_string_types" << std::endl;
        }

        void write_string_types(JsonVariant var, const std::string& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        template<typename U>
        void write_basic_types(JsonVariant var, const Reflection::type& value_type, const U& value)
        {
            if (value_type.is_arithmetic())
            {
                std::cout << "is_arithmetic" << std::endl;
                write_arithmetic_types(var,value);
            }
            else if (value_type.is_enumeration())
            {

                std::cout << "is_enumeration" << std::endl;
                Reflection::enumeration enum_type = value_type.get_enumeration();
                std::string enum_string = enum_type.value_to_name(value).to_string();
                write_string_types(var,enum_string);
            }
            else if (value_type == Reflection::type::get<std::string>())
            {
                std::cout << "is_string" << std::endl;
                write_string_types(var,value);
            }
        }

        template<typename U>
        void write_types(JsonVariant var, const Reflection::type& value_type, const U& value)
        {
            if(value_type.is_pointer())
            {
                std::cout << "is_pointer" << std::endl;
                write_pointer_types(var,value_type, value);
            }
            else if(value_type.is_wrapper())
            {
                std::cout << "is_wrapper" << std::endl;
                write_wrapper_types(var,value_type, value);
            }
            else if (  value_type.is_arithmetic() || value_type.is_enumeration()
                || value_type == Reflection::type::get<std::string>())
            {
                write_basic_types(var,value_type, value); 
            }
            else if(value_type.is_sequential_container())
            {
                std::cout << "is_sequential_container" << std::endl;
                write_sequential_container(var,&value);
            }
            else if(value_type.is_associative_container())
            {
                std::cout << "is_associative_container" << std::endl;
                write_associative_container(var,&value);
            }
            else // object
            {
                std::cout << "is_object" << std::endl;
                JsonObject obj = var.to<JsonObject>();
                JsonSerializer serializer(obj,&value);
                serializer.visit(Reflection::instance(value).get_derived_type());
            }
        }

private:
        EZIReflection(visitor)

        struct ClassEntry
        {
            std::string mName;
            size_t mSize = 0;
            StaticJsonDocument<1 << 20> mJsonDoc;
        };
         
        std::vector<ClassEntry> mTypeStack;
        JsonObject mJsonOwner;
        JsonObject mJsonObj;

        size_t mPtrOffset = 0;
        const void* mObjPtr = nullptr;
    };

    class JsonDeserializer: public Reflection::visitor
    {
        public:
        JsonDeserializer(JsonObject jsonowner,void* objptr)
        :mJsonOwner(jsonowner),mObjPtr(objptr){}

        template<typename Derived>
        void iterate_base_classes(JsonArray array)
        {
        }

        template<typename Derived, typename Base_Class, typename...Base_Classes>
        void iterate_base_classes(JsonArray array)
        {
            iterate_base_classes<Derived, Base_Classes...>(array);

            auto it = mTypeStack.begin();
            for(;it != mTypeStack.end(); ++it)
            {
                if(it->mName == get_type_name<Base_Class>())
                {
                    array.add(it->mJsonDoc.as<JsonObject>());
                    break;
                }
            }
            mTypeStack.erase(it);
        }
    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename...Base_Classes>
        void visit_type_begin(const type_info<T>& info)
        {
            std::cout << "visit_type_begin" << std::endl;
            using declaring_type_t = typename type_info<T>::declaring_type;

            ClassEntry temp;
            temp.mName = get_type_name<declaring_type_t>();
            temp.mSize = sizeof(declaring_type_t);
            temp.mJsonDoc["CLASS_TYPE_NAME"] = get_type_name<declaring_type_t>(); 
            JsonArray array = temp.mJsonDoc.createNestedArray("BASE_CLASSES");
            temp.mJsonDoc.createNestedObject("PROPERTIES");
            iterate_base_classes<declaring_type_t, Base_Classes...>(array);
            mPtrOffset = 0;
            if(mTypeStack.empty() == false)
            {
                for(const auto& elem: mTypeStack)
                {
                    mPtrOffset+= elem.mSize;
                }
            }
            mTypeStack.push_back(temp);
            mJsonObj = mTypeStack.back().mJsonDoc.as<JsonObject>();
        }

        template<typename T, typename...Base_Classes>
        void visit_type_end(const type_info<T>& info)
        {
            std::cout << "visit_type_end" << std::endl;
            mJsonOwner.set(mJsonObj);
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

            using declaring_type_t = typename property_info<T>::declaring_type;
            Reflection::type value_type = info.property_item.get_type();
            char* charptr = reinterpret_cast<char*>(mObjPtr);
            auto& accessor = reinterpret_cast<declaring_type_t*>( charptr + mPtrOffset)->*info.property_accessor;

            std::string var_name = info.property_item.get_name().to_string();

            JsonObject obj = mJsonObj["PROPERTIES"].as<JsonObject>();

            read_types(obj[var_name].to<JsonVariant>() ,value_type,accessor);
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

        template<typename U>
        void read_pointer_types(JsonVariant var, const Reflection::type& value_type, const U& value)
        {
            std::cout << "UNHANDLE!!! read_pointer: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void read_pointer_types(JsonVariant var, const Reflection::type& value_type, U*& value)
        {
            std::cout << "pointer: " << typeid(U).name() << std::endl;

            if(value != nullptr)
            {
                delete value;
                value = nullptr;
            }

            value = new U();

            read_types(var, value_type.get_raw_type(), *value);
        }

        void read_pointer_types(JsonVariant var, const Reflection::type& value_type, char*& value)
        {
            if(value != nullptr)
            {
                read_string_types(var, value);
            }
        }

        template<typename U>
        void read_wrapper_types(JsonVariant var, const Reflection::type& value_type,const U& value)
        {
            std::cout << "UNHANDLE!!! read_wrapper: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void read_wrapper_types(JsonVariant var, const Reflection::type& value_type, std::shared_ptr<U>& value)
        {
            std::cout << "std::shared_ptr: " << typeid(U).name() << std::endl;
            value.reset(new U());
            read_types(var,value_type.get_wrapped_type().get_raw_type(), *value);
        }

        template<typename U>
        void read_sequential_range(JsonVariant var, const Reflection::type& value_type,U start, U last)
        {
            JsonArray array = var.to<JsonArray>();

            size_t index = 0;

            for(auto it = start; it != last; ++it)
            {
                array.add(0);

                read_types(array[index],value_type, *it);

                ++index;
            }
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! read_sequential_container" << std::endl;
        }

        template<typename U, std::size_t SIZE>
        void read_sequential_container(JsonVariant var, U(*value)[SIZE])
        {
            std::cout << "array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            auto start = std::begin(*value);
            auto last = std::end(*value);
            read_sequential_range(var,value_type, start,last);
        }

        template<typename U, std::size_t SIZE>
        void read_sequential_container(JsonVariant var, std::array<U, SIZE>* value)
        {
            std::cout << "std::array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            read_sequential_range(var,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::vector<U>* value)
        {
            std::cout << "std::vector:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            read_sequential_range(var,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::list<U>* value)
        {
            std::cout << "std::list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            read_sequential_range(var,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::forward_list<U>* value)
        {
            std::cout << "std::forward_list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            read_sequential_range(var,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::deque<U>* value)
        {
            std::cout << "std::deque:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            read_sequential_range(var,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_associative_keyvalue_range(JsonVariant var
                                            , const Reflection::type& key_type
                                            , const Reflection::type& value_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                JsonArray array = var.to<JsonArray>();

                for(auto it = start; it != last; ++it)
                {
                    JsonObject obj = array.createNestedObject();
                    read_basic_types(obj["KEY"].to<JsonVariant>(),key_type, it->first);
                    read_types(obj["VALUE"].to<JsonVariant>(),value_type,it->second);
                }
            }
            else
            {
                std::cout << "read_associative_keyvalue_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

        template<typename U>
        void read_associative_key_range(JsonVariant var, const Reflection::type& key_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                JsonArray array = var.to<JsonArray>();

                for(auto it = start; it != last; ++it)
                {
                    JsonObject obj = array.createNestedObject();
                    read_basic_types(obj["KEY"].to<JsonVariant>(),key_type, *it);
                }
            }
            else
            {
                std::cout << "read_associative_key_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

       template<typename U>
        void read_associative_container(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! read_associative_container" << std::endl;
        }

        template<typename U,typename V>
        void read_associative_container(JsonVariant var, std::map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            read_associative_keyvalue_range(var,key_type,value_type, value->begin(),value->end());
        }

        template<typename U,typename V>
        void read_associative_container(JsonVariant var, std::multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            read_associative_keyvalue_range(var,key_type,value_type, value->begin(),value->end());
        }

        template<typename U,typename V>
        void read_associative_container(JsonVariant var, std::unordered_map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            read_associative_keyvalue_range(var,key_type,value_type, value->begin(),value->end());
        }

        template<typename U,typename V>
        void read_associative_container(JsonVariant var, std::unordered_multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();

            read_associative_keyvalue_range(var,key_type,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            read_associative_key_range(var,key_type,value->begin(),value->end());
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            read_associative_key_range(var,key_type,value->begin(),value->end());
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::unordered_set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            read_associative_key_range(var,key_type,value->begin(),value->end());
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::unordered_multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            read_associative_key_range(var,key_type,value->begin(),value->end());
        }

        template<typename U>
        void read_arithmetic_types(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! read_arithmetic_types" << std::endl;
        }

        void read_arithmetic_types(JsonVariant var, bool& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, char& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, int8_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, int16_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, int32_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, int64_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, uint8_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, uint16_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, uint32_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, uint64_t& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, float& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        void read_arithmetic_types(JsonVariant var, double& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        template<typename U>
        void read_string_types(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! read_string_types" << std::endl;
        }

        void read_string_types(JsonVariant var, std::string& value)
        {
            std::cout << value << std::endl;
            var.set(value);
        }

        template<typename U>
        void read_basic_types(JsonVariant var, const Reflection::type& value_type,U& value)
        {
            if (value_type.is_arithmetic())
            {
                std::cout << "is_arithmetic" << std::endl;
                read_arithmetic_types(var,value);
            }
            else if (value_type.is_enumeration())
            {

                std::cout << "is_enumeration" << std::endl;
                Reflection::enumeration enum_type = value_type.get_enumeration();
                std::string enum_string = enum_type.value_to_name(value).to_string();
                read_string_types(var,enum_string);
            }
            else if (value_type == Reflection::type::get<std::string>())
            {
                std::cout << "is_string" << std::endl;
                read_string_types(var,value);
            }
        }

        template<typename U>
        void read_types(JsonVariant var, const Reflection::type& value_type, U& value)
        {
            if(value_type.is_pointer())
            {
                std::cout << "is_pointer" << std::endl;
                read_pointer_types(var,value_type, value);
            }
            else if(value_type.is_wrapper())
            {
                std::cout << "is_wrapper" << std::endl;
                read_wrapper_types(var,value_type, value);
            }
            else if (  value_type.is_arithmetic() || value_type.is_enumeration()
                || value_type == Reflection::type::get<std::string>())
            {
                read_basic_types(var,value_type, value); 
            }
            else if(value_type.is_sequential_container())
            {
                std::cout << "is_sequential_container" << std::endl;
                read_sequential_container(var,&value);
            }
            else if(value_type.is_associative_container())
            {
                std::cout << "is_associative_container" << std::endl;
                read_associative_container(var,&value);
            }
            else // object
            {
                std::cout << "is_object" << std::endl;
                JsonObject obj = var.to<JsonObject>();
                JsonSerializer serializer(obj,&value);
                serializer.visit(Reflection::instance(value).get_derived_type());
            }
        }

private:
        struct ClassEntry
        {
            std::string mName;
            size_t mSize = 0;
            StaticJsonDocument<1 << 20> mJsonDoc;
        };
         
        std::vector<ClassEntry> mTypeStack;
        JsonObject mJsonOwner;
        JsonObject mJsonObj;

        size_t mPtrOffset = 0;
        void* mObjPtr = nullptr;

        EZIReflection(visitor)
    };
}

#endif