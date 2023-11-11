#pragma once

#include "../SDK/SDK.h"

class CApp
{
public:
	void Start();
	void Shutdown();
};

MAKE_SINGLETON(CApp, App);