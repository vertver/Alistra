/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once

/*
	Our render stuff
*/
bool InitRender(bool IsImgui, bool bVSYNC, bool bFS);
void DestroyRender();

bool ResizeRender(int Width, int Height);
bool RenderDraw();

LPDWORD GetMainWidth();
LPDWORD GetMainHeight();

