#define _AFXDLL
#include "StdAfx.h"
#include "FileUtils.h"
#include "Settings.h"

#include <fstream>
#include "SimpleIni.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
#endif

namespace file_utils
{
	void SaveWindowInformation(int nPosx, int nPosY, int nSizeX)
	{
		std::ofstream WindowPositionFile("WindowInformation.ini");
		WindowPositionFile << "[WindowPosition]" << '\n';
		WindowPositionFile << "x = " << nPosx << '\n';
		WindowPositionFile << "y = " << nPosY << '\n';
		WindowPositionFile << "[WindowSize]" << '\n';
		WindowPositionFile << "width = " << nSizeX << '\n';
		WindowPositionFile.close();
	}

	void GetWindowInformation(int &posx, int &posy, int &sizex)
	{
		CSimpleIni ini;
		ini.SetUnicode();
		ini.LoadFile("WindowInformation.ini");
		posx = ini.GetLongValue(_T("WindowPosition"), _T("x"), 550);
		posy = ini.GetLongValue(_T("WindowPosition"), _T("y"), 200);
		sizex = ini.GetLongValue(_T("WindowSize"), _T("width"), 270);
	}

	void SaveSettings(const std::vector<std::pair<bool, std::wstring>> settings)
	{
		std::wofstream settingfile("Settings.ini");
		settingfile << "[Settings]" << '\n';

		for(auto &it : settings)
		{
			settingfile << it.second.c_str() << _T(" = ");
			if( it.first ) settingfile << _T("true");
			else settingfile << _T("false");
			settingfile << '\n';
		}

		settingfile.close();
	}

	void GetSettings(std::vector<std::pair<bool, std::wstring>> &settings)
	{
		CSimpleIni ini;
		ini.SetUnicode();
		ini.LoadFile("Settings.ini");
		LPCTSTR sKey = L"Settings";
		for(int i = 0; i < ESettings::eSize; ++i) {
			settings[i].first = ini.GetBoolValue(sKey, settings[i].second.c_str(), true);
		}
	}
}