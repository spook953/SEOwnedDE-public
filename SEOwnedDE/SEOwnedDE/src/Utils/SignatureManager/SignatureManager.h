#pragma once

using DWORD = unsigned long;

#include "../Singleton/Singleton.h"
#include <vector>

class CSignature
{
private:
	DWORD m_dwVal = 0x0;
	const char *m_pszDLLName = {};
	const char *m_pszSignature = {};
	int m_nOffset = 0;
	const char *m_pszName = {};

public:
	CSignature(const char *sDLLName, const char *sSignature, int nOffset, const char *sName);

	void Initialize();

	inline DWORD Get()
	{
		return m_dwVal;
	}
};

#define MAKE_SIGNATURE(name, dll, sig, offset) namespace Signatures { inline CSignature name(dll, sig, offset, #name); }

class CSignatureManager
{
private:
	std::vector<CSignature *> m_vecSignatures = {};

public:
	void InitializeAllSignatures();

	inline void AddSignature(CSignature *pSignature)
	{
		m_vecSignatures.push_back(pSignature);
	}
};

MAKE_SINGLETON_SCOPED(CSignatureManager, SignatureManager, U);