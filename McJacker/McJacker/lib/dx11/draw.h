#pragma once
#include "../pch.h"

struct RGB {
    int r, g, b;
};

class Draw {
public:
    Draw(ImDrawList* drawList);

    void DrawLine(int x1, int y1, int x2, int y2, RGB color, int thickness);
    void DrawRect(int x, int y, int w, int h, RGB color, int thickness);

private:
    ImDrawList* drawList;

    ImU32 Color(const RGB& color) const;
};
