#include <reflection/EZIReflection.h>
#include <iostream>

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

int conv_func(int* value, bool& ok)
{
    std::cout << "conv_func: " << *value << std::endl;
    ok = true;
    return *value;
}

EZIReflectionRegistration
{
    EZIEngine::Reflection::type::register_converter_func(conv_func);
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
}