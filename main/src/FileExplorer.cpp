#include "FileExplorer.hpp"
#include <cstring>
#include <filesystem>
#include <imgui/imgui.h>
#include <iostream>
#include <vector>

static int buttonSize = 256;
static float padding = 16.0f;
static std::filesystem::path outPath;

static std::filesystem::path mainPath;
static std::filesystem::path curPath;

static std::vector<std::filesystem::path> dirs;

static std::vector<std::filesystem::path> files;
static std::vector<std::filesystem::path> directories;

static float inputWidth = 0.0f;

static char filename[256];
static int flags;

static const char* err;

void readDir()
{
	files.clear();
	directories.clear();
	std::filesystem::path path = curPath;
	for (const std::filesystem::path& p : dirs)
		path /= p;

	for (const auto& p : std::filesystem::directory_iterator(path))
	{
		if (p.is_directory())
			directories.emplace_back(p.path().filename());
		if (p.is_regular_file())
			files.emplace_back(p.path().filename());
	}
}

std::string explorerGetPath()
{
	return outPath.string();
}

static bool open = false;

void explorerReset(int _flags)
{
	flags = _flags;
	memset(filename, 0, sizeof(filename));
	if (flags & FileExplorerFlags_AlwaysShowBack)
		curPath = std::filesystem::absolute(mainPath);
	else
		curPath = mainPath;
	dirs.clear();
	err = "";
	readDir();

	inputWidth = ImGui::CalcTextSize("cancel").x;
	inputWidth += ImGui::CalcTextSize("mkdir").x;
	inputWidth += ImGui::CalcTextSize("save").x;

	const ImGuiStyle& style = ImGui::GetStyle();

	inputWidth += style.FramePadding.x * 6;
	inputWidth += style.ItemSpacing.x * 3.5f;
}

void explorerSetPath(const std::string& path)
{
	mainPath = path;
	curPath = path;
}

int explorerUpdate(const char* ext)
{
	if (open == false)
		open = true;
	if (flags & FileExplorerFlags_MakeFile && flags & FileExplorerFlags_DontShowFiles)
	{
		std::cout << "Explorer: invalid flag combination - makeFile + dontShowFiles\n";
		return 2;
	}

	ImGui::Begin("File Explorer", &open, ImGuiWindowFlags_NoDocking);
	ImGui::BeginDisabled(!(dirs.size() > 0 || flags & FileExplorerFlags_AlwaysShowBack));
	if (ImGui::Button("<-"))
	{
		if (dirs.size() == 0)
			curPath = curPath.parent_path();
		else
			dirs.pop_back();
		readDir();
	}
	ImGui::EndDisabled();
	ImGui::SameLine();
	int rm = -1;
	if (ImGui::Button(curPath.c_str()))
		rm = 0;

	for (int i = 0; i < dirs.size(); i++)
	{
		const std::string& dir = dirs[i];
		ImGui::SameLine();
		ImGui::Text("/");
		ImGui::SameLine();
		if (ImGui::Button(dir.c_str()))
			rm = i + 1;
	}
	if (rm > -1)
	{
		for (int j = dirs.size(); j > rm; j--)
			dirs.pop_back();
		readDir();
	}

	buttonSize = ImGui::GetIO().FontGlobalScale * 30;
	ImVec2 size = ImGui::GetContentRegionAvail();

	size.y = size.y -= buttonSize;

	int i = 0;
	if (ImGui::BeginListBox("##list", size))
	{
		ImGui::PushStyleColor(ImGuiCol_Text, {0.0f, 0.0f, 1.0f, 1.0f});
		for (const std::filesystem::path& p : directories)
		{
			ImGui::PushID(i++);
			if (!(flags & FileExplorerFlags_DontShowFolders))
			{
				if (ImGui::Selectable(p.c_str()))
				{
					dirs.emplace_back(p);
					readDir();
				}
			}
			ImGui::PopID();
		}
		ImGui::PopStyleColor();
		for (const std::filesystem::path& p : files)
		{
			ImGui::PushID(i++);
			if (!(flags & FileExplorerFlags_DontShowFiles))
			{
				if (p.extension() == ext)
				{
					if (ImGui::Selectable(p.c_str()))
						strncpy(filename, p.c_str(), sizeof(filename));
				}
			}
			ImGui::PopID();
		}
		ImGui::EndListBox();
	}

	float size2 = ImGui::GetContentRegionAvail().x - inputWidth;

	ImGui::PushItemWidth(size2);
	ImGui::InputText("##input", filename, sizeof(filename));
	ImGui::PopItemWidth();
	ImGui::SameLine();
	if (ImGui::Button("cancel"))
	{
		ImGui::End();
		return FileExplorerUpdate_Close;
	}
	ImGui::SameLine();
	if (ImGui::Button("mkdir"))
	{
		std::filesystem::path path = curPath;
		for (const std::filesystem::path& p : dirs)
			path /= p;
		path /= filename;
		std::filesystem::create_directory(path);
		directories.emplace_back(filename);
	}
	ImGui::SameLine();
	const char* text = 0;
	if (flags & FileExplorerFlags_Save)
		text = "save";
	else
		text = "open";
	if (ImGui::Button(text))
	{
		outPath = curPath;
		for (const std::filesystem::path& p : dirs)
			outPath /= p;
		outPath /= filename;
		if (outPath.extension() != ".path")
			outPath.replace_extension(".path");
		if (!std::filesystem::is_directory(outPath))
		{
			ImGui::End();
			return 1;
		}
	}
	ImGui::End();
	if (open == false)
		return FileExplorerUpdate_Close;
	return 0;
}
