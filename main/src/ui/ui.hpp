#include "renderer/FrameBuffer.hpp"
#include "trajectories/Trajectory.hpp"

void setNotif(const std::string& str);
void setWarn(const std::string& str);
void setErr(const std::string& str);
void clearMsg();

void initUi();
void closeUi();

void renderUi(FrameBuffer& framebuffer);
void drawAboutWindow();
void drawActionList();
void drawActionEditor();
void drawTrajectoryEditor(Trajectory* grid);
void drawSettingsMenu();

void openSettings();
void openAboutWindow();
