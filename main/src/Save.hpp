#pragma once

#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include "json/json.hpp"
#include <iostream>
#include <string>

void save(const std::string& filename);
void load(const std::string& filename);

namespace RoadRunner
{
void saveTrajectory(const TrajectoryRR* trajectory, nlohmann::json& j);
TrajectoryRR* parseTrajectory(const nlohmann::json& json, int ind);
} // namespace RoadRunner
namespace PedroPathing
{
void saveTrajectory(const TrajectoryPedro* trajectory, nlohmann::json& j);
TrajectoryPedro* parseTrajectory(const nlohmann::json& json, int ind);
} // namespace PedroPathing

#define tryGet(json, key, type2, out)                                                                                  \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		std::cerr << "cant get key: " << key << '\n';                                                                  \
		goto err;                                                                                                      \
	}                                                                                                                  \
	if (!json[key].type2())                                                                                            \
	{                                                                                                                  \
		std::cerr << "wrong type for key: " << key << " expected " << json[key].type_name() << ", " << __LINE__ << ", " << __FILE__ << '\n';                   \
		goto err;                                                                                                      \
	}                                                                                                                  \
	out = json[key];

#define tryGetc(json, key, type2, out, cast)                                                                           \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		std::cerr << "cant get key: " << key << '\n';                                                                  \
		goto err;                                                                                                      \
	}                                                                                                                  \
	if (!json[key].type2())                                                                                            \
	{                                                                                                                  \
		std::cerr << "wrong type for key: " << key << " expected " << json[key].type_name() << '\n';                   \
		goto err;                                                                                                      \
	}                                                                                                                  \
	out = cast json[key];

#define typeCheck(json, key, type2)                                                                                    \
	if (!json.contains(key))                                                                                           \
	{                                                                                                                  \
		std::cerr << "cant get key: " << key << '\n';                                                                  \
		goto err;                                                                                                      \
	}                                                                                                                  \
	if (!json[key].type2())                                                                                            \
	{                                                                                                                  \
		std::cerr << "wrong type for key: " << key << " expected " << json[key].type_name() << ", " << __LINE__ << ", " << __FILE__ << '\n';                   \
		goto err;                                                                                                      \
	}
