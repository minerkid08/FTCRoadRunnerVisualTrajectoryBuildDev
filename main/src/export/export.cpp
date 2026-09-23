#include "actions/Action.hpp"
#include "global.hpp"
#include "projectSettings.hpp"
#include "settings.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include "ui/ui.hpp"
#include "utils.hpp"
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>

#ifdef standalone
#include "lua/lauxlib.h"
#include "lua/lualib.h"
#include <lua/lua.h>
#else
#include <lua/lua.hpp>
#endif

static bool raisedErr = false;

static int indexOfAction(const Action* action)
{
	if (action == nullptr)
		return -1;
	for (int i = 0; i < global.actions.size(); i++)
	{
		if (global.actions[i] == action)
			return i + 1;
	}
	return -1;
}

static std::ofstream* file;

static int writeFile(lua_State* l)
{
	const char* c = lua_tostring(l, 1);
	*file << c;
	return 0;
}

static int err(lua_State* l)
{
	raisedErr = true;
	const char* c = lua_tostring(l, 1);
	setErr(c);
	luaL_error(l, c);
	return 0;
}

static int warn(lua_State* l)
{
	const char* c = lua_tostring(l, 1);
	setWarn(c);
	return 0;
}

static int info(lua_State* l)
{
	const char* c = lua_tostring(l, 1);
	setNotif(c);
	return 0;
}

static void processTrajectoryPedro(lua_State* l, const PedroPathing::TrajectoryPedro* trajectory)
{
	lua_newtable(l);
	lua_newtable(l);
	for (int i2 = 0; i2 < trajectory->nodes.count; i2++)
	{
		const PedroPathing::PathNode* node = trajectory->nodes.get(i2);
		lua_newtable(l);
		lua_pushnumber(l, node->pos.x);
		lua_setfield(l, -2, "x");
		lua_pushnumber(l, node->pos.y);
		lua_setfield(l, -2, "y");
		lua_pushnumber(l, node->heading);
		lua_setfield(l, -2, "heading");
		lua_seti(l, -2, i2 + 1);
	}
	lua_setfield(l, -2, "nodes");
	lua_newtable(l);
	for (int i2 = 0; i2 < trajectory->segs.count; i2++)
	{
		const PedroPathing::PathSegment* segment = trajectory->segs.get(i2);
		lua_newtable(l);
		lua_pushnumber(l, segment->startNode + 1);
		lua_setfield(l, -2, "startNode");
		lua_pushnumber(l, segment->endNode + 1);
		lua_setfield(l, -2, "endNode");
		lua_pushnumber(l, segment->headingMode);
		lua_setfield(l, -2, "heading");
		lua_newtable(l);
		for (int i3 = 0; i3 < segment->controlPointsCount; i3++)
		{
			glm::vec2 pos = segment->controlPoints[i3];
			lua_newtable(l);
			lua_pushnumber(l, pos.x);
			lua_setfield(l, -2, "x");
			lua_pushnumber(l, pos.y);
			lua_setfield(l, -2, "y");
			lua_seti(l, -2, i3 + 1);
		}
		lua_setfield(l, -2, "controlPoints");
		lua_seti(l, -2, i2 + 1);
	}
	lua_setfield(l, -2, "segments");
}

static void processTrajectoryRR(lua_State* l, const RoadRunner::TrajectoryRR* trajectory)
{
	lua_newtable(l);
	lua_newtable(l);
	for (int i2 = 0; i2 < trajectory->nodes.count; i2++)
	{
		const RoadRunner::PathNode* node = trajectory->nodes.get(i2);
		lua_newtable(l);
		lua_pushnumber(l, node->pos.x);
		lua_setfield(l, -2, "x");
		lua_pushnumber(l, node->pos.y);
		lua_setfield(l, -2, "y");
		lua_pushnumber(l, node->heading);
		lua_setfield(l, -2, "heading");
		lua_seti(l, -2, i2 + 1);
	}
	lua_setfield(l, -2, "nodes");
	lua_newtable(l);
	for (int i2 = 0; i2 < trajectory->segs.count; i2++)
	{
		const RoadRunner::PathSegment* segment = trajectory->segs.get(i2);
		lua_newtable(l);
		lua_pushnumber(l, segment->startNode + 1);
		lua_setfield(l, -2, "startNode");
		lua_pushnumber(l, segment->endNode + 1);
		lua_setfield(l, -2, "endNode");
		lua_pushnumber(l, segment->headingMode);
		lua_setfield(l, -2, "heading");
		lua_pushnumber(l, segment->startTan);
		lua_setfield(l, -2, "startTan");
		lua_pushnumber(l, segment->endTan);
		lua_setfield(l, -2, "endTan");
		lua_seti(l, -2, i2 + 1);
	}
	lua_setfield(l, -2, "segments");
}

