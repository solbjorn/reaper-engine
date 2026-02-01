#include "stdafx.h"

#include "log.h"

#include "xrcpuid.h"

#include <SFML/System/Err.hpp>

#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <absl/log/log_sink_registry.h>

#include <bitmap_object_pool.hpp>

#include <fstream>

xr_vector<xr_string> LogFile;
string_path logFName;

namespace xr
{
namespace
{
class log_pool
{
public:
    using msg_vec = xr_vector<std::pair<size_t, xr_string>>;

private:
    class absl_sink final : public absl::LogSink
    {
    public:
        ~absl_sink() override = default;

        void Send(const absl::LogEntry& entry) override
        {
            gsl::czstring pfx;

            switch (entry.log_severity())
            {
            case absl::LogSeverity::kInfo: pfx = "* Abseil: "; break;
            case absl::LogSeverity::kWarning: pfx = "~ Abseil: "; break;
            case absl::LogSeverity::kError:
            case absl::LogSeverity::kFatal: pfx = "! Abseil: "; break;
            default: pfx = "Abseil: "; break;
            }

            Log(pfx + xr_string{entry.stacktrace().empty() ? entry.text_message() : entry.stacktrace()});
        }
    };

    tzcnt_utils::BitmapObjectPool<msg_vec> pool;
    std::atomic<gsl::index> calls{};
    std::ofstream logfs;

    std::stringstream couts;
    std::stringstream cerrs;
    std::stringstream clogs;
    std::stringstream sfmls;

    absl_sink absls;

    std::streambuf* coutb;
    std::streambuf* cerrb;
    std::streambuf* clogb;
    std::streambuf* sfmlb;

public:
    log_pool();
    ~log_pool();

    void open(gsl::czstring path)
    {
        logfs.open(path, std::ios::binary);
        flush();
    }

    [[nodiscard]] auto acquire_scoped()
    {
        calls++;
        return pool.acquire_scoped();
    }

    void flush();
};

log_pool::log_pool()
{
    pool.acquire_scoped();

    coutb = std::cout.rdbuf(couts.rdbuf());
    cerrb = std::cerr.rdbuf(cerrs.rdbuf());
    clogb = std::clog.rdbuf(clogs.rdbuf());
    sfmlb = sf::err().rdbuf(sfmls.rdbuf());

    // Disable std::cerr sink
    absl::SetStderrThreshold(absl::LogSeverityAtLeast::kInfinity);
    absl::InitializeLog();

    absl::AddLogSink(&absls);
}

log_pool::~log_pool()
{
    flush();

    absl::RemoveLogSink(&absls);

    sf::err().rdbuf(sfmlb);
    std::clog.rdbuf(clogb);
    std::cerr.rdbuf(cerrb);
    std::cout.rdbuf(coutb);
}

void log_pool::flush()
{
    xr_string line;

    while (std::getline(couts, line))
        Log("cout: " + line);

    while (std::getline(cerrs, line))
        Log("! cerr: " + line);

    while (std::getline(clogs, line))
        Log("! clog: " + line);

    while (std::getline(sfmls, line))
        Log("! SFML: " + line);

    if (calls.load(std::memory_order::relaxed) == 0)
        return;

    msg_vec msgs;

    pool.for_each_available([&msgs](auto&& vec) {
        std::ranges::move(vec, std::back_inserter(msgs));
        vec.clear();
    });

    std::ranges::stable_sort(msgs, [](const auto& a, const auto& b) { return std::string_view{a.second.data(), a.first} < std::string_view{b.second.data(), b.first}; });
    const auto open = logfs.is_open();

    for (auto&& msg : msgs)
    {
        // Visual Studio
        if (IsDebuggerPresent())
            OutputDebugString(msg.second.c_str() + msg.first);

        // Log file
        if (open)
            logfs.write(msg.second.data(), std::ssize(msg.second));

        // Console history
        const auto count = msg.second.size() - msg.first - 1;
        const auto& back = LogFile.emplace_back(std::move(msg.second), msg.first, count);

        static xr_string last_str;
        static gsl::index last_cnt;

        if (last_str == back)
        {
            *(LogFile.end() - 2) = std::move(*(LogFile.end() - 1));
            LogFile.pop_back();

            LogFile.back() += " [" + std::to_string(++last_cnt) + ']';
        }
        else
        {
            last_str = back;
            last_cnt = 1;
        }
    }

    if (open)
        logfs.flush();

    calls.store(0, std::memory_order::relaxed);
}

std::optional<xr::log_pool> log;
} // namespace

namespace detail
{
void log_init()
{
    logFName[0] = '\0';

    xr::log.emplace();
}
} // namespace detail

void log_flush() { xr::log->flush(); }

namespace
{
void add_one(xr::log_pool::msg_vec& vec, xr_string&& split)
{
    const auto gen_pfx = [&split] {
        using namespace std::chrono;

        string64 buf, curTime;
        const auto now = system_clock::now();
        const auto time = system_clock::to_time_t(now);
        const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) - duration_cast<seconds>(now.time_since_epoch());
        const auto lt = xr::localtime(time);

        std::strftime(buf, sizeof(buf), "%d.%m.%y %H:%M:%S", &lt);
        sprintf_s(curTime, "[%s.%03lld]", buf, ms.count());
        const auto slen = xr_strlen(curTime);

        std::array<char, 16> tid;
        gsl::index tlen;

        if (CPU::ID.on_cpu())
            tlen = xr_sprintf(tid.data(), tid.size(), "[%s%02zuP%1zu]",
                              CPU::ID.threads[tmc::current_thread_index()].group.cpu_kind == tmc::topology::cpu_kind::PERFORMANCE ? "PE" : "EF", tmc::current_thread_index(),
                              tmc::current_priority());
        else if (CPU::ID.on_st())
            tlen = xr_sprintf(tid.data(), tid.size(), "[ST00P%1zu]", tmc::current_priority());
        else
            tlen = xr_sprintf(tid.data(), tid.size(), "[EX%s]", std::format("{0}", std::this_thread::get_id()).c_str());

        split = std::format("{0} {1} {2}\n", std::string_view{curTime, gsl::narrow_cast<size_t>(slen)}, std::string_view{tid.data(), gsl::narrow_cast<size_t>(tlen)}, split);

        return gsl::narrow_cast<size_t>(slen + 1 + tlen + 1);
    };

