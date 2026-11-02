#include <atlbase.h>
#include <atlapp.h>
#include <commctrl.h>
#include <stdexcept>
#include "wtl_backend_impl.h"
#include "wtl_peer.h"
#include <shore/app.h>
#include <shore/view.h>
#include <shore-backend-wtl/backend.h>

WTL::CAppModule _Module;

namespace shore {
namespace backend {
namespace wtl {
namespace {
int g_frameCount = 0;
}

LRESULT WtlCanvasWindow::OnPaint(UINT, WPARAM, LPARAM, BOOL&) {
    PAINTSTRUCT paint;
    HDC dc = BeginPaint(&paint);
    RECT rect;
    GetClientRect(&rect);
    HBRUSH background = CreateSolidBrush(RGB(255, 255, 255));
    ::FillRect(dc, &rect, background);
    DeleteObject(background);
    WtlCanvas canvas(dc, rect);
    frame_->Frame()->Paint(canvas);
    EndPaint(&paint);
    return 0;
}
LRESULT WtlCanvasWindow::OnLButtonDown(UINT, WPARAM, LPARAM lParam, BOOL&) {
    if (frame_->Frame()->View())
        frame_->Frame()->View()->OnMouseDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    return 0;
}
LRESULT WtlFrame::OnCreate(UINT, WPARAM, LPARAM, BOOL&) {
    menuBar_ = ::CreateMenu();
    if (!menuBar_ || !::SetMenu(m_hWnd, menuBar_)) return -1;
    RECT rect = {0, 0, 0, 0};
    if (!toolbar_.Create(m_hWnd, rect, nullptr,
            WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | CCS_TOP | TBSTYLE_FLAT | TBSTYLE_LIST,
            0, 1001))
        return -1;
    toolbar_.SendMessage(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    if (!statusBar_.Create(m_hWnd, rect, nullptr,
            WS_CHILD | WS_VISIBLE | CCS_BOTTOM | SBARS_SIZEGRIP, 0, 1002))
        return -1;
    int parts[] = {-1};
    statusBar_.SetParts(1, parts);
    if (!canvas_.Create(m_hWnd, rect, nullptr, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, 0, 1003))
        return -1;
    Layout();
    ++g_frameCount;
    return 0;
}
LRESULT WtlFrame::OnSize(UINT, WPARAM, LPARAM, BOOL&) { Layout(); return 0; }
LRESULT WtlFrame::OnDestroy(UINT, WPARAM, LPARAM, BOOL& handled) {
    if (g_frameCount > 0 && --g_frameCount == 0) ::PostQuitMessage(0);
    handled = TRUE;
    return 0;
}
LRESULT WtlFrame::OnCommand(UINT, WPARAM wParam, LPARAM, BOOL& handled) {
    UINT id = LOWORD(wParam);
    if (id >= kCommandBase && frame_->Dispatch(static_cast<int>(id - kCommandBase))) {
        handled = TRUE;
        return 0;
    }
    handled = FALSE;
    return 0;
}
void WtlFrame::SetStatusText(const std::string& text) {
    std::wstring wide = ToWide(text);
    ::SendMessageW(statusBar_.m_hWnd, SB_SETTEXTW, 0, reinterpret_cast<LPARAM>(wide.c_str()));
}
void WtlFrame::AddMenuItem(const std::string& menu, const std::string& label, int id) {
    HMENU& popup = menus_[menu];
    if (!popup) {
        popup = ::CreatePopupMenu();
        if (!popup) return;
        std::wstring menuText = ToWide(menu);
        ::AppendMenuW(menuBar_, MF_POPUP, reinterpret_cast<UINT_PTR>(popup), menuText.c_str());
    }
    std::wstring itemText = ToWide(label);
    ::AppendMenuW(popup, MF_STRING, kCommandBase + id, itemText.c_str());
    DrawMenuBar();
}
void WtlFrame::SetToolBar(const SHToolBar& toolbar) {
    while (toolbar_.GetButtonCount() > 0) toolbar_.DeleteButton(0);
    for (const auto& item : toolbar.Items()) {
        TBBUTTON button = {};
        if (item.separator) {
            button.fsStyle = BTNS_SEP;
        } else {
            std::wstring label = ToWide(item.label);
            button.iBitmap = I_IMAGENONE;
            button.idCommand = kCommandBase + item.id;
            button.fsState = TBSTATE_ENABLED;
            button.fsStyle = BTNS_BUTTON | BTNS_AUTOSIZE | BTNS_SHOWTEXT;
            button.iString = static_cast<INT_PTR>(
                ::SendMessageW(toolbar_.m_hWnd, TB_ADDSTRINGW, 0,
                               reinterpret_cast<LPARAM>(label.c_str())));
        }
        toolbar_.AddButtons(1, &button);
    }
    toolbar_.AutoSize();
    Layout();
}
void WtlFrame::Layout() {
    if (!::IsWindow(m_hWnd) || !::IsWindow(toolbar_.m_hWnd) ||
        !::IsWindow(statusBar_.m_hWnd) || !::IsWindow(canvas_.m_hWnd))
        return;
    RECT client;
    GetClientRect(&client);
    const int toolbarHeight = 30;
    const int statusHeight = 22;
    int width = client.right - client.left;
    int height = client.bottom - client.top;
    toolbar_.SetWindowPos(nullptr, 0, 0, width, toolbarHeight, SWP_NOZORDER | SWP_NOACTIVATE);
    statusBar_.SetWindowPos(nullptr, 0, height - statusHeight, width, statusHeight,
                            SWP_NOZORDER | SWP_NOACTIVATE);
    canvas_.SetWindowPos(nullptr, 0, toolbarHeight, width, height - toolbarHeight - statusHeight,
                         SWP_NOZORDER | SWP_NOACTIVATE);
}

WtlPeer::WtlPeer(SHFrame* frame) : window_(new WtlFrame(frame)) {
    RECT rect = {0, 0, frame->Width(), frame->Height()};
    if (!window_->Create(nullptr, rect, ToWide(frame->Title()).c_str(), WS_OVERLAPPEDWINDOW)) {
        delete window_;
        window_ = nullptr;
        throw std::runtime_error("WTL failed to create a Shore frame");
    }
}
WtlPeer::~WtlPeer() {
    if (window_ && ::IsWindow(window_->m_hWnd)) window_->DestroyWindow();
    delete window_;
}
void WtlPeer::SetTitle(const std::string& title) {
    ::SetWindowTextW(window_->m_hWnd, ToWide(title).c_str());
}
void WtlPeer::SetStatusText(const std::string& text) { window_->SetStatusText(text); }
void WtlPeer::AddMenuItem(const std::string& menu, const std::string& label, int id) {
    window_->AddMenuItem(menu, label, id);
}
void WtlPeer::SetToolBar(const SHToolBar& toolbar) { window_->SetToolBar(toolbar); }
void WtlPeer::Show(bool show) { window_->ShowWindow(show ? SW_SHOW : SW_HIDE); }
void WtlPeer::Invalidate() { window_->InvalidateCanvas(); }

std::unique_ptr<SHFramePeer> WtlBackend::CreateFramePeer(SHFrame* frame) {
    return std::unique_ptr<SHFramePeer>(new WtlPeer(frame));
}
int WtlBackend::Run(SHApp* app, int, char**) {
    INITCOMMONCONTROLSEX controls = {sizeof(controls), ICC_BAR_CLASSES};
    InitCommonControlsEx(&controls);
    if (FAILED(_Module.Init(nullptr, GetModuleHandle(nullptr)))) return 1;
    WTL::CMessageLoop loop;
    _Module.AddMessageLoop(&loop);
    int result = app->OnInit() ? loop.Run() : 1;
    _Module.RemoveMessageLoop();
    _Module.Term();
    return result;
}
void WtlBackend::Quit() { PostQuitMessage(0); }
const char* WtlBackend::Name() const { return "WTL"; }

std::unique_ptr<SHBackend> CreateBackend() {
    return std::unique_ptr<SHBackend>(new WtlBackend);
}

}  // namespace wtl
}  // namespace backend
}  // namespace shore
