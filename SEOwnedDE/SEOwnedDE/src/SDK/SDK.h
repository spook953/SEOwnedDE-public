#pragma once

#include "TF2/IMatSystemSurface.h"
#include "TF2/cdll_int.h"
#include "TF2/icliententitylist.h"
#include "TF2/ivmodelinfo.h"
#include "TF2/ivdebugoverlay.h"
#include "TF2/clientmode_shared.h"
#include "TF2/imaterialsystem.h"
#include "TF2/ivmodelrender.h"
#include "TF2/ienginevgui.h"
#include "TF2/ivrenderview.h"
#include "TF2/globalvars_base.h"
#include "TF2/IEngineTrace.h"
#include "TF2/icvar.h"
#include "TF2/prediction.h"
#include "TF2/igamemovement.h"
#include "TF2/netmessages.h"
#include "TF2/iinputsystem.h"
#include "TF2/iinput.h"
#include "TF2/istudiorender.h"
#include "TF2/igameevents.h"
#include "TF2/c_tf_playerresource.h"
#include "TF2/itoolentity.h"
#include "TF2/random.h"
#include "TF2/client.h"
#include "TF2/iviewrender_beams.h"
#include "TF2/vphysics.h"

#include "TF2/keyvalues.h"
#include "TF2/c_baseobject.h"
#include "TF2/imaterialvar.h"
#include "TF2/itexture.h"
#include "TF2/MD5.h"
#include "TF2/c_tf_player.h"

#include "Helpers/Draw/Draw.h"
#include "Helpers/Entities/Entities.h"
#include "Helpers/Fonts/Fonts.h"
#include "Helpers/Input/Input.h"

#define PRINT(...) I::CVar->ConsoleColorPrintf({ 20, 220, 55, 255 }, __VA_ARGS__)

MAKE_INTERFACE_SIGNATURE(int, RandomSeed, "client.dll", "C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34", 2, 1);
MAKE_INTERFACE_NULL(void, TFGameMovement);

class IViewRender
{
public:
	void RenderView(const CViewSetup &view, int nClearFlags, int whatToDraw) {
		reinterpret_cast<void(__thiscall *)(void *, const CViewSetup &, int, int)>(Signatures::CViewRender_RenderView.Get())(this, view, nClearFlags, whatToDraw);
	}
};

MAKE_INTERFACE_NULL(IViewRender, ViewRender);

namespace SDKUtils
{
	inline void GetProjectileFireSetupRebuilt(C_TFPlayer *player, Vec3 offset, const Vec3 &ang_in, Vec3 &pos_out, Vec3 &ang_out, bool pipes)
	{
		static auto cl_flipviewmodels{ I::CVar->FindVar("cl_flipviewmodels") };

		if (!cl_flipviewmodels)
		{
			return;
		}

		if (cl_flipviewmodels->GetBool())
		{
			offset.y *= -1.0f;
		}

		Vec3 forward{}, right{}, up{};
		Math::AngleVectors(ang_in, &forward, &right, &up);

		auto shoot_pos{ player->GetShootPos() };

		pos_out = shoot_pos + (forward * offset.x) + (right * offset.y) + (up * offset.z);

		if (pipes)
		{
			ang_out = ang_in;
		}

		else
		{
			auto end_pos{ shoot_pos + (forward * 2000.0f) };

			Math::VectorAngles(end_pos - pos_out, ang_out);
		}
	}

	static float GetLerp()
	{
		static ConVar *cl_interp = I::CVar->FindVar("cl_interp");
		static ConVar *cl_interp_ratio = I::CVar->FindVar("cl_interp_ratio");
		static ConVar *cl_updaterate = I::CVar->FindVar("cl_updaterate");

		return std::max(cl_interp->GetFloat(), cl_interp_ratio->GetFloat() / cl_updaterate->GetFloat());
	}

	static Vec3 GetHitboxPosFromMatrix(C_BaseAnimating *pAnimating, int nHitbox, matrix3x4_t *pMatrix)
	{
		auto pModel = pAnimating->GetModel();

		if (!pModel)
			return {};
		
		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);

