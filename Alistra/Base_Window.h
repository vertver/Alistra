#pragma once
#include "Base.h"

HWND GetMainWindowHandle();

HWND CreateMainWindow();
void DestroyMainWindow();

void MainWindowLoop(boolean bAudio);

void MainWindowDestroyEvent();
void SetInitedEvent();

void SetLoadProcess(float floatProcess);
float GetRenderLoadProcess();
boolean IsRenderWorkDone();