#include "preview.hpp"
#include "curve.hpp"
#include "glm/ext/vector_float2.hpp"
#include "projectSettings.hpp"
#include "renderer/Renderer.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include <cmath>

RobotPreview preview;

void updatePreview()
{
	if (projectSettings.pathType == Trajectory_RR)
	{
		preview.curPos = getPosRR(preview.start, preview.end, preview.startTan, preview.endTan, preview.t);
	}
	if (projectSettings.pathType == Trajectory_Pedro)
	{
		glm::vec2 pos = bezierPosition(preview.ctrlPts, preview.t);
		glm::vec2 tangent = bezierTangent(preview.ctrlPts, preview.t);
		preview.curPos = {pos.x, pos.y, atan2(tangent.y, tangent.x)};
	}
}

void drawPreview(Renderer& renderer)
{
  renderer.drawRobot(preview.curPos, preview.sizeX, preview.sizeY);
}