		if (!pHDR)
			return {};

		auto pSet = pHDR->pHitboxSet(pAnimating->m_nHitboxSet());

		if (!pSet)
			return {};

		auto pBox = pSet->pHitbox(nHitbox);

		if (!pBox)
			return {};

		Vec3 vOut = {};
		Math::VectorTransform((pBox->bbmin + pBox->bbmax) * 0.5f, pMatrix[pBox->bone], vOut);
		return vOut;
	}

	static void GetHitboxInfoFromMatrix(C_BaseAnimating *pAnimating, int nHitbox, matrix3x4_t *pMatrix, Vec3 *pCenter, Vec3 *pMins, Vec3 *pMaxs)
	{
		auto pModel = pAnimating->GetModel();

		if (!pModel)
			return;

		auto pHDR = I::ModelInfoClient->GetStudiomodel(pModel);

		if (!pHDR)
			return;

		auto pSet = pHDR->pHitboxSet(pAnimating->m_nHitboxSet());

		if (!pSet)
			return;

		auto pBox = pSet->pHitbox(nHitbox);

		if (!pBox)
			return;

		if (pMins)
			*pMins = pBox->bbmin;

		if (pMaxs)
			*pMaxs = pBox->bbmax;

		if (pCenter)
			Math::VectorTransform((pBox->bbmin + pBox->bbmax) * 0.5f, pMatrix[pBox->bone], *pCenter);
	}

	inline float GetLatency()
	{
		if (auto pNet = I::EngineClient->GetNetChannelInfo())
			return pNet->GetLatency(FLOW_INCOMING) + pNet->GetLatency(FLOW_OUTGOING);

		return 0.0f;
	}

	inline float GetGravity()
	{
		static ConVar *sv_gravity = I::CVar->FindVar("sv_gravity");
		return sv_gravity ? sv_gravity->GetFloat() : 0.0f;
	}

	static float RandomFloat(float min_val, float max_val)
	{
		static auto fn = reinterpret_cast<float(__cdecl *)(float, float)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomFloat"));
		return fn(min_val, max_val);
	}

	static int RandomInt(int min_val, int max_val)
	{
		static auto fn = reinterpret_cast<int(__cdecl *)(int, int)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomInt"));
		return fn(min_val, max_val);
	}

	static void RandomSeed(unsigned int seed)
	{
		static auto fn = reinterpret_cast<void(__cdecl *)(unsigned int)>(GetProcAddress(GetModuleHandleA("vstdlib.dll"), "RandomSeed"));
		fn(seed);
	}

	static int FindCmdNumWithSeed(int nCommandNumber, int nDesiredSeed)
	{
		int nCmdNum = nCommandNumber, nIter = 0;

		while (nIter++ < 1024)
		{
			int nSeed = MD5_PseudoRandom(++nCmdNum) & std::numeric_limits<int>::max();

			if ((nSeed & 255) == nDesiredSeed)
				return nCmdNum;
		}

		return nCommandNumber;
	}

	static int SharedRandomInt(const char *sharedname, int iMinVal, int iMaxVal, int additionalSeed)
	{
		using fn = int(__cdecl *)(const char *, int, int, int);
		return reinterpret_cast<fn>(Signatures::SharedRandomInt.Get())(sharedname, iMinVal, iMaxVal, additionalSeed);
	}

	static bool BInEndOfMatch() {
		return reinterpret_cast<bool(__cdecl *)()>(Memory::RelToAbs(Signatures::BInEndOfMatch.Get()))();
	}

	static int LookupSequence(CStudioHdr *pstudiohdr, const char *label) {
		return reinterpret_cast<int(__cdecl *)(CStudioHdr *, const char *)>(Signatures::LookupSequence.Get())(pstudiohdr, label);
	};

	static int CreateTextureFromArray(const unsigned char *rgba, int w, int h)
	{
		int nTextureIdOut = I::MatSystemSurface->CreateNewTextureID(true);
		I::MatSystemSurface->DrawSetTextureRGBAEx(nTextureIdOut, rgba, w, h, IMAGE_FORMAT_BGRA8888);
		return nTextureIdOut;
	}
}

