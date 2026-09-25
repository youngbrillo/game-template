#pragma once
#include <stdint.h>

namespace lib
{
	inline void floatn_copy(const float* from, float* to, size_t size)
	{
		for (int i = 0; i < size; i++)
			to[i] = from[i];
	}

	inline void float2_copy(const float* from, float* to)
	{
		floatn_copy(from, to, 2);
	}

	inline void float3_copy(const float* from, float* to)
	{
		floatn_copy(from, to, 3);
	}

	inline void float4_copy(const float* from, float* to)
	{
		floatn_copy(from, to, 4);
	}
}