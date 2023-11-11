#include "../../SDK/SDK.h"

#include "../Features/CFG.h"
#include "../Features/Players/Players.h"

MAKE_HOOK(
	CBaseHudChatLine_InsertAndColorizeText,
	Signatures::CBaseHudChatLine_InsertAndColorizeText.Get(),
	void,
	__fastcall,
	void *ecx, void *edx,
	wchar_t *buf, int clientIndex)
{
	if (CFG::Visuals_Chat_Name_Tags && ecx)
	{
		std::wstring str{ buf };
		std::wstring prefix{};

		if (clientIndex == I::EngineClient->GetLocalPlayer())
		{
			prefix = std::format(L"\x8{}[Local] \x3", CFG::Color_Local.toHexStrW());
		}

		else
		{
			if (C_TFPlayer *pl{ I::ClientEntityList->GetClientEntity(clientIndex)->As<C_TFPlayer>() })
			{
				if (pl->IsPlayerOnSteamFriendsList())
				{
					prefix = std::format(L"\x8{}[Friend] \x3", CFG::Color_Friend.toHexStrW());
				}
			}

			players::PlayerInfo pi{};

			if (players::getInfo(clientIndex, pi))
			{
				if (pi.m_ignored)
				{
					prefix = std::format(L"\x8{}[Ignored] \x3", CFG::Color_Friend.toHexStrW());
				}

				if (pi.m_cheater)
				{
					prefix = std::format(L"\x8{}[Cheater] \x3", CFG::Color_Cheater.toHexStrW());
				}

				if (pi.m_retard_legit)
				{
					prefix = std::format(L"\x8{}[Retard Legit] \x3", CFG::Color_RetardLegit.toHexStrW());
				}
			}
		}

		if (!prefix.empty())
		{
			const int name_start{ *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(ecx) + 0x290) };
			const int name_length{ *reinterpret_cast<int *>(reinterpret_cast<uintptr_t>(ecx) + 0x264) };

			str.insert(name_start + name_length, L"\x1");
			str.insert(0, prefix);

			CALL_ORIGINAL(ecx, edx, const_cast<wchar_t *>(str.c_str()), clientIndex);

			return;
		}
	}

	CALL_ORIGINAL(ecx, edx, buf, clientIndex);
}