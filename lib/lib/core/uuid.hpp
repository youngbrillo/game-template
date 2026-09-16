#pragma once

#include <xhash>
#include <string>

namespace lib
{
	class UUID
	{
	public:
		UUID();
		UUID(uint64_t uuid);
		UUID(const UUID&) = default;
		std::string to_str() const;

		uint64_t toUint64() const { return m_UUID; }
		operator uint64_t() const { return m_UUID; }
	private:
		uint64_t m_UUID;
	};

}

namespace std
{
	template<>
	struct hash<lib::UUID>
	{
		std::size_t operator()(const lib::UUID& uuid) const
		{
			return hash<uint64_t>()((uint64_t)uuid);
		}
	};

}