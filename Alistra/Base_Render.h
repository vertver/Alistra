#pragma once
#include "glcorearb.h"

/*
	GL Stuff
*/
void *get_proc(const char *proc);
char OpenGL_Library();
void load_procs();

/*
	Our render stuff
*/
char InitRender(char IsImgui);
void DestroyRender();

boolean ResizeRender(int Width, int Height);
boolean RenderDraw();

LPDWORD GetMainWidth();
LPDWORD GetMainHeight();

