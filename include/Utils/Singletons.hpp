#pragma once
#include "utils_def.h"

_UTILS_BEGIN

template <typename _Type>
class Singleton {
private:
	Singleton(const Singleton&)            = default;
	Singleton& operator=(const Singleton&) = default;

protected:
	Singleton() = default;

public:
	virtual ~Singleton() = default;

	static _Type& GetInstance() {
		static _Type instance;
		return instance;
	}
};

_UTILS_END