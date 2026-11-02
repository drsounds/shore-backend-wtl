#pragma once

#include <atlbase.h>
#include <windows.h>
#include <stdexcept>
#include <string>
#include <shore/canvas.h>

namespace shore {
namespace backend {
namespace wtl {

inline std::wstring ToWide(const std::string& value) {
    if (value.empty()) return std::wstring();
    int length = MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, nullptr, 0);
    if (length <= 0) throw std::runtime_error("WTL failed to convert UTF-8 text");
    std::wstring result(static_cast<std::size_t>(length), L'\0');
    if (!MultiByteToWideChar(CP_UTF8, 0, value.c_str(), -1, &result[0], length))
        throw std::runtime_error("WTL failed to convert UTF-8 text");
    result.resize(static_cast<std::size_t>(length - 1));
    return result;
}

class WtlCanvas : public SHCanvas {
public:
    WtlCanvas(HDC dc, const RECT& rect) : dc_(dc), rect_(rect) {}
    int Width() const override { return rect_.right - rect_.left; }
    int Height() const override { return rect_.bottom - rect_.top; }
    void FillRect(int x, int y, int width, int height, SHColor color) override {
        RECT rect = {x, y, x + width, y + height};
        HBRUSH brush = CreateSolidBrush(RGB(color.r, color.g, color.b));
        ::FillRect(dc_, &rect, brush);
        DeleteObject(brush);
    }
    void DrawLine(int x1, int y1, int x2, int y2, SHColor color) override {
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(color.r, color.g, color.b));
        HGDIOBJ oldPen = SelectObject(dc_, pen);
        MoveToEx(dc_, x1, y1, nullptr);
        LineTo(dc_, x2, y2);
        SelectObject(dc_, oldPen);
        DeleteObject(pen);
    }
    void DrawText(int x, int y, const std::string& text, SHColor color) override {
        std::wstring wide = ToWide(text);
        SetTextColor(dc_, RGB(color.r, color.g, color.b));
        SetBkMode(dc_, TRANSPARENT);
        TextOutW(dc_, x, y, wide.c_str(), static_cast<int>(wide.size()));
    }
private:
    HDC dc_;
    RECT rect_;
};

}  // namespace wtl
}  // namespace backend
}  // namespace shore
