#include <string>
#include "mouse.h"
#include <CmdCallback.cpp>
//#include <ints/mouse.cpp>
#include <server\result\ServerResult.h>

class MouseCmd {
public:
    static constexpr const char* MOUVE_MOVE_CMD = "mouse_move,"; // ...,x,y
    static constexpr const char* MOUVE_PRESS_CMD = "mouse_press,"; // ...,button_num
    static constexpr const char* MOUVE_RELEASE_CMD = "mouse_release,"; // ...,button_num

    static constexpr const double MAX_X = 640.0;
    static constexpr const double MAX_Y = 480.0;

    static bool canHandle(std::string cmd) {
        if(cmd.rfind(MOUVE_MOVE_CMD, 0) == 0) {
            return true;
        }
        else if(cmd.rfind(MOUVE_PRESS_CMD, 0) == 0) {
            return true;
        }
        else if(cmd.rfind(MOUVE_RELEASE_CMD, 0) == 0) {
            return true;
        }
        return false;
    }

    static void handle(std::string cmd, ServerResult* serverResult, CmdCallback callback) {
        if(cmd.rfind(MOUVE_MOVE_CMD, 0) == 0) {
            registerMouseCallback([callback, serverResult]() {
                serverResult->setTrue();
                callback(serverResult);
            });
            handlerMouseMoveCmd(cmd);
        }
        else if(cmd.rfind(MOUVE_PRESS_CMD, 0) == 0) {
            registerMouseCallback([callback, serverResult]() {
                serverResult->setTrue();
                callback(serverResult);
            });
            handlerMousePressedCmd(cmd);
        }
        else if(cmd.rfind(MOUVE_RELEASE_CMD, 0) == 0) {
            registerMouseCallback([callback, serverResult]() {
                serverResult->setTrue();
                callback(serverResult);
            });
            handlerMouseReleasedCmd(cmd);
        }
        else {
            serverResult->setFalse();
            callback(serverResult);
        }
    }

    static void handlerMouseMoveCmd(std::string cmd) {
        std::string xAndYstr = cmd.substr(std::string(MOUVE_MOVE_CMD).size());
        std::string xStr = xAndYstr.substr(0, xAndYstr.find(","));
        std::string yStr = xAndYstr.substr(xAndYstr.find(",") + 1);
        double x = std::stoi(xStr);
        double y = std::stoi(yStr);

        double xRatio = x / MAX_X;
        double yRatio = y / MAX_Y;
        ServerMouse_SursorMoved(x, y, xRatio, yRatio);
    }

    static void handlerMousePressedCmd(std::string cmd) {
        std::string button = cmd.substr(std::string(MOUVE_PRESS_CMD).size());
        int buttonNum = std::stoi(button);
        Mouse_ButtonPressed(buttonNum); // 0: Left, 1: Right, 2: Middle
    }

    static void handlerMouseReleasedCmd(std::string cmd) {
        std::string button = cmd.substr(std::string(MOUVE_RELEASE_CMD).size());
        int buttonNum = std::stoi(button);
        Mouse_ButtonReleased(buttonNum); // 0: Left, 1: Right, 2: Middle
    }
};
