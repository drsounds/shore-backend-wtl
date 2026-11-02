#pragma once

#include <shore/backend.h>
#include <shore/frame.h>
#include <shore/toolbar.h>
#include "wtl_frame.h"

namespace shore {
namespace backend {
namespace wtl {

class WtlPeer : public SHFramePeer {
public:
    explicit WtlPeer(SHFrame* frame);
    ~WtlPeer() override;
    void SetTitle(const std::string& title) override;
    void SetStatusText(const std::string& text) override;
    void AddMenuItem(const std::string& menu, const std::string& label, int id) override;
    void SetToolBar(const SHToolBar& toolbar) override;
    void Show(bool show) override;
    void Invalidate() override;
private:
    WtlFrame* window_;
};

}  // namespace wtl
}  // namespace backend
}  // namespace shore
