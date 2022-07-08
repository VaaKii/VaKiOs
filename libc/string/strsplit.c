#include <string.h>
#include <stdint.h>

size_t strsplit(char* str, char delim)
{
    size_t n = 0;
    uint32_t i = 0;
    while(str[i])
    {
        if(str[i] == delim)
        {
            str[i] = 0;
            n++;
        }
        i++;
    }
    n++;
    return n;
}
