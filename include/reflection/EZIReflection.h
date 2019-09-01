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
#include <forward_list>

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
        Serializer(const void* object)
        :mObject(object){}
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

            using declaring_type_t = typename property_info<T>::declaring_type;
            Reflection::type value_type = info.property_item.get_type();
            const auto& accessor = reinterpret_cast<const declaring_type_t*>(mObject)->*info.property_accessor;

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
            const auto& getter = (reinterpret_cast<const declaring_type_t*>(mObject)->*info.property_getter)();

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
        void write_sequential_range(U start, U last)
        {
            for(auto it = start; it != last; ++it)
            {
                Reflection::type value_type = Reflection::instance(*it).get_type();
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
            auto start = std::begin(*value);
            auto last = std::end(*value);
            write_sequential_range(start,last);
        }

        template<typename U, std::size_t SIZE>
        void write_sequential_container(const std::array<U, SIZE>* value)
        {
            std::cout << "std::array:" << std::endl;
            write_sequential_range(value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::vector<U>* value)
        {
            std::cout << "std::vector:" << std::endl;
            write_sequential_range(value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::list<U>* value)
        {
            std::cout << "std::list:" << std::endl;
            write_sequential_range(value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::forward_list<U>* value)
        {
            std::cout << "std::forward_list:" << std::endl;
            write_sequential_range(value->cbegin(),value->cend());
        }

        template<typename U>
        void write_sequential_container(const std::deque<U>* value)
        {
            std::cout << "std::deque:" << std::endl;
            print_iterator_range(value->cbegin(),value->cend());
        }

        template<typename U>
        void write_arithmetic_types(const U& value)
        {
            std::cout << "UNHANDLE!!! write_arithmetic_types" << std::endl;
        }

        void write_arithmetic_types(const int& value)
        {
            std::cout << value << std::endl;
        }

        void write_arithmetic_types(const double& value)
        {
            std::cout << value << std::endl;
        }

        template<typename U>
        void write_basic_types(const Reflection::type& value_type,U& value)
        {
            if (value_type.is_arithmetic())
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
                    write_arithmetic_types(value);
                    #endif
            }
            else if (value_type.is_enumeration())
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
                std::cout << typeid(value).name() << std::endl;
                std::cout << value_type.get_enumeration().value_to_name(value).to_string() << std::endl;
                #endif
            }
            else if (value_type == Reflection::type::get<std::string>())
            {
                //data.setValue(var.to_string());
                std::cout << "is_string" << std::endl;
                //std::cout << value << std::endl;
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
                || value_type == Reflection::type::get<std::string>() )
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

        const void* mObject;
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