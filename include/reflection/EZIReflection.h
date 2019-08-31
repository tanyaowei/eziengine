#ifndef _EZI_REFLECTION_H_
#define _EZI_REFLECTION_H_

#include <rttr/visitor.h>
#include <iterator>

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

#include <iostream>
#include <typeinfo>

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
        Serializer(void* obj)
        :mObj(obj){}
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
            std::cout << info.property_item.get_name().to_string() << " - ";
            std::cout << info.property_item.get_type().get_name().to_string() << std::endl;

            //std::cout << typeid(declaring_type_t).name() << std::endl;
            //write_types(info.property_item.get_type());

            using declaring_type_t = typename property_info<T>::declaring_type;
            Reflection::type value_type = info.property_item.get_type();
            const auto& accessor = reinterpret_cast<declaring_type_t*>(mObj)->*info.property_accessor;

            if(value_type.is_pointer())
            {
                std::cout << "is_pointer" << std::endl;
                //write_types(value_type.get_raw_type());
            }
            else if(value_type.is_wrapper())
            {      
              std::cout << "is_wrapper" << std::endl;
                //write_types(value_type.get_wrapped_type());
            }
            else if (value_type.is_arithmetic() || value_type.is_enumeration()
                || value_type == Reflection::type::get<std::string>() )
            {
                write_basic_types(value_type); 
            }
            else if(value_type.is_sequential_container())
            {
                printArr(&accessor);
            }
            else if(value_type.is_associative_container())
            {
                std::cout << "is_associative_container" << std::endl;
            }
            else // object
            {
                std::cout << "is_object" << std::endl;
            }

        }

        template<typename T>
        void visit_getter_setter_property(const property_getter_setter_info<T>& info)
        {
            std::cout << "visit_getter_setter_property: ";
            std::cout << std::string("get_") + info.property_item.get_name().to_string() << " - ";
            std::cout << std::string("set_") + info.property_item.get_name().to_string() << " - ";
            std::cout << info.property_item.get_type().get_name().to_string() << std::endl;
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
        void printIter(const U start, const U last)
        {
            for(auto it = start; it != last; ++it)
            {
                std::cout << *it << std::endl;
            }
        }

        template<typename U>
        void printArr(const U& val)
        {
            std::cout << "UNHANDLE!!!" << std::endl;
        }

        template<typename U, std::size_t SIZE>
        void printArr(const U(*val)[SIZE])
        {
            std::cout << "array:" << std::endl;
            const auto start = std::begin(*val);
            const auto last = std::end(*val);
            printIter(start,last);
        }

        template<typename U, std::size_t SIZE>
        void printArr(const std::array<U, SIZE>* val)
        {
            std::cout << "std::array:" << std::endl;
            printIter(val->cbegin(),val->cend());
        }

        template<typename U>
        void printArr(const std::vector<U>* val)
        {
            std::cout << "std::vector:" << std::endl;
            printIter(val->cbegin(),val->cend());
        }

        template<typename U>
        void printArr(const std::list<U>* val)
        {
            std::cout << "std::list:" << std::endl;
            printIter(val->cbegin(),val->cend());
        }

        void write_basic_types(const Reflection::type& t)
        {
            if (t.is_arithmetic())
            {
                #if 0
                if (t == Reflection::type::get<bool>())
                    data.setValue(var.to_bool());
                else if (t == Reflection::type::get<char>())
                    data.setValue(var.to_int8());
                else if (t == Reflection::type::get<int8_t>())
                    data.setValue(var.to_int8());
                else if (t == Reflection::type::get<int16_t>())
                    data.setValue(var.to_int16());
                else if (t == Reflection::type::get<int32_t>())
                    data.setValue(var.to_int32());
                else if (t == Reflection::type::get<int64_t>())
                    data.setValue(var.to_int64());
                else if (t == Reflection::type::get<uint8_t>())
                    data.setValue(var.to_uint8());
                else if (t == Reflection::type::get<uint16_t>())
                    data.setValue(var.to_uint16());
                else if (t == Reflection::type::get<uint32_t>())
                    data.setValue(var.to_uint32());
                else if (t == Reflection::type::get<uint64_t>())
                    data.setValue(var.to_uint64());
                else if (t == Reflection::type::get<float>())
                    data.setValue(var.to_float());
                else if (t == Reflection::type::get<double>())
                    data.setValue(var.to_double());
                    #else
                    std::cout << "is_arithmetic" << std::endl;
                    #endif
            }
            else if (t.is_enumeration())
            {
                #if 0
                bool ok = false;
                auto enumstr = var.to_string(&ok);
                if (ok)
                {
                data.setValue(enumstr);
                }
                else
                {
                data.setValue(std::string());
                }
                #else
                std::cout << "is_enumeration" << std::endl;
                #endif
            }
            else if (t == Reflection::type::get<std::string>())
            {
                //data.setValue(var.to_string());
                std::cout << "is_string" << std::endl;
            }
        }

        void write_types(const Reflection::type&value_type)
        {
            if(value_type.is_pointer())
            {
                std::cout << "is_pointer" << std::endl;
                //write_types(value_type.get_raw_type());
            }
            else if(value_type.is_wrapper())
            {      
              std::cout << "is_wrapper" << std::endl;
                //write_types(value_type.get_wrapped_type());
            }
            else if (value_type.is_arithmetic() || value_type.is_enumeration()
                || value_type == Reflection::type::get<std::string>() )
            {
                write_basic_types(value_type); 
            }
            else if(value_type.is_sequential_container())
            {
                std::cout << "is_sequential_container" << std::endl;
            }
            else if(value_type.is_associative_container())
            {
                std::cout << "is_associative_container" << std::endl;
            }
            else // object
            {
                std::cout << "is_object" << std::endl;
            }
        }

private:
        EZIReflection(visitor)

        void* mObj;
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