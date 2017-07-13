#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__
#pragma once

#include <vector>

namespace file_utils
{
	void SaveWindowInformation(int nPosx, int nPosY, int nSizeX);
	void GetWindowInformation(int &posx, int &posy, int &sizex);
	void SaveSettings(const std::vector<std::pair<bool, std::wstring>> settings);
	void GetSettings(std::vector<std::pair<bool, std::wstring>> &settings);
};

#endif
