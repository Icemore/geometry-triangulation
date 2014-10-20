#include "stdafx.h"

#include "logging.h"

dummy_stream dummy;

std::ostream& log()
{
    #ifdef LOG
    return std::cerr;
    #else
    return dummy;
    #endif
}
