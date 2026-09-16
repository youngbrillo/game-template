#include "lib/core/uuid.hpp"
#include "lib/utils/random_funcs.hpp"
#include <format>

lib::UUID::UUID()
    : m_UUID(Random_uint64_t())
{
}

lib::UUID::UUID(uint64_t uuid)
    : m_UUID(uuid)
{
}

std::string lib::UUID::to_str() const
{
    return std::format("{:#018X}", m_UUID);
}
