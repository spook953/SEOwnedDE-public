#include "../../../SDK/SDK.h"

#include "../CFG.h"

//pasted from mfed :muscle:

const std::vector<std::pair<std::string, std::string>> patterns
{
    { "r", "w" },
    { "l", "w" },
    { "R", "W" },
    { "L", "W" },
    { "n([aeiou])", "ny$1" },
    { "N([aeiou])", "Ny$1" },
    { "N([AEIOU])", "NY$1" }
};

const std::vector<std::string> suffixes
{
    " UwU", " OwO", " owo", " uwu", " nwn", " :3", " >w<", " ^w^", " <3"
};

std::string owoify(std::string text)
{
    for (auto &pattern : patterns)
    {
        std::regex reg(pattern.first);

        text = std::regex_replace(text, reg, pattern.second);
    }

    if (text.size() < 124)
    {
        int suffix{ I::UniformRandomStream->RandomInt(0, suffixes.size() - 1) };

        text += suffixes[suffix];
    }

    return text;
}

MAKE_HOOK(
    IVEngineClient013_ClientCmd_Unrestricted,
    Memory::GetVFunc(I::EngineClient, 106),
    void,
    __fastcall,
    void *ecx, void *edx, const char *szCmdString)
{
    if (CFG::Misc_Chat_Owoify)
    {
        if (strstr(szCmdString, "say"))
        {
            std::string cmdString(szCmdString);

            if (cmdString.rfind("say", 0) != 0)
            {
                return CALL_ORIGINAL(ecx, edx, szCmdString);
            }

            std::smatch match{};
            std::regex_search(cmdString, match, std::regex("(say.* )\"(.*)\""));

            cmdString = match[1].str() + "\"" + owoify(match[2].str()) + "\"";

            return CALL_ORIGINAL(ecx, edx, cmdString.c_str());
        }
    }

    return CALL_ORIGINAL(ecx, edx, szCmdString);
}

MAKE_HOOK(
    C_TFGameRules_ModifySentChat,
    Signatures::C_TFGameRules_ModifySentChat.Get(),
    void,
    __fastcall,
    void *ecx, void *edx, char *pBuf, int iBufSize)
{
    static ConVar *tf_medieval_autorp{ I::CVar->FindVar("tf_medieval_autorp") };
    static ConVar *english{ I::CVar->FindVar("english") };

    if (CFG::Misc_Chat_Medieval && pBuf && iBufSize && tf_medieval_autorp && english)
    {
        struct s_CTFGameRules
        {
            char pad[1051]{};
            bool m_bPlayingMedieval{};
        };

        s_CTFGameRules *gamerules{ reinterpret_cast<s_CTFGameRules *>(ecx) };

        if (!tf_medieval_autorp || !english || !gamerules)
        {
            return CALL_ORIGINAL(ecx, edx, pBuf, iBufSize);
        }

        bool originalEnglish{ english->GetBool() };
        bool originalAutoRP{ tf_medieval_autorp->GetBool() };
        bool originalPlaying{ gamerules->m_bPlayingMedieval };

        gamerules->m_bPlayingMedieval = true;
        tf_medieval_autorp->SetValue(true);
        english->SetValue(true);

        CALL_ORIGINAL(ecx, edx, pBuf, iBufSize);

        gamerules->m_bPlayingMedieval = originalPlaying;
        tf_medieval_autorp->SetValue(originalAutoRP);
        english->SetValue(originalEnglish);

        return;
    }

    CALL_ORIGINAL(ecx, edx, pBuf, iBufSize);
}