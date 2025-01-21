#pragma once

#define ActionFlags_Deleted 1

struct Action
{
	int type = 0;
  char flags = 0;
  unsigned long long id = 0;
  void* data;
  int dataSize;

  Action* next = nullptr;
  Action* prev = nullptr;
  Action* actions = nullptr;
  Action* parrent = nullptr;
};
