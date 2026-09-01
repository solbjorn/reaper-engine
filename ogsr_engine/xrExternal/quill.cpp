#include "pch.h"

#include "quill.h"

XR_DIAG_PUSH();
XR_DIAG_IGNORE("-Wextra-semi");
XR_DIAG_IGNORE("-Wnewline-eof");
XR_DIAG_IGNORE("-Wnrvo");
XR_DIAG_IGNORE("-Wsign-conversion");

#include <quill/Backend.h>

XR_DIAG_POP();

namespace xr
{
void quill_manual_backend::init(quill::BackendOptions options)
{
    worker = quill::Backend::acquire_manual_backend_worker();
    worker->init(std::move(options));
}

void quill_manual_backend::poll(std::chrono::microseconds timeout) const { worker->poll(timeout); }

void quill_manual_backend::shutdown()
{
    worker->poll();
    worker->shutdown();

    worker = nullptr;
}
} // namespace xr
