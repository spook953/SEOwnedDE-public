#pragma once

#define MAKE_SINGLETON(type, name) namespace MAKE_SINGLETON_INSTANCES { inline type name = {}; } inline type *name = &MAKE_SINGLETON_INSTANCES::name;
#define MAKE_SINGLETON_SCOPED(type, name, scope) namespace MAKE_SINGLETON_INSTANCES { inline type name = {}; } \
namespace scope { inline type *name = &MAKE_SINGLETON_INSTANCES::name; }