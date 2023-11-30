#pragma once
#include <filesystem>
#define FileExplorerFlags_None 0
#define FileExplorerFlags_MakeFile 1
#define FileExplorerFlags_DontShowFolders 1 << 1
#define FileExplorerFlags_DontShowFiles 1 << 2
#define FileExplorerFlags_AlwaysShowBack 1 << 3
class FileExplorer{
	public:
	FileExplorer();
	void setMainPath(const std::string& path);
	int render(const char* ext);//0 normal, 1 done, 2 close
	void reset(int _flags = FileExplorerFlags_None);
	int buttonSize = 256;
	float padding = 16.0f;
	std::filesystem::path outPath;
	private:
	std::filesystem::path mainPath;
	std::filesystem::path curPath;
	char filename[256];
	int flags;
};