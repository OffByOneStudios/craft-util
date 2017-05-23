#pragma once
#include "common.h"

namespace craft {
	template<typename TId = unsigned long long>
	struct Ider
	{
	protected:
		inline Ider(TId id) : id(id) { }
	public:
		const TId id;

		inline operator TId() { return id; }
	};
}
