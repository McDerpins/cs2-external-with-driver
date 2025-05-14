#include "draw.h"

Draw::Draw(ImDrawList* drawList) : drawList(drawList) {}

ImU32 Draw::Color(const RGB& color) const {
    return IM_COL32(color.r, color.g, color.b, 255);
}

void Draw::DrawLine(int x1, int y1, int x2, int y2, RGB color, int thickness) {
    drawList->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), Color(color), thickness);
}

void Draw::DrawRect(int x, int y, int w, int h, RGB color, int thickness) {
    drawList->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), Color(color), 0, 0, thickness);
}
