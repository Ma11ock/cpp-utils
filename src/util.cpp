#include "util.hpp"

#ifndef WIN32
extern "C" {
#    include <unistd.h>
}
#endif  // WIN32

#include <cerrno>
#include <cstring>

namespace fs = std::filesystem;
using path = fs::path;

static std::string errorString = "";

static void setLastError() {
#ifdef WIN32
    auto errorId = GetLastError();
    if (errId == 0) {
        goto get_error_unix;
    }

    LPSTR messageBuffer = nullptr;

    size_t size = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        static_cast<LPSTR>(&messageBuffer), 0, nullptr);

    errorString = std::string(messageBuffer, size);

    LocalFree(messageBuffer);

get_error_unix:
#endif  // WIN32

    errorString = std::strerror(errno);
}

path util::mkTmp(std::string_view prefix, std::string_view suffix) {
    // Try to make a random filepath 10 times. If it fails, throw.
    for (int i = 0; i < 10; i++) {
        auto maybeResult = (fs::temp_directory_path() / path(prefix) / path(randomAlNumString(8)))
                               .replace_extension(path(suffix));
        if (!fs::exists(maybeResult)) {
            return maybeResult;
        }
    }
    throw std::runtime_error("Could not create a temporary file...");
}

std::string util::randomAlNumString(std::size_t len) {
    static const std::array alphaNum = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F',
        'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V',
        'W', 'X', 'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
        'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
    static auto stringRandomDevice = seededMT();
    static std::uniform_int_distribution<std::size_t> dist(0, alphaNum.size());

    std::string result;
    result.resize(len);
    for (std::size_t i = 0; i < len; i++) {
        result[i] = alphaNum[dist(stringRandomDevice)];
    }

    return result;
}

int util::exec(const std::filesystem::path &path, const std::vector<std::string> &args) {
    char **argv = new char *[args.size() + 1];
    for (std::size_t i = 0; i < args.size(); i++) {
        argv[i] = const_cast<char *>(args[i].c_str());
    }
    argv[args.size()] = nullptr;
#ifdef WIN32
    if (auto i = _execv(path.c_str(), argv); i < 0) {
        setLastError();
        return i;
    }
#else   // Unix
    if (auto i = execv(path.c_str(), argv); i < 0) {
        setLastError();
        return i;
    }
#endif  // WIN32

    return 0;
}

int util::forkAndExec(const std::filesystem::path &path, const std::vector<std::string> &args) {
#ifdef WIN32
    STARTUPINFO info = {sizeof(info)};
    PROCESS_INFORMATION processInfo;
    std::string argStr;
    for (const auto &arg : args) {
        argStr += arg;
    }
    if (!CreateProcess(path.c_str(), argStr.c_str(), nullptr, nullptr, FALSE, 0, nullptr, nullptr,
                       &info, &processInfo)) {
        setLastError();
        return -1;
    }
#else   // Unix
    auto pid = fork();
    if (pid == 0) {
        // Child.
        return util::exec(path, args);
    } else if (pid < 0) {
        // Error.
        setLastError();
        return static_cast<int>(pid);
    }
    // Parent.
#endif  // WIN32
    return 0;
}

std::string util::getErrorString() {
    return errorString;
}
