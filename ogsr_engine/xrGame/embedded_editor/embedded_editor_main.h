#pragma once

void ShowEditor();

bool Editor_KeyPress(int key);
bool Editor_KeyRelease(int key);
bool Editor_KeyHold();
bool Editor_MouseMove();
bool Editor_MouseWheel(int direction);

void Editor_OnFrame();
