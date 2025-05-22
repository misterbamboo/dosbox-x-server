#include <string>
//#include "mouse.h"
#include "sdlmain.h"
#include <CmdCallback.cpp>
#include <PathGenerator.cpp>
//#include <server\cmds\PathGenerator.cpp>
//#include <ints/mouse.cpp>
//#include <gui/sdlmain.cpp>

extern int user_cursor_x, user_cursor_y;

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

    static void handle(std::string cmd, CmdCallback callback) {
        if(cmd.rfind(MOUVE_MOVE_CMD, 0) == 0) {
            handlerMouseMoveCmd(cmd, callback);
        }
        else if(cmd.rfind(MOUVE_PRESS_CMD, 0) == 0) {
            registerMouseCallback([callback]() {
                callback("mouse_pressed");
            });
            handlerMousePressedCmd(cmd);
        }
        else if(cmd.rfind(MOUVE_RELEASE_CMD, 0) == 0) {
            registerMouseCallback([callback]() {
                callback("mouse_released");
            });
            handlerMouseReleasedCmd(cmd);
        }
    }

    static void handlerMouseMoveCmd(std::string cmd, CmdCallback callback) {
        std::string xAndYstr = cmd.substr(std::string(MOUVE_MOVE_CMD).size());
        std::string xStr = xAndYstr.substr(0, xAndYstr.find(","));
        std::string yStr = xAndYstr.substr(xAndYstr.find(",") + 1);
        double x = std::stoi(xStr);
        double y = std::stoi(yStr);


        // todo: trigger the server call back only when all mouse event are processed
        registerMouseCallback([callback]() {
            callback("mouse_moved");
        });

        std::vector<std::pair<double, double>> paths = PathGenerator::generateHumanLikePath(
            user_cursor_x, user_cursor_y, x, y);

        double last_x = user_cursor_x;
        double last_y = user_cursor_y;
        // Output the generated path
        for(const auto& point : paths) {

            // type=4 which=0 state=0 coord=(586, 377) rel=(-9, -17)
            SDL_MouseMotionEvent motion;
            motion.type = 4;
            motion.which = 0;
            motion.state = 0;
            motion.x = point.first;
            motion.y = point.second;
            motion.xrel = last_x - point.first;
            motion.yrel = last_y - point.second;

            HandleMouseMotion(&motion);

            last_x = point.first;
            last_y = point.second;
        }

        /*double xRatio = x / MAX_X;
        double yRatio = y / MAX_Y;*/
        //ServerMouse_SursorMoved(xRatio, yRatio, x, y);
    }

    static void handlerMousePressedCmd(std::string cmd) {
        std::string buttonStr = cmd.substr(std::string(MOUVE_PRESS_CMD).size());
        int buttonNum = std::stoi(buttonStr);

        /*
        SDL_MouseButtonEvent: type=5 which=0 button=1 state=1 coord=(592, 386)
        SDL_MouseMotionEvent: type=5 which=0 state=1 coord=(592, 386) rel=(0, 0)
        mouse.events end!
        SDL_MouseButtonEvent: type=6 which=0 button=1 state=0 coord=(592, 386)
        SDL_MouseMotionEvent: type=6 which=0 state=1 coord=(592, 386) rel=(0, 0)
        */

        SDL_MouseButtonEvent button;
        button.type = SDL_MOUSEBUTTONDOWN;
        button.which = 0;
        button.button = 1; // 0: Left, 1: Right, 2: Middle
        button.state = SDL_PRESSED;
        button.x = user_cursor_x;
        button.y = user_cursor_y;

        SDL_MouseMotionEvent motion;
        motion.type = SDL_MOUSEBUTTONDOWN;
        motion.which = 0;
        motion.state = 1;
        motion.x = user_cursor_x;
        motion.y = user_cursor_y;
        motion.xrel = 0;
        motion.yrel = 0;


        HandleMouseButton(&button, &motion);

        // Mouse_ButtonPressed(buttonNum); // 0: Left, 1: Right, 2: Middle
    }

    static void handlerMouseReleasedCmd(std::string cmd) {
        std::string buttonStr = cmd.substr(std::string(MOUVE_RELEASE_CMD).size());
        int buttonNum = std::stoi(buttonStr);

        SDL_MouseButtonEvent button;
        button.type = SDL_MOUSEBUTTONUP;
        button.which = 0;
        button.button = 1; // 0: Left, 1: Right, 2: Middle
        button.state = SDL_RELEASED;
        button.x = user_cursor_x;
        button.y = user_cursor_y;

        SDL_MouseMotionEvent motion;
        motion.type = SDL_MOUSEBUTTONUP;
        motion.which = 0;
        motion.state = 1;
        motion.x = user_cursor_x;
        motion.y = user_cursor_y;
        motion.xrel = 0;
        motion.yrel = 0;

        HandleMouseButton(&button, &motion);

        //Mouse_ButtonReleased(buttonNum); // 0: Left, 1: Right, 2: Middle
    }
};
