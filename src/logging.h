#pragma once

#include <iostream>

struct dummy_stream : std::ostream
{
    template<class T>
    std::ostream& operator<<(T val)
    {
        return *this;
    }
};

extern dummy_stream dummy;
std::ostream& log();
