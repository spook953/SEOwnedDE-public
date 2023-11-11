#pragma once

#include "Assert/Assert.h"
#include "Color/Color.h"
#include "Config/Config.h"
#include "Hash/Hash.h"
#include "HookManager/HookManager.h"
#include "InterfaceManager/InterfaceManager.h"
#include "Math/Math.h"
#include "Memory/Memory.h"
#include "SignatureManager/SignatureManager.h"
#include "Singleton/Singleton.h"
#include "Vector/Vector.h"

#include <intrin.h>
#include <random>
#include <chrono>
#include <filesystem>
#include <deque>
#include <regex>

namespace Utils
{
    static std::wstring ConvertUtf8ToWide(const std::string &str)
    {
        int count = MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), NULL, 0);
        std::wstring s(count, 0);
        MultiByteToWideChar(CP_UTF8, 0, str.data(), str.length(), &s[0], count);
        return s;
    }

    static std::string ConvertWideToUTF8(const std::wstring &str)
    {
        int len = 0;
        int slength = static_cast<int>(str.length()) + 1;
        len = WideCharToMultiByte(CP_ACP, 0, str.c_str(), slength, 0, 0, 0, 0);
        std::string s(len, 0);
        WideCharToMultiByte(CP_ACP, 0, str.c_str(), slength, &s[0], len, 0, 0);
        return s;
    }

    static int RandInt(int min, int max)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distr(min, max);
        return distr(gen);
    }
}