#include <bashpp/env.hpp>
#include <gtest/gtest.h>

TEST(LibbashppEnv, TheEnvInheritsGlobalEnv) {
    ASSERT_EQ(setenv("HELLO", "WORLD", 1), 0);

    bashpp::Env env;
    EXPECT_TRUE(env.contains("HELLO"));
    EXPECT_STREQ(env.get("HELLO").data(), "WORLD");
}

TEST(LibbashppEnv, AnEnvironmentVariableCanBeSet) {
    bashpp::Env env;
    env.set("HELLO", "WORLD");

    EXPECT_TRUE(env.contains("HELLO"));
    EXPECT_STREQ(env.get("HELLO").data(), "WORLD");
}
