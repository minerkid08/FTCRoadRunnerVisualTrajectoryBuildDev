#include "FileExplorer.hpp"
#include <cstring>
#include <imgui/imgui.h>
#include <iostream>

static const char* err;

FileExplorer::FileExplorer()
{
}
void FileExplorer::reset(int _flags)
{
	flags = _flags;
	memset(filename, 0, sizeof(filename));
	if (flags & FileExplorerFlags_AlwaysShowBack)
	{
		curPath = std::filesystem::absolute(mainPath);
		std::cout << curPath.string() << '\n';
	}
	else
	{
		curPath = mainPath;
	}
	err = "";
}
void FileExplorer::setMainPath(const std::string& path)
{
	mainPath = path;
	curPath = path;
}
int FileExplorer::render(const char* ext)
{
	if (flags & FileExplorerFlags_MakeFile && flags & FileExplorerFlags_DontShowFiles)
	{
		std::cout << "Explorer: invalid flag combination - makeFile + dontShowFiles\n";
		return 2;
	}

	ImGui::Begin("File Explorer", nullptr, ImGuiWindowFlags_NoDocking);
	if (ImGui::Button("x"))
	{
		ImGui::End();
		return 2;
	}
	if (curPath != mainPath || flags & FileExplorerFlags_AlwaysShowBack)
	{
		ImGui::SameLine();
		if (ImGui::Button("<-"))
		{
			curPath = curPath.parent_path();
		}
	}
	if (flags & FileExplorerFlags_DontShowFiles)
	{
		ImGui::SameLine();
		if (ImGui::Button("+"))
		{
			outPath = curPath;
			ImGui::End();
			return 1;
		}
	}
	if (flags & FileExplorerFlags_MakeFile)
	{
		ImGui::SameLine();
		if (ImGui::Button("+"))
		{
      err = "";
			std::string name(filename);
			for (int i = 0; i < name.size(); i++)
			{
				if (name[i] == '.')
				{
					err = "path name cant contain '.'";
					break;
				}
				if (name[i] == '/')
				{
					name[i] = '\\';
				}
			}
			if (strcmp(err, "") == 0)
			{
				outPath = curPath / (name + std::string(ext));
				ImGui::End();
				return 1;
			}
		}
		if (strcmp(err, "") != 0)
		{
			ImGui::PushStyleColor(ImGuiCol_Text, {1.0f, 0.0f, 0.0f, 1.0f});
			ImGui::Text("%s", err);
			ImGui::PopStyleColor();
		}
		ImGui::SameLine();
		ImGui::Text("%s", curPath.string().c_str());
		ImGui::InputText(" ", filename, sizeof(filename));
	}
	else
	{
		ImGui::SameLine();
		ImGui::Text("%s", curPath.string().c_str());
	}
	buttonSize = ImGui::GetIO().FontGlobalScale * 150;
	float cellSize = buttonSize + padding;
	float panelWidth = ImGui::GetContentRegionAvail().x;
	int columCount = (int)(panelWidth / cellSize);
	if (columCount < 1)
	{
		columCount = 1;
	}
	ImGui::Columns(columCount, 0, false);
	int i = 0;
	for (auto& p : std::filesystem::directory_iterator(curPath))
	{
		ImGui::PushID(i++);
		std::string path = p.path().filename().string();
		if (p.is_directory())
		{
			if (!(flags & FileExplorerFlags_DontShowFolders))
			{
				auto name = p.path().filename();
				if (ImGui::Button("folder.png", ImVec2(buttonSize, buttonSize)))
				{
					curPath /= name;
				}
				ImGui::TextWrapped("%s", path.c_str());
				ImGui::NextColumn();
			}
		}
		ImGui::PopID();
	}
	for (auto& p : std::filesystem::directory_iterator(curPath))
	{
		ImGui::PushID(i++);
		std::string path = p.path().filename().string();
		if (!p.is_directory())
		{
			if (!(flags & FileExplorerFlags_DontShowFiles))
			{
				if (p.path().extension() == ext)
				{
					if (ImGui::Button("file.png", ImVec2(buttonSize, buttonSize)))
					{
						outPath = p.path();
						ImGui::PopID();
						ImGui::End();
						return 1;
					}
					ImGui::TextWrapped("%s", path.c_str());
					ImGui::NextColumn();
				}
			}
		}
		ImGui::PopID();
	}
	ImGui::End();
	return 0;
}
