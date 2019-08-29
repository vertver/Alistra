/****************************************************************
* MZPE Team, 2019.
* Alistra intro
* License: MIT
*****************************************************************/
#pragma once
#include "Base.h"

HWND GetMainWindowHandle();

HWND CreateMainWindow(int Width, int Height);
void DestroyMainWindow();

void MainWindowLoop(bool bAudio);

void MainWindowDestroyEvent();
void SetInitedEvent();

void SetLoadProcess(float floatProcess);
float GetRenderLoadProcess();
bool IsRenderWorkDone();