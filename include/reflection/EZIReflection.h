#ifndef _EZI_REFLECTION_H_
#define _EZI_REFLECTION_H_

#include <rttr/registration>
#include <rttr/registration_friend>
#include <rttr/type>

namespace EZIEngine
{
    namespace Reflection = rttr;
}

#define EZIReflection(...)                \
RTTR_REGISTRATION_FRIEND                  \
RTTR_ENABLE(__VA_ARGS__)

#define EZIReflectionRegistration          \
RTTR_REGISTRATION

#endif