#include "SignatureManager.h"
#include "../Memory/Memory.h"
#include "../Assert/Assert.h"
#include <string>

CSignature::CSignature(const char *sDLLName, const char *sSignature, int nOffset, const char *sName)
{
	m_pszDLLName = sDLLName;
	m_pszSignature = sSignature;
	m_nOffset = nOffset;
	m_pszName = sName;

	U::SignatureManager->AddSignature(this);
}

void CSignature::Initialize()
{
	m_dwVal = Memory::FindSignature(m_pszDLLName, m_pszSignature);

	AssertCustom(!m_dwVal, std::string("CSignature::Initialize() Failed to initialize (" + std::string(m_pszName) + ")").c_str());

	m_dwVal += m_nOffset;
}

void CSignatureManager::InitializeAllSignatures()
{
	for (auto Signature : m_vecSignatures)
	{
		if (!Signature)
			continue;

		Signature->Initialize();
	}
}