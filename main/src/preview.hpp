#include "List.hpp"
#include "glm/ext/vector_float2.hpp"
#include "glm/ext/vector_float3.hpp"
#include "renderer/Renderer.hpp"

struct RobotPreview
{
  bool active;
  bool running;

	float sizeX;
	float sizeY;
  glm::vec3 curPos;
	float t;

  glm::vec2 start;
  glm::vec2 end;
  float startTan;
  float endTan;

  List<glm::vec2>* ctrlPts;
};

extern RobotPreview preview;

void updatePreview();
void drawPreview(Renderer& renderer);
