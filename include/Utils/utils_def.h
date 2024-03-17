#pragma once

#ifdef UTILS_EXPORTS
#define UTILS_API __declspec(dllexport)
#else
#define UTILS_API __declspec(dllimport)
#endif // UTILS_EXPORTS

#define _UTILS Utils::

#define _UTILS_BEGIN namespace Utils {
#define _UTILS_END }

#define DECLARE_READONLY_PROPERTY(type, name) __declspec(property(get = Get##name)) type name

#define DECLARE_WRITEONLY_PROPERTY(type, name) __declspec(property(put = Set##name)) type name

#define DECLARE_PROPERTY(type, name) __declspec(property(get = Get##name, put = Set##name)) type name

#define DECLARE_READONLY_PROPERTY_WITH_BODY(type, name, field)                                                                                       \
	inline const type& Get##name() const noexcept { return field; }                                                                                \
	DECLARE_READONLY_PROPERTY(type, name)

#define DECLARE_WRITEONLY_PROPERTY_WITH_BODY(type, name, field)                                                                                      \
	inline void Set##name(const type& val) noexcept { field = val; }                                                                               \
	DECLARE_WRITEONLY_PROPERTY(type, name)

#define DECLARE_PROPERTY_WITH_BODY(type, name, field)                                                                                                \
	inline const type& Get##name() const noexcept { return field; }                                                                                \
	inline void Set##name(const type& val) noexcept { field = val; }                                                                               \
	DECLARE_PROPERTY(type, name)

#define DECLARE_VIRTUAL_READONLY_PROPERTY_WITH_BODY(type, name, field)                                                                               \
	inline virtual const type& Get##name() const noexcept { return field; }                                                                        \
	DECLARE_READONLY_PROPERTY(type, name)

#define DECLARE_VIRTUAL_WRITEONLY_PROPERTY_WITH_BODY(type, name, field)                                                                              \
	inline virtual void Set##name(const type& val) noexcept { field = val; }                                                                       \
	DECLARE_WRITEONLY_PROPERTY(type, name)

#define DECLARE_VIRTUAL_PROPERTY_WITH_BODY(type, name, field)                                                                                        \
	inline virtual const type& Get##name() const noexcept { return field; }                                                                        \
	inline virtual void Set##name(const type& val) noexcept { field = val; }                                                                       \
	DECLARE_PROPERTY(type, name)

#define OVERRIDE_READONLY_PROPERTY(type, name, field)                                                                                                \
	inline const type& Get##name() const noexcept override { return field; }

#define OVERRIDE_PROPERTY(type, name, field)                                                                                                         \
	OVERRIDE_READONLY_PROPERTY(type, name, field)                                                                                                    \
	inline void Set##name(const type& val) noexcept override { field = val; }