namespace G
{
	inline bool bSilentAngles = false;
	inline bool bPSilentAngles = false;
	inline int nTargetIndex = -1;
	inline float flAimbotFOV = 0.0f;
	inline bool bCanPrimaryAttack = false;
	inline bool bCanSecondaryAttack = false;
	inline bool bCanHeadshot = false;
	inline int nOldButtons = 0;
	inline Vec3 vUserCmdAngles = {};
	inline bool CHudTFCrosshair_ShouldDraw_Result = false;

	struct VelFixRecord_t
	{
		Vec3 m_vecOrigin = {};
		int m_fFlags = 0;
		float m_flSimulationTime = 0.0f;
	};

	inline std::map<C_BasePlayer *, VelFixRecord_t> mapVelFixRecords = {};

	inline bool bFiring = false;
	inline int nTicksTargetSame = 0;
	inline int nTargetIndexEarly = 0;
	inline int nTicksSinceCanFire = 0;

	inline bool bUpdatingAnims = false;

	inline bool bStartedFakeTaunt = false;
	inline float flFakeTauntStartYaw = 0.0f;

	inline bool bRenderingSpyCamera = false;

	inline CViewSetup View = {};

	inline auto in_CViewRender_DrawViewModels{ false };
}

namespace Shifting
{
	inline int nAvailableTicks = 0;
	inline bool bRecharging = false;
	inline bool bShifting = false;
	inline bool bRapidFireWantShift = false;
	inline bool bShiftingWarp = false;

	inline void Reset()
	{
		nAvailableTicks = 0;
		bRecharging = false;
		bShifting = false;
		bRapidFireWantShift = false;
		bShiftingWarp = false;
	}
}

#define GET_ENT_FROM_USER_ID(userid) I::ClientEntityList->GetClientEntity(I::EngineClient->GetPlayerForUserID(userid))

struct ShaderStencilState_t
{
	bool m_bEnable = false;
	StencilOperation_t m_FailOp = {};
	StencilOperation_t m_ZFailOp = {};
	StencilOperation_t m_PassOp = {};
	StencilComparisonFunction_t m_CompareFunc = {};
	int m_nReferenceValue = 0;
	uint32 m_nTestMask = 0;
	uint32 m_nWriteMask = 0;

	ShaderStencilState_t()
	{
		m_bEnable = false;
		m_PassOp = m_FailOp = m_ZFailOp = STENCILOPERATION_KEEP;
		m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
		m_nReferenceValue = 0;
		m_nTestMask = m_nWriteMask = 0xFFFFFFFF;
	}

	void SetStencilState(IMatRenderContext *pRenderContext)
	{
		pRenderContext->SetStencilEnable(m_bEnable);
		pRenderContext->SetStencilFailOperation(m_FailOp);
		pRenderContext->SetStencilZFailOperation(m_ZFailOp);
		pRenderContext->SetStencilPassOperation(m_PassOp);
		pRenderContext->SetStencilCompareFunction(m_CompareFunc);
		pRenderContext->SetStencilReferenceValue(m_nReferenceValue);
		pRenderContext->SetStencilTestMask(m_nTestMask);
		pRenderContext->SetStencilWriteMask(m_nWriteMask);
	}
};

MAKE_SIGNATURE(GetClientInterpAmount, "client.dll", "55 8B EC A1 ? ? ? ? 83 EC 08 56 A8 01", 0);

inline float GetClientInterpAmount()
{
	return reinterpret_cast<float(__cdecl *)()>(Signatures::GetClientInterpAmount.Get())();
}

inline double Plat_FloatTime()
{
	static auto fn{ reinterpret_cast<double(__cdecl *)()>(GetProcAddress(GetModuleHandleA("tier0.dll"), "Plat_FloatTime")) };

	return fn();
}