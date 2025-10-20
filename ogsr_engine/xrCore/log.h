#pragma once

#define VPUSH(a) a.x, a.y, a.z

void XR_PRINTF(1, 2) Msg(const char* format, ...);
void Log(const std::string& msg);
void Log(const char* msg);
void Log(const char* msg, const Fvector& dop);
void Log(const char* msg, const Fmatrix& dop);

using LogCallback = std::function<void(const char*)>;
void SetLogCB(LogCallback cb);
void CreateLog(BOOL no_log = FALSE);

extern xr_vector<std::string> LogFile;
extern string_path logFName;
