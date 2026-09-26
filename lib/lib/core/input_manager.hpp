#pragma once
#include <raylib.h>
#include <raymath.h>

namespace lib
{
    enum ActionType
    {
        ActionConfirm,	// cross
        ActionCancel,	// circle
        ActionUse,		// square
        ActionSpecial,	// triangle
        ActionSprint,	// L3
        ActionPause,    // Start
        ActionSelect,   // Select
    };

    struct  ActionOption
    {
        int key;
        int gamepad_button;
    };

    struct InputManager
    {
        static Vector2 GetMoveDirection();

        static Vector2 GetLookDirection();

        static bool isActionPressed(ActionType action);

        static bool isActionReleased(ActionType action);

        static bool isActionDown(ActionType action);
    };
}