#include <bashpp/env.hpp>
#include <gtest/gtest.h>

TEST(LibbashppEnv, TheEnvInheritsGlobalEnv) {
    ASSERT_EQ(setenv("HELLO", "WORLD", 1), 0);

    bashpp::Env env;
    EXPECT_TRUE(env.contains("HELLO"));
    EXPECT_STREQ(env.get("HELLO").data(), "WORLD");

    ASSERT_EQ(unsetenv("HELLO"), 0);
}

TEST(LibbashppEnv, AnEnvironmentVariableCanBeSet) {
    bashpp::Env env;
    ASSERT_FALSE(env.contains("HELLO"));

    env.set("HELLO", "WORLD");

    EXPECT_TRUE(env.contains("HELLO"));
    EXPECT_STREQ(env.get("HELLO").data(), "WORLD");
}

TEST(LibbashppEnv, ItCanBeCleared) {
    bashpp::Env env;
    env.set("HELLO", "WORLD");
    ASSERT_TRUE(env.contains("HELLO"));

    env.clear();
    EXPECT_TRUE(env.entries().empty());
}
