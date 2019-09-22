#ifndef _EZI_DESERIALIZER_H_
#define _EZI_DESERIALIZER_H_

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
    class JsonDeserializer: public Reflection::visitor
    {
        public:
        JsonDeserializer(JsonObject jsonowner,void* objptr)
        :mJsonOwner(jsonowner),mObjPtr(objptr){}

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
                if(it->mName == get_type_name<Base_Class>())
                {
                    break;
                }
            }
            mTypeStack.erase(it);
        }

        template<typename Derived>
        JsonObject find_base_class(JsonObject object)
        {
            Reflection::type curr_type = Reflection::type::get<Derived>();
            for(auto elem: object)
            {
                Reflection::type temp_type = Reflection::type::get_by_name(Reflection::string_view(elem.key().c_str()));
                
                JsonObject temp_obj = elem.value().as<JsonObject>();
                
                if(temp_type == curr_type)
                {
                    return temp_obj;
                }
                else if(curr_type.is_base_of(temp_type))
                {
                    return find_base_class<Derived>(temp_obj);
                }
            }
            return JsonObject();
        }

    /////////////////////////////////////////////////////////////////////////////////////

        template<typename T, typename...Base_Classes>
        void visit_type_begin(const type_info<T>& info)
        {
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
            ClassEntry temp;
            temp.mName = get_type_name<declaring_type_t>();
            temp.mSize = sizeof(declaring_type_t);
            mTypeStack.push_back(temp);
            JsonObject object = find_base_class<declaring_type_t>(mJsonOwner);
            if(object.isNull())
            {
                std::cout << "UNHANDLE!!! find_base_class " << std::endl;                
            }
            else
            {
                mJsonObj.set(object);
            }
        }

        template<typename T, typename...Base_Classes>
        void visit_type_end(const type_info<T>& info)
        {

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
            char* charptr = reinterpret_cast<char*>(mObjPtr);
            auto& accessor = reinterpret_cast<declaring_type_t*>( charptr + mPtrOffset)->*info.property_accessor;

            std::string var_name = info.property_item.get_name().to_string();

            read_types(mJsonObj[var_name].as<JsonVariant>() ,value_type,accessor);
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
            char* charptr = reinterpret_cast<char*>(mObjPtr);
            const auto& getter = (reinterpret_cast<declaring_type_t*>(charptr + mPtrOffset)->*info.property_getter)();

            std::string var_name = info.property_item.get_name().to_string();

            decltype(getter) temp;

            read_types(mJsonObj[var_name].as<JsonVariant>(),value_type,temp);

            (reinterpret_cast<declaring_type_t*>(charptr + mPtrOffset)->*info.property_setter)(temp);
        }

        template<typename T>
        void visit_readonly_property(const property_info<T>& info)
        {
            std::cout << "visit_readonly_property: ";
            std::cout << info.property_item.get_name().to_string() << std::endl;

            using declaring_type_t = typename property_info<T>::declaring_type;
            Reflection::type value_type = info.property_item.get_type();
            char* charptr = reinterpret_cast<char*>(mObjPtr);
            auto& accessor = reinterpret_cast<declaring_type_t*>( charptr + mPtrOffset)->*info.property_accessor;

            std::string var_name = info.property_item.get_name().to_string();

            read_types(mJsonObj[var_name].as<JsonVariant>() ,value_type,accessor);
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
            std::cout << "UNHANDLE!!! read_pointer_types: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void read_pointer_types(JsonVariant var, const Reflection::type& value_type,U*& value)
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

        void read_pointer_types(JsonVariant var, const Reflection::type& value_type,char*& value)
        {
            if(value != nullptr)
            {
                delete value;
                value = nullptr;
            }

            std::string temp;

            read_string_types(var, temp);

            value = new char[temp.size()];

            std::copy(temp.begin(),temp.end(),value);
        }

        template<typename U>
        void read_wrapper_types(JsonVariant var, const Reflection::type& value_type,const U& value)
        {
            std::cout << "UNHANDLE!!! read_wrapper: " << typeid(U).name() << std::endl;
        }

        template<typename U>
        void read_wrapper_types(JsonVariant var, const Reflection::type& value_type,std::shared_ptr<U>& value)
        {
            std::cout << "std::shared_ptr: " << typeid(U).name() << std::endl;
            value.reset(new U());
            read_types(var,value_type.get_wrapped_type().get_raw_type(), *value);
        }

        template<typename U>
        void read_sequential_range(JsonArray array, const Reflection::type& value_type,U start, U last)
        {
            size_t index = 0;

            for(auto it = start; it != last; ++it)
            {
                if(index < array.size())
                {
                    read_types(array[index].as<JsonVariant>(),value_type, *it);

                    ++index;
                }
            }
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! read_sequential_container" << std::endl;
        }

        template<typename U, std::size_t SIZE>
        void read_sequential_container(JsonVariant var, const U(*value)[SIZE])
        {
            std::cout << "array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            auto start = std::begin(*value);
            auto last = std::end(*value);
            JsonArray array = var.as<JsonArray>();
            read_sequential_range(array,value_type, start,last);
        }

        template<typename U, std::size_t SIZE>
        void read_sequential_container(JsonVariant var, std::array<U, SIZE>* value)
        {
            std::cout << "std::array:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            read_sequential_range(array,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::vector<U>* value)
        {
            std::cout << "std::vector:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            value->resize(array.size());
            read_sequential_range(array,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::list<U>* value)
        {
            std::cout << "std::list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            value->resize(array.size());
            read_sequential_range(array,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::forward_list<U>* value)
        {
            std::cout << "std::forward_list:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            value->resize(array.size());
            read_sequential_range(array,value_type, value->begin(),value->end());
        }

        template<typename U>
        void read_sequential_container(JsonVariant var, std::deque<U>* value)
        {
            std::cout << "std::deque:" << std::endl;
            Reflection::type value_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            value->resize(array.size());
            read_sequential_range(array,value_type, value->begin(),value->end());
        }

        template<typename U, typename V, typename W>
        void read_associative_keyvalue_range(JsonArray array
                                            , const Reflection::type& key_type
                                            , const Reflection::type& value_type
                                            ,W inserter)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                for(auto elem: array)
                {
                    JsonObject temp_obj = elem.as<JsonObject>();

                    U key_temp;

                    if(key_type.is_arithmetic())
                    {

                    }
                    else if(key_type.is_enumeration())
                    {
                        Reflection::enumeration enum_type = key_type.get_enumeration();
                        Reflection::variant var = enum_type.name_to_value(temp_obj.begin()->key().c_str());
                        key_temp = var.get_value<U>();
                    }
                    else
                    {
                        key_temp = temp_obj.begin()->key().c_str();
                    }
                    V value_temp;

                    read_types(temp_obj.begin()->value().as<JsonVariant>(),value_type,value_temp);

                    *inserter = std::make_pair(key_temp,value_temp);

                    ++inserter;                    
                }
            }
            else
            {
                std::cout << "read_associative_keyvalue_range, UNSUPPORT key type!!!" << std::endl;
            }
        }

        template<typename U, typename V>
        void read_associative_key_range(JsonArray array, const Reflection::type& key_type
                                            ,V inserter)
        {
            if (  key_type.is_arithmetic() || key_type.is_enumeration()
                || key_type == Reflection::type::get<std::string>() )
            {
                for(auto elem: array)
                {
                    U temp;

                    read_basic_types(elem.as<JsonVariant>(),key_type, temp);

                    *inserter = temp;

                    ++inserter;
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
            JsonArray array = var.as<JsonArray>();
            value->clear();
            read_associative_keyvalue_range<U,V>(array,key_type,value_type, std::inserter(*value,value->end()));
        }

        template<typename U,typename V>
        void read_associative_container(JsonVariant var, std::multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();
            JsonArray array = var.to<JsonArray>();
            value->clear();
            read_associative_keyvalue_range<U,V>(array,key_type,value_type,  std::inserter(*value,value->end()));
        }

        template<typename U,typename V>
        void read_associative_container(JsonVariant var, std::unordered_map<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();
            JsonArray array = var.as<JsonArray>();
            value->clear();
            read_associative_keyvalue_range<U,V>(array,key_type,value_type, std::inserter(*value,value->end()));
        }

        template<typename U,typename V>
        void read_associative_container(JsonVariant var, std::unordered_multimap<U,V>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            Reflection::type value_type = Reflection::type::get<V>();
            JsonArray array = var.as<JsonArray>();
            value->clear();
            read_associative_keyvalue_range<U,V>(array,key_type,value_type,  std::inserter(*value,value->end()));
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            value->clear();
            read_associative_key_range<U>(array,key_type, std::inserter(*value,value->end()));
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            read_associative_key_range<U>(array,key_type, std::inserter(*value,value->end()));
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::unordered_set<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            value->clear();
            read_associative_key_range<U>(array,key_type, std::inserter(*value,value->end()));
        }

        template<typename U>
        void read_associative_container(JsonVariant var, std::unordered_multiset<U>* value)
        {
            Reflection::type key_type = Reflection::type::get<U>();
            JsonArray array = var.as<JsonArray>();
            value->clear();
            read_associative_key_range<U>(array,key_type, std::inserter(*value,value->end()));
        }

        template<typename U>
        void read_arithmetic_types(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! read_arithmetic_types" << std::endl;
        }

        void read_arithmetic_types(JsonVariant var, bool& value)
        {
            std::cout << var.as<bool>() << std::endl;
            value = var.as<bool>();
        }

        void read_arithmetic_types(JsonVariant var, char& value)
        {
            std::cout << var.as<char>() << std::endl;
            value = var.as<char>();
        }

        void read_arithmetic_types(JsonVariant var, int8_t& value)
        {
            std::cout << var.as<int8_t>() << std::endl;
            value = var.as<int8_t>();
        }

        void read_arithmetic_types(JsonVariant var, int16_t& value)
        {
            std::cout << var.as<int16_t>() << std::endl;
            value = var.as<int16_t>();
        }

        void read_arithmetic_types(JsonVariant var, int32_t& value)
        {
            std::cout << var.as<int32_t>() << std::endl;
            value = var.as<int32_t>();
        }

        void read_arithmetic_types(JsonVariant var, int64_t& value)
        {
            std::cout << var.as<int64_t>() << std::endl;
            value = var.as<int64_t>();
        }

        void read_arithmetic_types(JsonVariant var, uint8_t& value)
        {
            std::cout << var.as<uint8_t>() << std::endl;
            value = var.as<uint8_t>();
        }

        void read_arithmetic_types(JsonVariant var, uint16_t& value)
        {
            std::cout << var.as<uint16_t>() << std::endl;
            value = var.as<uint16_t>();
        }

        void read_arithmetic_types(JsonVariant var, uint32_t& value)
        {
            std::cout << var.as<uint32_t>() << std::endl;
            value = var.as<uint32_t>();
        }

        void read_arithmetic_types(JsonVariant var, uint64_t& value)
        {
            std::cout << var.as<uint64_t>() << std::endl;
            value = var.as<uint64_t>();
        }

        void read_arithmetic_types(JsonVariant var, float& value)
        {
            std::cout << var.as<float>() << std::endl;
            value = var.as<float>();
        }

        void read_arithmetic_types(JsonVariant var, double& value)
        {
            std::cout << var.as<double>() << std::endl;
            value = var.as<double>();
        }

        template<typename U>
        void read_string_types(JsonVariant var, const U& value)
        {
            std::cout << "UNHANDLE!!! read_string_types" << std::endl;
        }

        void read_string_types(JsonVariant var, std::string& value)
        {
            std::cout << var.as<std::string>() << std::endl;
            value = var.as<std::string>();
        }

        template<typename U>
        void read_basic_types(JsonVariant var, const Reflection::type& value_type, U& value)
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
        EZIReflection(visitor)

        struct ClassEntry
        {
            std::string mName;
            size_t mSize = 0;
        };
         
        std::vector<ClassEntry> mTypeStack;
        JsonObject mJsonOwner;
        JsonObject mJsonObj;

        size_t mPtrOffset = 0;
        void* mObjPtr = nullptr;
    };
}
#endif