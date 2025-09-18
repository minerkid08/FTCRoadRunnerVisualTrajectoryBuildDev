#include "glm/ext/vector_float3.hpp"
#include "renderer/Renderer.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"
#include <vector>

template <typename T, typename I> union Traj
{
  T rr;
  I pedro;
};

struct PreviewPathSegment
{
  Traj<RoadRunner::PathSegment*,PedroPathing::PathSegment*> segment;
  float length;
  float tStart;
  float tEnd;
  float tMul;
};

struct RobotPreview
{
  bool active;
  bool running;

	float sizeX = 12.0f;
	float sizeY = 12.0f;
  glm::vec3 curPos;
	float t = 0.0f;

  float dt;
  float playbackLength;
  bool playing;

  bool useSingleSegment;
  int singleSegmentId;
  Traj<RoadRunner::PathSegment*,PedroPathing::PathSegment*> singleSegment;

  Traj<RoadRunner::TrajectoryRR*,PedroPathing::TrajectoryPedro*> trajectory;
  std::vector<PreviewPathSegment> segments;
};

extern RobotPreview preview;

void generatePathPedro(PedroPathing::TrajectoryPedro* trajectory);
void generatePathRR(RoadRunner::TrajectoryRR* trajectory);
void updatePreview();
void drawPreview(Renderer& renderer);
