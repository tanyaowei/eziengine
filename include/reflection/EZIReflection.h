#ifndef _EZI_REFLECTION_H_
#define _EZI_REFLECTION_H_

#include <rttr/registration>
#include <rttr/type>

namespace EZIEngine
{
    namespace Reflection = rttr;
}

#define EZIReflection(...)                \
RTTR_REGISTRATION_FRIEND                  \
RTTR_ENABLE(...)

#define EZIReflectionRegistration          \
RTTR_REGISTRATION

#endif