#ifndef __UI_OBS_H__
#define __UI_OBS_H__
#pragma once

class wxTextAttr;

class CUIObs
{
public:
	virtual void CreateAdditionalItems() = 0;
	virtual void SetDefaultStyle(const wxTextAttr &atr) = 0;
	virtual void WriteText(wxString sMsg) = 0;
	virtual void SetRange(int range) = 0;
	virtual void SetValue(int value) = 0;
};


#endif