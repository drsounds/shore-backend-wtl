#pragma once

#include <atlwin.h>
#include <windowsx.h>
#include <shore/frame.h>
#include "wtl_canvas.h"

namespace shore {
namespace backend {
namespace wtl {

class WtlFrame;

class WtlCanvasWindow : public ATL::CWindowImpl<WtlCanvasWindow> {
public:
    DECLARE_WND_CLASS(_T("ShoreWtlCanvas"))
    explicit WtlCanvasWindow(WtlFrame* frame) : frame_(frame) {}

    BEGIN_MSG_MAP(WtlCanvasWindow)
        MESSAGE_HANDLER(WM_PAINT, OnPaint)
        MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
        MESSAGE_HANDLER(WM_LBUTTONDOWN, OnLButtonDown)
    END_MSG_MAP()

    LRESULT OnPaint(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnEraseBkgnd(UINT, WPARAM, LPARAM, BOOL&) { return 1; }
    LRESULT OnLButtonDown(UINT, WPARAM, LPARAM lParam, BOOL&);
private:
    WtlFrame* frame_;
};

}  // namespace wtl
}  // namespace backend
}  // namespace shore
