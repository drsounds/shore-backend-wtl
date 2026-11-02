#pragma once

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>
#include <atlctrls.h>
#include <atlframe.h>
#include <commctrl.h>
#include <map>
#include <shore/frame.h>
#include <shore/toolbar.h>
#include "wtl_canvas_window.h"

namespace shore {
namespace backend {
namespace wtl {

class WtlFrame : public WTL::CFrameWindowImpl<WtlFrame> {
public:
    DECLARE_FRAME_WND_CLASS(_T("ShoreWtlFrame"), 0)
    explicit WtlFrame(SHFrame* frame) : frame_(frame), canvas_(this) {}
    ~WtlFrame() { if (menuBar_) ::DestroyMenu(menuBar_); }

    BEGIN_MSG_MAP(WtlFrame)
        MESSAGE_HANDLER(WM_CREATE, OnCreate)
        MESSAGE_HANDLER(WM_SIZE, OnSize)
        MESSAGE_HANDLER(WM_COMMAND, OnCommand)
        MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
        CHAIN_MSG_MAP(WTL::CFrameWindowImpl<WtlFrame>)
    END_MSG_MAP()

    LRESULT OnCreate(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnSize(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnDestroy(UINT, WPARAM, LPARAM, BOOL&);
    LRESULT OnCommand(UINT, WPARAM wParam, LPARAM, BOOL& handled);
    void SetStatusText(const std::string& text);
    void AddMenuItem(const std::string& menu, const std::string& label, int id);
    void SetToolBar(const SHToolBar& toolbar);
    void InvalidateCanvas() { canvas_.Invalidate(); }
    SHFrame* Frame() const { return frame_; }
    void Layout();
    static const UINT kCommandBase = 0x8000;
private:
    SHFrame* frame_;
    HMENU menuBar_ = nullptr;
    WTL::CToolBarCtrl toolbar_;
    WTL::CStatusBarCtrl statusBar_;
    WtlCanvasWindow canvas_;
    std::map<std::string, HMENU> menus_;
};

}  // namespace wtl
}  // namespace backend
}  // namespace shore
