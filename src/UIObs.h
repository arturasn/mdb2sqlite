#ifndef __UI_OBS_H__
#define __UI_OBS_H__
#pragma once

class wxTextAttr;

class CUIObs
{
public:
	virtual void CreateAdditionalItems()				= 0;
	virtual void SetDefaultStyle(const wxTextAttr &atr) = 0;
	virtual void WriteText(std::wstring sMsg)			= 0;
	virtual void SetRange(int nRange)					= 0;
	virtual void SetValue(int nValue)					= 0;
};

#endif