void exportProject(const Action* action)
{
	lua_State* l = luaL_newstate();

	luaL_openlibs(l);

	lua_newtable(l);
	lua_pushcfunction(l, writeFile);
	lua_setfield(l, -2, "write");
	lua_setglobal(l, "file");

	lua_newtable(l);
	lua_pushcfunction(l, info);
	lua_setfield(l, -2, "info");
	lua_pushcfunction(l, warn);
	lua_setfield(l, -2, "warn");
	lua_pushcfunction(l, err);
	lua_setfield(l, -2, "error");
	lua_setglobal(l, "log");

	lua_newtable(l);
	int i = 1;
	for (const Action* action : global.actions)
	{
		lua_newtable(l);
		lua_pushstring(l, global.actionTypes[action->type]);
		lua_setfield(l, -2, "name");
		lua_pushstring(l, action->label);
		lua_setfield(l, -2, "label");
		lua_pushnumber(l, indexOfAction(action->next));
		lua_setfield(l, -2, "next");
		lua_pushnumber(l, indexOfAction(action->prev));
		lua_setfield(l, -2, "prev");
		lua_pushnumber(l, indexOfAction(action->parrent));
		lua_setfield(l, -2, "parent");
		lua_pushnumber(l, indexOfAction(action->actions));
		lua_setfield(l, -2, "actions");
		if (action->type > ACTION_TRAJECTORY)
		{
			lua_newtable(l);
			std::vector<CustomActionField>* fields = action->fields;
			int i2 = 1;
			for (const CustomActionField& field : *fields)
			{
				lua_newtable(l);
				switch (field.type)
				{
				case FIELDTYPE_INT:
					lua_pushinteger(l, field.value.i);
					break;
				case FIELDTYPE_DOUBLE:
					lua_pushnumber(l, field.value.f);
					break;
				case FIELDTYPE_BOOL:
					lua_pushboolean(l, field.value.b);
					break;
				case FIELDTYPE_STRING:
					lua_pushstring(l, field.value.s);
					break;
				}
				lua_setfield(l, -2, "value");
				lua_pushstring(l, field.name);
				lua_setfield(l, -2, "name");
				lua_seti(l, -2, i2++);
			}
			lua_setfield(l, -2, "fields");
		}
		if (action->type == ACTION_TRAJECTORY)
		{
			if (projectSettings.pathType == PathType_Pedro)
			{
				PedroPathing::TrajectoryPedro* trajectory = (PedroPathing::TrajectoryPedro*)action->traj;
				processTrajectoryPedro(l, trajectory);
			}
			else
			{
				RoadRunner::TrajectoryRR* trajectory = (RoadRunner::TrajectoryRR*)action->traj;
				processTrajectoryRR(l, trajectory);
			}
			lua_setfield(l, -2, "trajectory");
		}
		lua_seti(l, -2, i++);
	}
	lua_setglobal(l, "actions");

	lua_pushnumber(l, 0);
	if (projectSettings.pathType == PathType_Pedro)
		lua_setglobal(l, "pedro");
	else
		lua_setglobal(l, "rr");

	luaL_dostring(l, "package.path = \"lang/?.lua\"");

	std::string path = global.filename;

	for (int i = 0; i < path.size(); i++)
	{
		if (path[i] == '\\')
			path[i] = '/';
	}

	if (path.find("/") != std::string::npos)
	{
		path = path.substr(strlen(settings.savePath) + 1);
	}
	std::string prefix = settings.exportPath;
	path = prefix + '/' + path;
	std::string s = path.substr(0, path.find_last_of('/'));
	if (!std::filesystem::exists(s))
		std::filesystem::create_directories(s);
	std::ofstream stream(path);
	file = &stream;

	std::string lang = fmt("lang/%s.lua", global.languages[settings.language]);

	clearMsg();
	raisedErr = false;

	if (luaL_dofile(l, lang.c_str()))
	{
		std::cerr << "lua error:\n" << lua_tostring(l, -1);
		if (!raisedErr)
			setErr("internal script error: check the console for more information");
	}

	lua_close(l);

	stream.close();
}
