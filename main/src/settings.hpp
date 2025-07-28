#pragma once

struct Settings
{
  char* savePath;
  char* exportPath;
  float trajectoryOpac;

  int language;
};

extern Settings settings;

void loadSettings();
void saveSettings();
