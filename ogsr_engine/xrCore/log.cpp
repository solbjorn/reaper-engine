#include "stdafx.h"

#include "log.h"

#include "cpu.h"

#include <SFML/System/Err.hpp>

#include <absl/log/globals.h>
#include <absl/log/initialize.h>
#include <absl/log/log_sink_registry.h>

#include <bitmap_object_pool.hpp>

#include <fstream>

xr_vector<xr_string> LogFile;
static string_path logFName;

namespace xr
{
namespace
{
class log_pool final
{
public:
    using msg_vec = xr_vector<xr_string>;

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
        logfs.open(std::filesystem::path{reinterpret_cast<xr::cu8zstring>(path)}, std::ios::binary);
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
    Debug.to_log(nullptr);
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
        msgs.append_range(std::views::as_rvalue(vec));
        vec.clear();
    });

    std::ranges::stable_sort(msgs, {}, [] [[nodiscard]] (const auto& msg) { return msg.subview(0, xr::detail::log_pfx_len); });
    const auto open = logfs.is_open();

    for (auto&& msg : msgs)
    {
        // Visual Studio
        if (xr::is_debugger_present())
            ::OutputDebugStringW(sf::String::fromUtf8(msg.begin(), msg.end()).toWideString().c_str() + xr::detail::log_pfx_len);

        // Log file
        if (open)
            logfs.write(msg.c_str(), std::ssize(msg));

        // Console history
        const auto count = msg.size() - xr::detail::log_pfx_len - 1;
        const auto& back = LogFile.emplace_back(std::move(msg), xr::detail::log_pfx_len, count);

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
    const auto orig = split.size();

    split.resize_and_overwrite(xr::detail::log_pfx_len + orig + 1, [orig] [[nodiscard]] (gsl::zstring p, std::size_t size) noexcept {
        const auto now = std::chrono::system_clock::now();
        const auto lt = xr::localtime(std::chrono::system_clock::to_time_t(now));

        std::memmove(p + xr::detail::log_pfx_len, p, orig);
        xr::format_to(p, "[{:02}.{:02}.{:02} {:02}:{:02}:{:02}.{:03}] [{}] ", lt.tm_mday, lt.tm_mon + 1, lt.tm_year % 100, lt.tm_hour, lt.tm_min, lt.tm_sec,
                      std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000, CPU::ID.this_thread());
        p[size - 1] = '\n';

        return size;
    });

    vec.emplace_back(std::move(split));
}
} // namespace
} // namespace xr

void Log(std::string_view str)
{
    if (str.empty())
        return;

    constexpr std::array<char, 5> color_codes{'-', '~', '!', '*', '#'}; // Зелёный, Жёлтый, Красный, Серый, Бирюзовый
    const char color_s = str.front();
    const bool have_color = std::find(color_codes.begin(), color_codes.end(), color_s) != color_codes.end(); // Ищем в начале строки цветовой код

    // Разбиваем текст по "\n"
    xr_vector<xr_string> substrs;
    size_t beg{};

    for (size_t end{}; (end = str.find("\n", end)) != std::string_view::npos; ++end)
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
            // Если надо, перед каждой строкой вставляем спец-символ цвета, чтобы в консоли цветными были все строки текста, а не только первая.
            str = ' ' + str;
            str = color_s + str;
        }

        xr::add_one(obj.value, std::move(str));
        not_first_line = true;
    }
}

void Log(xr::detail::string_view fmt, xr::detail::format_args args) { Log(xr::detail::vformat(fmt, args)); }

void Log(const char* msg, const Fvector& dop) { Msg("{} ({},{},{})", msg, dop.x, dop.y, dop.z); }

void Log(const char* msg, const Fmatrix& dop)
{
    Msg("{}:\n{},{},{},{}\n{},{},{},{}\n{},{},{},{}\n{},{},{},{}", msg, dop.vm[0].x, dop.vm[0].y, dop.vm[0].z, dop.vm[0].w, dop.vm[1].x, dop.vm[1].y,
        dop.vm[1].z, dop.vm[1].w, dop.vm[2].x, dop.vm[2].y, dop.vm[2].z, dop.vm[2].w, dop.vm[3].x, dop.vm[3].y, dop.vm[3].z, dop.vm[3].w);
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
    Debug.to_log(logFName);
}
