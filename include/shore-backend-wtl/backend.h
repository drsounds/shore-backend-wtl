#pragma once

#include <memory>
#include <shore/backend.h>

namespace shore {
namespace backend {
namespace wtl {

std::unique_ptr<SHBackend> CreateBackend();

}  // namespace wtl
}  // namespace backend
}  // namespace shore
