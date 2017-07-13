#define _AFXDLL
#include "StdAfx.h"
#include "FileUtils.h"

#include <fstream>
#include "SimpleIni.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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
}