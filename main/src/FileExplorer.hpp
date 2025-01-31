#pragma once
#include <string>

#define FileExplorerFlags_None 0
#define FileExplorerFlags_MakeFile 1
#define FileExplorerFlags_DontShowFolders 1 << 1
#define FileExplorerFlags_DontShowFiles 1 << 2
#define FileExplorerFlags_AlwaysShowBack 1 << 3

#define FileExplorerUpdate_Normal 0
#define FileExplorerUpdate_PathSelected 1
#define FileExplorerUpdate_Close 2

void explorerSetPath(const std::string& path);
int explorerUpdate(const char* ext);
void explorerReset(int flags = FileExplorerFlags_None);
std::string explorerGetPath();
