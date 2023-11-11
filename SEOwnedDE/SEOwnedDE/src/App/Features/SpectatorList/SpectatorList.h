#pragma once

#include "../../../SDK/SDK.h"

class CSpectatorList
{
private:
	struct Spectator_t {
		std::wstring m_wsName = {};
		int m_nMode = 0;
	};

	std::vector<Spectator_t> m_vecSpectators = {};

	bool GetSpectators();

private:
	void Drag();

public:
	void Run();
};

MAKE_SINGLETON_SCOPED(CSpectatorList, SpectatorList, F);