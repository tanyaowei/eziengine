#ifndef _EZI_REFLECTION_H_
#define _EZI_REFLECTION_H_

#include <rttr/registration>

namespace EZIEngine
{
    namespace Reflection = rttr;
}

#define EZIReflectionFriend                \
RTTR_REGISTRATION_FRIEND

#define EZIReflectionRegistration          \
RTTR_REGISTRATION

#endif