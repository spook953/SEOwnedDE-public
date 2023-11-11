#include "../../SDK/SDK.h"

#include "../Features/CFG.h"
#include "../Features/NetworkFix/NetworkFix.h"

MAKE_HOOK(
	CL_Move, Signatures::CL_Move.Get(),
	void, __cdecl, float accumulated_extra_samples, bool bFinalTick)
{
	auto CallOriginal = [&](bool bFinal)
	{
		if (CFG::Misc_Ping_Reducer)
		{
			F::NetworkFix->FixInputDelay(bFinal);
		}

		CALL_ORIGINAL(accumulated_extra_samples, bFinal);
	};

	if (Shifting::bRapidFireWantShift)
	{
		Shifting::bRapidFireWantShift = false;
		Shifting::bShifting = true;

		int nTicks = CFG::Exploits_RapidFire_Ticks;

		for (int n = 0; n < nTicks; n++)
		{
			CallOriginal(n == nTicks - 1);

			Shifting::nAvailableTicks--;
		}

		Shifting::bShifting = false;

		return;
	}

	if (auto pLocal = H::Entities->GetLocal())
	{
		if (!pLocal->deadflag() && !Shifting::bRecharging && !Shifting::bShifting && !Shifting::bShiftingWarp && !Shifting::bRapidFireWantShift)
		{
			if (!I::MatSystemSurface->IsCursorVisible() && !I::EngineVGui->IsGameUIVisible() && (H::Input->IsDown(CFG::Exploits_Warp_Key)))
			{
				if (Shifting::nAvailableTicks)
				{
					Shifting::bShifting = true;
					Shifting::bShiftingWarp = true;

					if (CFG::Exploits_Warp_Mode == 0)
					{
						for (int n = 0; n < 2; n++)
						{
							CallOriginal(n == 1);
						}

						Shifting::nAvailableTicks--;
					}

					if (CFG::Exploits_Warp_Mode == 1)
					{
						int nTicks = Shifting::nAvailableTicks;

						for (int n = 0; n < nTicks; n++)
						{
							CallOriginal(n == nTicks - 1);

							Shifting::nAvailableTicks--;
						}
					}

					Shifting::bShifting = false;
					Shifting::bShiftingWarp = false;

					return;
				}
			}
		}
	}

	CallOriginal(bFinalTick);
}