#pragma once

struct Settings
{
  char* savePath;
  char* exportPath;
  float trajectoryOpac = 0.5f;

	float targetFPS = 30.0f;

  int language;
};

extern Settings settings;

void loadSettings();
void saveSettings();
