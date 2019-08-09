#pragma once
#include "Base.h"

HWND GetMainWindowHandle();

HWND CreateMainWindow();
void DestroyMainWindow();

void MainWindowLoop(bool bAudio);

void MainWindowDestroyEvent();
void SetInitedEvent();

void SetLoadProcess(float floatProcess);
float GetRenderLoadProcess();
bool IsRenderWorkDone();