    vec.emplace_back(gen_pfx(), std::move(split));
}
} // namespace
} // namespace xr

void Log(const xr_string& str)
{
    if (str.empty())
        return;

    constexpr std::array<char, 5> color_codes{'-', '~', '!', '*', '#'}; // Зелёный, Жёлтый, Красный, Серый, Бирюзовый
    const char color_s = str.front();
    const bool have_color = std::find(color_codes.begin(), color_codes.end(), color_s) != color_codes.end(); // Ищем в начале строки цветовой код

    // Разбиваем текст по "\n"
    xr_vector<xr_string> substrs;
    size_t beg{};

    for (size_t end{}; (end = str.find("\n", end)) != xr_string::npos; ++end)
    {
        substrs.emplace_back(str.substr(beg, end - beg));
        beg = end + 1;
    }

    substrs.emplace_back(str.substr(beg));

    const auto obj = xr::log->acquire_scoped();
    bool not_first_line{};

    for (auto&& str : substrs)
    {
        if (not_first_line && have_color)
        {
            str = ' ' + str;
            str = color_s + str; // Если надо, перед каждой строкой вставляем спец-символ цвета, чтобы в консоли цветными были все строки текста, а не только первая.
        }

        xr::add_one(obj.value, std::move(str));
        not_first_line = true;
    }
}

void Log(const char* s) { Log(xr_string{s}); }

void Msg(const char* format, ...)
{
    std::va_list args, args_copy;

    va_start(args, format);
    va_copy(args_copy, args);

    const auto sz = std::vsnprintf(nullptr, 0, format, args);
    if (sz <= 0)
        return;

    const auto n = gsl::narrow_cast<size_t>(sz);
    xr_string out(n, '\0');
    std::vsnprintf(out.data(), n + 1, format, args_copy);

    va_end(args_copy);
    va_end(args);

    Log(out);
}

void Log(const char* msg, const Fvector& dop) { Msg("%s (%f,%f,%f)", msg, dop.x, dop.y, dop.z); }

void Log(const char* msg, const Fmatrix& dop)
{
    Msg("%s:\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f\n%f,%f,%f,%f", msg, dop.vm[0].x, dop.vm[0].y, dop.vm[0].z, dop.vm[0].w, dop.vm[1].x, dop.vm[1].y, dop.vm[1].z, dop.vm[1].w,
        dop.vm[2].x, dop.vm[2].y, dop.vm[2].z, dop.vm[2].w, dop.vm[3].x, dop.vm[3].y, dop.vm[3].z, dop.vm[3].w);
}

void CreateLog(BOOL nl)
{
    if (nl)
        return;

    if (std::strstr(Core.Params, "-no_unique_logs") == nullptr)
    {
        using namespace std::chrono;

        string32 TimeBuf;
        const auto now = system_clock::now();
        const auto time = system_clock::to_time_t(now);
        const auto lt = xr::localtime(time);

        std::strftime(TimeBuf, sizeof(TimeBuf), "%d-%m-%y_%H-%M-%S", &lt);
        xr_strconcat(logFName, Core.ApplicationName, "_", Core.UserName, "_", TimeBuf, ".log");
    }
    else
    {
        xr_strconcat(logFName, Core.ApplicationName, "_", Core.UserName, ".log");
    }

    std::ignore = FS.update_path(logFName, "$logs$", logFName);
    VerifyPath(logFName);

    xr::log->open(logFName);
}
