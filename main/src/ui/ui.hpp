#include "renderer/FrameBuffer.hpp"
#include "trajectories/NodeGrid.hpp"

void initUi();
void closeUi();

void renderUi(FrameBuffer &framebuffer);
void drawActionList();
void drawActionEditor();
void drawTrajectoryEditor(NodeGrid* grid);
void drawSettingsMenu();

void openSettings();
