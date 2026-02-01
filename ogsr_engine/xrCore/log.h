#pragma once

#define VPUSH(a) a.x, a.y, a.z

void XR_PRINTF(1, 2) Msg(const char* format, ...);
void Log(const xr_string& msg);
void Log(const char* msg);
void Log(const char* msg, const Fvector& dop);
void Log(const char* msg, const Fmatrix& dop);

void CreateLog(BOOL no_log = FALSE);

extern xr_vector<xr_string> LogFile;
extern string_path logFName;

namespace xr
{
namespace detail
{
void log_init();
}

void log_flush();
} // namespace xr
