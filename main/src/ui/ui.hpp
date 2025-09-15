#include "renderer/FrameBuffer.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"

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
void drawTrajectoryEditorRR(RoadRunner::TrajectoryRR* grid);
void drawTrajectoryEditorPedro(PedroPathing::TrajectoryPedro* grid);
void drawSettingsMenu();

void openSettings();
void openAboutWindow();
