#include "AbstractSpiBackend.hpp"

std::string to_string(SpiBackendType type)
{
    switch (type)
    {
    case SpiBackendType::Ftdi:   return "Ftdi";
    case SpiBackendType::Native: return "Native";
    default: break;
    }
    return "unknown";
}