#include "lib/utils/random_funcs.hpp"
#include <random>

namespace lib
{
	static std::random_device s_randomDevice;
	static std::mt19937_64 s_Engine(s_randomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	uint64_t Random_uint64_t()
	{
		return s_UniformDistribution(s_Engine);
	}
}
