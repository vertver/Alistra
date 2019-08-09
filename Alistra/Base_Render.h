#pragma once

/*
	Our render stuff
*/
bool InitRender(bool IsImgui);
void DestroyRender();

bool ResizeRender(int Width, int Height);
bool RenderDraw();

LPDWORD GetMainWidth();
LPDWORD GetMainHeight();

