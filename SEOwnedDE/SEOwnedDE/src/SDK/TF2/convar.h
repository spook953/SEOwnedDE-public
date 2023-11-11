#pragma once
#include "iconvar.h"
#include "icvar.h"
#include "utlvector.h"

class ConVar;
class CCommand;
class ConCommand;
class ConCommandBase;
struct characterset_t;

class IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase(ConCommandBase *pVar) = 0;
};

typedef void (*FnCommandCallbackVoid_t)(void);
typedef void (*FnCommandCallback_t)(const CCommand &command);

#define COMMAND_COMPLETION_MAXITEMS 64
#define COMMAND_COMPLETION_ITEM_LENGTH 64

typedef int  (*FnCommandCompletionCallback)(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH]);

class ICommandCallback
{
public:
	virtual void CommandCallback(const CCommand &command) = 0;
};

class ICommandCompletionCallback
{
public:
	virtual int CommandCompletionCallback(const char *pPartial, CUtlVector< /*CUtlString*/ void * > &commands) = 0;
};

class ConCommandBase
{
public:
	virtual ~ConCommandBase(void);
	virtual	bool IsCommand(void) const;
	virtual bool IsFlagSet(int flag) const;
	virtual void AddFlags(int flags);
	virtual const char *GetName(void) const;
	virtual const char *GetHelpText(void) const;
	virtual bool IsRegistered(void) const;
	virtual void *GetDLLIdentifier() const;
	virtual void CreateBase(const char *pName, const char *pHelpString = 0, int flags = 0);
	virtual void Init();

public:
	ConCommandBase *m_pNext;
	bool m_bRegistered;
	const char *m_pszName;
	const char *m_pszHelpString;
	int m_nFlags;

protected:
	static ConCommandBase *s_pConCommandBases;
	static IConCommandBaseAccessor *s_pAccessor;
};

enum
{
	COMMAND_MAX_ARGC = 64,
	COMMAND_MAX_LENGTH = 512
};

class CCommand
{
public:
	int m_nArgc;
	int m_nArgv0Size;
	char m_pArgSBuffer[COMMAND_MAX_LENGTH];
	char m_pArgvBuffer[COMMAND_MAX_LENGTH];
	const char *m_ppArgv[COMMAND_MAX_ARGC];
};

class ConCommand : public ConCommandBase
{
public:
	virtual ~ConCommand(void);
	virtual	bool IsCommand(void) const;
	virtual int AutoCompleteSuggest(const char *partial, CUtlVector< /*CUtlString*/ void * > &commands);
	virtual bool CanAutoComplete(void);
	virtual void Dispatch(const CCommand &command);

private:
	union
	{
		FnCommandCallbackVoid_t m_fnCommandCallbackV1;
		FnCommandCallback_t m_fnCommandCallback;
		ICommandCallback *m_pCommandCallback;
	};

	union
	{
		FnCommandCompletionCallback	m_fnCompletionCallback;
		ICommandCompletionCallback *m_pCommandCompletionCallback;
	};

	bool m_bHasCompletionCallback : 1;
	bool m_bUsingNewCommandCallback : 1;
	bool m_bUsingCommandCallbackInterface : 1;
};

class ConVar : public ConCommandBase, public IConVar
{
public:
	virtual ~ConVar(void);
	virtual bool IsFlagSet(int flag) const;
	virtual const char *GetHelpText(void) const;
	virtual bool IsRegistered(void) const;
	virtual const char *GetName(void) const;
	virtual void AddFlags(int flags);
	virtual	bool IsCommand(void) const;

	inline float GetFloat(void) const;
	inline int GetInt(void) const;
	inline bool GetBool() const { return !!GetInt(); }
	inline char const *GetString(void) const;

	virtual void SetValue(const char *value);
	virtual void SetValue(float value);
	virtual void SetValue(int value);

private:
	virtual void InternalSetValue(const char *value);
	virtual void InternalSetFloatValue(float fNewValue, bool bForce = false);
	virtual void InternalSetIntValue(int nValue);
	virtual bool ClampValue(float &value);
	virtual void ChangeStringValue(const char *tempVal, float flOldValue);
	virtual void Init();

	int GetFlags() { return m_pParent->m_nFlags; }

private:
	ConVar *m_pParent;
	const char *m_pszDefaultValue;
	char *m_pszString;
	int m_StringLength;
	float m_fValue;
	int m_nValue;
	bool m_bHasMin;
	float m_fMinVal;
	bool m_bHasMax;
	float m_fMaxVal;
	bool m_bHasCompMin;
	float m_fCompMinVal;
	bool m_bHasCompMax;
	float m_fCompMaxVal;
	bool m_bCompetitiveRestrictions;
	FnChangeCallback_t m_fnChangeCallback;
};

inline float ConVar::GetFloat(void) const
{
	return m_pParent->m_fValue;
}

inline int ConVar::GetInt(void) const
{
	return m_pParent->m_nValue;
}

inline const char *ConVar::GetString(void) const
{
	if (m_nFlags & FCVAR_NEVER_AS_STRING)
		return "FCVAR_NEVER_AS_STRING";

	return (m_pParent->m_pszString) ? m_pParent->m_pszString : "";
}