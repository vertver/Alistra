#pragma once
#include "Base.h"

HWND GetMainWindowHandle();

HWND CreateMainWindow(int Width, int Height);
void DestroyMainWindow();

void MainWindowLoop(bool bAudio);

void MainWindowDestroyEvent();
void SetInitedEvent();

void SetLoadProcess(float floatProcess);
void SetRenderLoadProcess();
float GetRenderLoadProcess();
bool IsRenderWorkDone();