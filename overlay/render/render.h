
inline void RectFilled(int x, int y, int w, int h, ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color);
}

inline static void FilledRectangle(int x, int y, int w, int h, ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color);
}

inline void Box(int x, int y, int w, int h, ImColor color)
{
	ImGui::GetBackgroundDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), color);
}

inline void DrawCornerBox(int x, int y, int w, int h, int borderPx, ImColor color)
{
	RectFilled(x + borderPx, y, w / 3, borderPx, color);
	RectFilled(x + w - w / 3 + borderPx, y, w / 3, borderPx, color);
	RectFilled(x, y, borderPx, h / 3, color);
	RectFilled(x, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color);
	RectFilled(x + borderPx, y + h + borderPx, w / 3, borderPx, color);
	RectFilled(x + w - w / 3 + borderPx, y + h + borderPx, w / 3, borderPx, color);
	RectFilled(x + w + borderPx, y, borderPx, h / 3, color);
	RectFilled(x + w + borderPx, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color);
}

inline void DrawQuadFilled(ImVec2 p1, ImVec2 p2, ImVec2 p3, ImVec2 p4, ImColor color) {
	ImGui::GetBackgroundDrawList()->AddQuadFilled(p1, p2, p3, p4, color);
}