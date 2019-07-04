#pragma once

/*
	Our render stuff
*/
char InitRender(char IsImgui);
void DestroyRender();

boolean ResizeRender(int Width, int Height);
boolean RenderDraw();

LPDWORD GetMainWidth();
LPDWORD GetMainHeight();

