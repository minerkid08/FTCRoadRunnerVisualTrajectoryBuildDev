#pragma once

struct Settings
{
  char* savePath;
  char* exportPath;
  
  int language;
};

extern Settings settings;

void loadSettings();
void saveSettings();
