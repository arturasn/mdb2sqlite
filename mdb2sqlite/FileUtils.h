#ifndef __FILE_UTILS_H__
#define __FILE_UTILS_H__
#pragma once

namespace file_utils
{
	void SaveWindowInformation(int nPosx, int nPosY, int nSizeX);
	void GetWindowInformation(int &posx, int &posy, int &sizex);
};

#endif
