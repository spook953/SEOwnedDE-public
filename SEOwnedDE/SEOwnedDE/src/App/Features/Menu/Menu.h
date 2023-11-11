#pragma once

#include "../../../SDK/SDK.h"

class CMenu
{
private:
	bool m_bOpen = false;
	bool m_bMenuWindowHovered = false;
	int m_nCursorX = 0, m_nCursorY = 0;

	int m_nLastGroupBoxY = 0, m_nLastGroupBoxW = 0;
	int m_nLastButtonW = 0;

	bool m_bClickConsumed = false;
	std::map<void *, bool> m_mapStates = {};

	std::string m_strConfigPath = {};

	std::unique_ptr<Color_t[]> m_pGradient = nullptr;
	unsigned int m_nColorPickerTextureId = 0;

private:
	void Drag(int &x, int &y, int w, int h, int offset_y);
	bool IsHovered(int x, int y, int w, int h, void *pVar, bool bStrict = false);
	bool IsHoveredSimple(int x, int y, int w, int h);

	bool CheckBox(const char *szLabel, bool &bVar);
	bool SliderFloat(const char *szLabel, float &flVar, float flMin, float flMax, float flStep, const char *szFormat);
	bool SliderInt(const char *szLabel, int &nVar, int nMin, int nMax, int nStep);
	bool InputKey(const char *szLabel, int &nKeyOut);
	bool Button(const char *szLabel, bool bActive = false, int nCustomWidth = 0);
	bool playerListButton(const wchar_t *label, int nCustomWidth, Color_t clr, bool center_txt);
	bool InputText(const char *szLabel, const char *szLabel2, std::string &strOutput);
	bool SelectSingle(const char *szLabel, int &nVar, const std::vector<std::pair<const char *, int>> &vecSelects);
	bool SelectMulti(const char *szLabel, std::vector<std::pair<const char *, bool &>> &vecSelects);
	bool ColorPicker(const char *szLabel, Color_t &colVar);
	void GroupBoxStart(const char *szLabel, int nWidth);
	void GroupBoxEnd();

public:
	inline bool IsOpen() { return m_bOpen; }
	inline bool IsMenuWindowHovered() { return m_bMenuWindowHovered; }

private:
	void MainWindow();
	void Snow();
	void Indicators();

public:
	void Run();
	CMenu();
};

MAKE_SINGLETON_SCOPED(CMenu, Menu, F);