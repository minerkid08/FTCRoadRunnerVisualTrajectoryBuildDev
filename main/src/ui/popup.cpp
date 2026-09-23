#include "popup.hpp"
#include "actions/Action.hpp"
#include "main.hpp"
#include <imgui/imgui.h>

void drawDeletePopups()
{
	if (ImGui::BeginPopupModal("deleteTraj"))
	{
		ImGui::Text("Deleting this action will delete its trajectory.");
		ImGui::Text("Are you sure you want to continue.");
		if (ImGui::Button("confirm"))
		{
			confermAction();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopupModal("deleteContainer"))
	{
		ImGui::Text("Deleting this action will delete all of its sub actions.");
		ImGui::Text("Are you sure you want to continue.");
		if (ImGui::Button("confirm"))
		{
			confermAction();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void drawChangeFromPopups()
{
	if (ImGui::BeginPopupModal("changeFromTraj"))
	{
		ImGui::Text("Changing the type of this action will delete its trajectory.");
		ImGui::Text("Are you sure you want to continue.");
		if (ImGui::Button("confirm"))
		{
			confermAction();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	if (ImGui::BeginPopupModal("changeFromContainer"))
	{
		ImGui::Text("Changing the type of this action will delete all of its sub actions.");
		ImGui::Text("Are you sure you want to continue.");
		if (ImGui::Button("confirm"))
		{
			confermAction();
			ImGui::CloseCurrentPopup();
		}
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void drawQuitPopup()
{
	if (ImGui::BeginPopupModal("quit"))
	{
		ImGui::Text("You might have unsaved changes.");
		if (ImGui::Button("quit"))
			quit();
		ImGui::SameLine();
		if (ImGui::Button("cancel"))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
}

void openChangeFromTrajectory()
{
	ImGui::OpenPopup("changeFromTraj");
}

void openChangeFromContainer()
{
	ImGui::OpenPopup("changeFromContainer");
}

void openDeleteTrajectory()
{
	ImGui::OpenPopup("deleteTraj");
}

void openDeleteContainer()
{
	ImGui::OpenPopup("deleteContainer");
}

void openQuitPopup()
{
	ImGui::OpenPopup("quit");
}
