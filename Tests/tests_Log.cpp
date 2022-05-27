/**
 * @ Author: Matthieu Moinvaziri
 * @ Description: Log tests
 */

#include <string>
#include <sstream>

#include <gtest/gtest.h>

#include <Kube/Core/Log.hpp>

#include <codecvt>

using namespace kF;

[[nodiscard]] static inline std::wstring BytesToWString(const std::string &s)
{
    return std::wstring(s.begin(), s.end());
}

TEST(Log, Singletons)
{
    constexpr auto Answer = L"Hello world";

    std::wstring output;

    // Info
    testing::internal::CaptureStdout();
    kFInfoRaw(Answer);
    output = BytesToWString(testing::internal::GetCapturedStdout());
    ASSERT_EQ(output, Answer);

    // Error
    testing::internal::CaptureStderr();
    kFErrorRaw(Answer);
    output = BytesToWString(testing::internal::GetCapturedStderr());
    ASSERT_EQ(output, Answer);
}

TEST(Log, Basics)
{
    std::wostringstream oss;
    Core::Log log(oss);
    log.log("The key is ", 42);
    ASSERT_EQ(oss.str(), L"The key is 42\n");
}