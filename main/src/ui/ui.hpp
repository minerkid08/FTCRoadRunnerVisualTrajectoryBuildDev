#include "renderer/FrameBuffer.hpp"
#include "trajectories/NodeGrid.hpp"

void setNotif(const std::string& str);
void setWarn(const std::string& str);
void setErr(const std::string& str);
void clearMsg();

void initUi();
void closeUi();

void renderUi(FrameBuffer& framebuffer);
void drawActionList();
void drawActionEditor();
void drawTrajectoryEditor(NodeGrid* grid);
void drawSettingsMenu();

void openSettings();
