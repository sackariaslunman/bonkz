#pragma once

class Input
{
  public:
    bool isPressing = false;
    bool clicked = false;
    bool right = false;
    bool left = false;
    bool up = false;
    bool down = false;

    Input();

    void Reset();
    void Update();
};