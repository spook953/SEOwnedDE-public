#pragma once
#include "cmodel.h"
#include "utlvector.h"
#include "ihandleentity.h"
#include "ispatialpartition.h"

class C_BaseEntity;

class CGameTrace : public CBaseTrace
{
public:
	bool DidHit() const;

	float fractionleftsolid{};
	csurface_t surface{};
	int hitgroup{};
	short physicsbone{};
	C_BaseEntity *m_pEnt{};
	int hitbox{};
};

inline bool CGameTrace::DidHit() const
{
	return fraction < 1.0f || allsolid || startsolid;
}

typedef CGameTrace trace_t;