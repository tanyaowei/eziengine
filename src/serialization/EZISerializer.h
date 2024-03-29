#ifndef _EZI_SERIALIZER_H_
#define _EZI_SERIALIZER_H_

#include <reflection/EZIReflection.h>

#include <ArduinoJson.h>

//#include <math/EZIType.h>
#include <iterator>
#include <iostream>
#include <typeinfo>
#include <forward_list>
#include <stack>

namespace EZIEngine
{
    class JsonSerializer: public Reflection::visitor
    {
        public:
        JsonSerializer(JsonObject jsonowner,const void* objptr)
        :mJsonOwner(jsonowner),mObjPtr(objptr){}

        template<typename Derived>
        void iterate_base_classes(JsonObject object)
        {
        }

        template<typename Derived, typename Base_Class, typename...Base_Classes>
        void iterate_base_classes(JsonObject object)
        {
            iterate_base_classes<Derived, Base_Classes...>(object);

            auto it = mTypeStack.begin();
            for(;it != mTypeStack.end(); ++it)
            {
                if(it->mName == get_type_name<Base_Class>())
                {
                    JsonObject base_obj = object.createNestedObject(it->mName);
                    base_obj.set(it->mJsonDoc.as<JsonObject>()[it->mName].as<JsonObject>());
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
            JsonObject object = temp.mJsonDoc.createNestedObject(temp.mName);
            iterate_base_classes<declaring_type_t, Base_Classes...>(object);
            mPtrOffset = 0;
            if(mTypeStack.empty() == false)
            {
                for(const auto& elem: mTypeStack)
                {
                    mPtrOffset+= elem.mSize;
                }
            }
            mTypeStack.push_back(temp);
            mJsonObj = mTypeStack.back().mJsonDoc.as<JsonObject>()[temp.mName].as<JsonObject>();
        }

        template<typename T, typename...Base_Classes>
        void visit_type_end(const type_info<T>& info)
        {
            mJsonOwner.set(mTypeStack.back().mJsonDoc.as<JsonObject>());
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

            write_types(mJsonObj[var_name].to<JsonVariant>() ,value_type,accessor);
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

            write_types(mJsonObj[var_name].to<JsonVariant>(),value_type,getter);
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

            write_types(mJsonObj[var_name].to<JsonVariant>() ,value_type,accessor);
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
                var.set(write_string_value(value));
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
        void write_sequential_range(JsonArray array, const Reflection::type& value_type,U start, U last)
        {
            size_t index = 0;

            for(auto it = start; it != last; ++it)
            {
                array.add(JsonVariant());

                write_types(array[index].to<JsonVariant>(),value_type, *it);

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
            JsonArray array = var.to<JsonArray>();
            write_sequential_range(array,value_type, start,last);
        }

        template<typename U, std::size_t SIZE>
        void write_sequential_container(JsonVariant var, const std::array<U, SIZE>* value)
        {
            std::cout << "std::array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_sequential_range(array,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::vector<U>* value)
        {
            std::cout << "std::vector:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_sequential_range(array,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::list<U>* value)
        {
            std::cout << "std::list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_sequential_range(array,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::forward_list<U>* value)
        {
            std::cout << "std::forward_list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_sequential_range(array,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(JsonVariant var, const std::deque<U>* value)
        {
            std::cout << "std::deque:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_sequential_range(array,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_keyvalue_range(JsonArray array
                                            , const Reflection::type& key_type
                                            , const Reflection::type& value_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                for(auto it = start; it != last; ++it)
                {
                    JsonObject obj = array.createNestedObject();
                    std::string key_name;
                    if(key_type.is_enumeration())
                    {
                        Reflection::enumeration enum_type = key_type.get_enumeration();
                        key_name = enum_type.value_to_name(it->first).to_string();
                    }
                    else
                    {
                        key_name = std::to_string(it->first);
                    }
                    write_types(obj[key_name].to<JsonVariant>(),value_type,it->second);
                }
            }
            else
            {
                std::cout << "write_associative_keyvalue_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

        template<typename U>
        void write_associative_key_range(JsonArray array, const Reflection::type& key_type
                                            ,U start, U last)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                size_t index = 0;

                for(auto it = start; it != last; ++it)
                {
                    array.add(JsonVariant());

                    write_basic_types(array[index].to<JsonVariant>(),key_type, *it);

                    ++index;
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
            JsonArray array = var.to<JsonArray>();
            write_associative_keyvalue_range(array,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(JsonVariant var, const std::multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();
            JsonArray array = var.to<JsonArray>();
            write_associative_keyvalue_range(array,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(JsonVariant var, const std::unordered_map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();
            JsonArray array = var.to<JsonArray>();
            write_associative_keyvalue_range(array,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U,typename V>
        void write_associative_container(JsonVariant var, const std::unordered_multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();
            JsonArray array = var.to<JsonArray>();
            write_associative_keyvalue_range(array,key_type,value_type, value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_associative_key_range(array,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_associative_key_range(array,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::unordered_set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_associative_key_range(array,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void write_associative_container(JsonVariant var, const std::unordered_multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.to<JsonArray>();
            write_associative_key_range(array,key_type,value->cbegin(),value->cend());
        }

        template<typename U>
        void* write_arithmetic_value(const U&)
        {
            return nullptr;
        }

        bool write_arithmetic_value(const bool& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        char write_arithmetic_value(const char& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        int8_t write_arithmetic_value(const int8_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        int16_t write_arithmetic_value(const int16_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        int32_t write_arithmetic_value(const int32_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        int64_t write_arithmetic_value(const int64_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        uint8_t write_arithmetic_value(const uint8_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        uint16_t write_arithmetic_value(const uint16_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        uint32_t write_arithmetic_value(const uint32_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        uint64_t write_arithmetic_value(const uint64_t& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        float write_arithmetic_value(const float& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        double write_arithmetic_value(const double& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        template<typename U>
        std::string write_string_value(const U& value)
        {
            std::cout << "UNHANDLE!!! write_string_types" << std::endl;
            return std::string();
        }

        std::string write_string_value(const std::string& value)
        {
            std::cout << value << std::endl;
            return value;
        }

        template<typename U>
        void write_basic_types(JsonVariant var, const Reflection::type& value_type, const U& value)
        {
            if (value_type.is_arithmetic())
            {
                std::cout << "is_arithmetic" << std::endl;
                var.set(write_arithmetic_value(value));
            }
            else if (value_type.is_enumeration())
            {

                std::cout << "is_enumeration" << std::endl;
                Reflection::enumeration enum_type = value_type.get_enumeration();
                std::string enum_string = enum_type.value_to_name(value).to_string();
                var.set(write_string_value(enum_string));
            }
            else if (value_type == Reflection::type::get<std::string>())
            {
                std::cout << "is_string" << std::endl;
                var.set(write_string_value(value));
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
}
#endif