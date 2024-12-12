#include "StringAPI.h"
#include <string.h>

namespace XNet
{
    void strncpy_z(char* dest, const char* src, unsigned int max)
    {
        strncpy(dest, src, max);
        dest[max - 1] = 0;
    }
} // namespace XNet
