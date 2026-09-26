#include "input_manager.hpp"
#include <map>
#include <Vector>

namespace lib
{

    static Vector2 left_deadzone = { 0.001f, 0.001f };
    static Vector2 right_deadzone = {0.1f, 0.3f};

    static std::map<ActionType, ActionOption> action_mapping = {
        {ActionConfirm, ActionOption{.key = KEY_ENTER, .gamepad_button = GAMEPAD_BUTTON_RIGHT_FACE_DOWN}},
        {ActionCancel, ActionOption{.key = KEY_BACKSPACE, .gamepad_button = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT}},
        {ActionUse, ActionOption{.key = KEY_E, .gamepad_button = GAMEPAD_BUTTON_RIGHT_FACE_LEFT}},
        {ActionSpecial, ActionOption{.key = KEY_I, .gamepad_button = GAMEPAD_BUTTON_RIGHT_FACE_UP}},
        {ActionPause, ActionOption{.key = KEY_ESCAPE, .gamepad_button = GAMEPAD_BUTTON_MIDDLE_RIGHT}},
        {ActionSelect, ActionOption{.key = KEY_TAB, .gamepad_button = GAMEPAD_BUTTON_MIDDLE_LEFT}},
        {ActionSprint, ActionOption{.key = KEY_RIGHT_SHIFT, .gamepad_button = GAMEPAD_BUTTON_LEFT_THUMB}},
    };

    static Vector2 GetRightJoystickDirection()
    {
        float x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_X);
        float y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_RIGHT_Y);
        if (fabsf(x) < right_deadzone.x)
            x = 0.0f;

        if (fabsf(y) < right_deadzone.y)
            y = 0.0f;
        return Vector2{ x,y };
    }

    static Vector2 GetLeftJoystickDirection()
    {
        float x = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
        float y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);

        if (fabsf(x) < left_deadzone.x)
            x = 0.0f;

        if (fabsf(y) < left_deadzone.y)
            y = 0.0f;

        return Vector2{ x,y };

    }


    Vector2 InputManager::GetMoveDirection()
    {

        int wasd_x = IsKeyDown(KEY_D) - IsKeyDown(KEY_A);
        int wasd_y = IsKeyDown(KEY_S) - IsKeyDown(KEY_W);
        Vector2 wasd_dir = {.x = (float)wasd_x, .y = (float)wasd_y};
        Vector2 joy_dir = GetLeftJoystickDirection();

        Vector2 clamped = wasd_dir + joy_dir;
        clamped = Vector2Clamp(clamped, { -1, -1 }, { 1, 1 });

        return clamped;
    }

    Vector2 InputManager::GetLookDirection()
    {
        Vector2 mouse_dir = IsMouseButtonDown(MOUSE_BUTTON_RIGHT) ? Vector2Normalize(GetMouseDelta()) : Vector2{0.0f, 0.0f};
        Vector2 joy_dir = GetRightJoystickDirection();

        Vector2 clamped = mouse_dir + joy_dir;
        clamped = Vector2Clamp(clamped, { -1, -1 }, { 1, 1 });

        return clamped;
    }

    bool InputManager::isActionPressed(ActionType action)
    {
        const ActionOption& input = action_mapping[action];

        bool value = IsKeyPressed(input.key) || IsGamepadButtonPressed(0, input.gamepad_button);
        return value;
    }

    bool InputManager::isActionReleased(ActionType action)
    {
        const ActionOption& input = action_mapping[action];

        bool value = IsKeyReleased(input.key) || IsGamepadButtonReleased(0, input.gamepad_button);
        return value;
    }

    bool InputManager::isActionDown(ActionType action)
    {
        const ActionOption& input = action_mapping[action];

        bool value = IsKeyDown(input.key) || IsGamepadButtonDown(0, input.gamepad_button);
        return value;
    }
}