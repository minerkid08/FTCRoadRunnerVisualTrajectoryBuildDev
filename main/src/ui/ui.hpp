#include "renderer/FrameBuffer.hpp"
#include "trajectories/TrajectoryPedro.hpp"
#include "trajectories/TrajectoryRR.hpp"

#define LEVEL_NONE -1
#define LEVEL_INFO 0
#define LEVEL_WARN 1
#define LEVEL_ERROR 2

void addLog(int level, const std::string& msg);
void setNotif(const std::string& str);
void setWarn(const std::string& str);
void setErr(const std::string& str);
void clearMsg();

void initUi();
void closeUi();

void renderUi(FrameBuffer& framebuffer, bool shouldClose);
void drawAboutWindow();
void drawActionList();
void drawActionEditor();
void drawTrajectoryEditorRR(RoadRunner::TrajectoryRR* grid);
void drawTrajectoryEditorPedro(PedroPathing::TrajectoryPedro* grid);
void drawSettingsMenu(bool* open);
void drawLog(bool* open);

void openAboutWindow();
