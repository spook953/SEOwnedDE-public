#include "../../SDK/SDK.h"

#include "../Features/CFG.h"

#include "../Features/AimUtils/AimUtils.h"

MAKE_HOOK(
	CTFPlayer_FireBullet, Signatures::CTFPlayer_FireBullet.Get(),
	void, __fastcall, void *ecx, void *edx, C_TFWeaponBase *pWpn, FireBulletsInfo_t &info, bool bDoEffects, int nDamageType, int nCustomDamageType)
{
	if (auto pLocal = H::Entities->GetLocal())
	{
		if (ecx == pLocal)
		{
			if (CFG::Visuals_Tracer_Type)
			{
				if (nDamageType & DMG_CRITICAL)
					nDamageType &= ~DMG_CRITICAL;

				info.m_iTracerFreq = 1;
			}

			if (CFG::Visuals_Beams_Active)
			{
				info.m_iTracerFreq = 0;

				Vec3 vStart = {};
				Vec3 vEnd = {};

				if (auto pWeapon = pLocal->m_hActiveWeapon().Get())
				{
					Vec3 angles = {};

					if (auto pAttachEnt = pWpn->GetAppropriateWorldOrViewModel())
						pAttachEnt->GetAttachment(pWeapon->LookupAttachment("muzzle"), vStart, angles);
				}

				trace_t trace = {};
				CTraceFilterHitscan filter = {};
				F::AimUtils->Trace(info.m_vecSrc, info.m_vecSrc + (info.m_vecDirShooting * info.m_flDistance), (MASK_SHOT | CONTENTS_GRATE), &filter, &trace);

				vEnd = trace.endpos;

				BeamInfo_t beam = {};
				beam.m_nType = 0;
				beam.m_pszModelName = "sprites/physbeam.vmt";
				beam.m_nModelIndex = -1;
				beam.m_flHaloScale = 0.0f;
				beam.m_flLife = CFG::Visuals_Beams_LifeTime;
				beam.m_flWidth = CFG::Visuals_Beams_Width;
				beam.m_flEndWidth = CFG::Visuals_Beams_EndWidth;
				beam.m_flFadeLength = CFG::Visuals_Beams_FadeLength;
				beam.m_flAmplitude = CFG::Visuals_Beams_Amplitude;
				beam.m_flBrightness = 255.0f;
				beam.m_flSpeed = CFG::Visuals_Beams_Speed;
				beam.m_nStartFrame = 0;
				beam.m_flFrameRate = 0;
				beam.m_flRed = static_cast<float>(CFG::Color_Beams.r);
				beam.m_flGreen = static_cast<float>(CFG::Color_Beams.g);
				beam.m_flBlue = static_cast<float>(CFG::Color_Beams.b);
				beam.m_nSegments = 2;
				beam.m_bRenderable = true;
				beam.m_nFlags = 0;
				beam.m_vecStart = vStart;
				beam.m_vecEnd = vEnd;

				if (CFG::Visuals_Beams_Flag_FBEAM_FADEIN)
					beam.m_nFlags |= FBEAM_FADEIN;

				if (CFG::Visuals_Beams_Flag_FBEAM_FADEOUT)
					beam.m_nFlags |= FBEAM_FADEOUT;

				if (CFG::Visuals_Beams_Flag_FBEAM_SINENOISE)
					beam.m_nFlags |= FBEAM_SINENOISE;

				if (CFG::Visuals_Beams_Flag_FBEAM_SOLID)
					beam.m_nFlags |= FBEAM_SOLID;

				if (CFG::Visuals_Beams_Flag_FBEAM_SHADEIN)
					beam.m_nFlags |= FBEAM_SHADEIN;

				if (CFG::Visuals_Beams_Flag_FBEAM_SHADEOUT)
					beam.m_nFlags |= FBEAM_SHADEOUT;

				if (auto pBeam = I::ViewRenderBeams->CreateBeamPoints(beam))
					I::ViewRenderBeams->DrawBeam(pBeam);
			}
		}
	}

	CALL_ORIGINAL(ecx, edx, pWpn, info, bDoEffects, nDamageType, nCustomDamageType);
}