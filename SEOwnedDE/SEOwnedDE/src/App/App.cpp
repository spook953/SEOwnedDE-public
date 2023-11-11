#include "App.h"

#include "Hooks/WINAPI_WndProc.h"

#include "Features/Materials/Materials.h"
#include "Features/Outlines/Outlines.h"
#include "Features/WorldModulation/WorldModulation.h"
#include "Features/Paint/Paint.h"
#include "Features/Menu/Menu.h"
#include "Features/Players/Players.h"

#include "Features/CFG.h"

void CApp::Start()
{
	U::SignatureManager->InitializeAllSignatures();
	U::InterfaceManager->InitializeAllInterfaces();

	H::Draw->UpdateScreenSize();
	
	H::Fonts->Reload();

	if (I::EngineClient->IsInGame() && I::EngineClient->IsConnected())
	{
		H::Entities->UpdateModelIndexes();
	}
	
	U::HookManager->InitializeAllHooks();

	Hooks::WINAPI_WndProc::Init();

	players::parse();

	Config::Load((std::filesystem::current_path().string() + "\\SEOwnedDE\\configs\\default.json").c_str());

	auto month = []
	{
		std::time_t t = std::time(0);
		tm Time = {};
		localtime_s(&Time, &t);

		return Time.tm_mon + 1;
	}();

	if (month == 10)
	{
		I::MatSystemSurface->PlaySound("vo\\halloween_boss\\knight_alert.mp3");

		I::CVar->ConsoleColorPrintf({ 247, 136, 18, 255 }, "SEOwnedDE Loaded!\n");
	}

	if (month == 12 || month == 1 || month == 2)
	{
		if (month == 12)
		{
			I::MatSystemSurface->PlaySound("misc\\jingle_bells\\jingle_bells_nm_04.wav");
		}

		I::CVar->ConsoleColorPrintf({ 28, 179, 210, 255 }, "SEOwnedDE Loaded!\n");
	}

	else
	{
		I::CVar->ConsoleColorPrintf({ 197, 108, 240, 255 }, "SEOwnedDE Loaded!\n");
	}
}

void CApp::Shutdown()
{
	U::HookManager->FreeAllHooks();

	SetWindowLongPtr(Hooks::WINAPI_WndProc::hwWindow, GWL_WNDPROC, (LONG_PTR)Hooks::WINAPI_WndProc::Original);
	
	Sleep(250);

	F::Materials->CleanUp();
	F::Outlines->CleanUp();
	F::Paint->CleanUp();

	F::WorldModulation->RestoreWorldModulation();

	if (auto cl_wpn_sway_interp{ I::CVar->FindVar("cl_wpn_sway_interp") })
	{
		cl_wpn_sway_interp->SetValue(0.0f);
	}

	if (F::Menu->IsOpen())
	{
		I::MatSystemSurface->SetCursorAlwaysVisible(false);
	}
	
	I::CVar->ConsoleColorPrintf({ 255, 70, 70, 255 }, "SEOwnedDE Unloaded!\n");
}

MAKE_HOOK(
	IMaterialSystem_FindTexture,
	Memory::GetVFunc(I::MaterialSystem, 81),
	ITexture *,
	__fastcall,
	void *ecx, void *edx, char const *pTextureName, const char *pTextureGroupName, bool complain, int nAdditionalCreationFlags)
{
	ITexture *const result{ CALL_ORIGINAL(ecx, edx, pTextureName, pTextureGroupName, complain, nAdditionalCreationFlags) };

	if (CFG::Visuals_Flat_Textures)
	{
		auto replaceWorld = [&]() -> ITexture *
		{
			if (!result
				|| result->IsTranslucent()
				|| !pTextureName
				|| !pTextureGroupName
				|| !std::string_view{ pTextureGroupName }.starts_with(TEXTURE_GROUP_WORLD)
				|| std::string_view{ pTextureName }.find("sky") != std::string_view::npos
				|| std::string_view{ pTextureName }.find("water") != std::string_view::npos)
			{
				return nullptr;
			}

			Vector clr{};

			result->GetLowResColorSample(0.5f, 0.5f, &clr.x);

			unsigned char bits[4]
			{
				static_cast<unsigned char>(clr.x * 255.0f),
				static_cast<unsigned char>(clr.y * 255.0f),
				static_cast<unsigned char>(clr.z * 255.0f),
				255
			};

			return I::MaterialSystem->CreateTextureFromBits(1, 1, 1, IMAGE_FORMAT_RGBA8888, 4, bits);
		};

		if (ITexture *const replacement{ replaceWorld() })
		{
			return replacement;
		}
	}

	return result;
}