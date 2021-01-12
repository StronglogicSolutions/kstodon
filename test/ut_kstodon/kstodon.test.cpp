#include "kstodon.test.hpp"


TEST(KStodonTests, Instantiate) {
  ASSERT_NO_THROW(kstodon::Client{});
}

TEST(KStodonTests, DISABLED_FetchAuthToken) {
  using namespace kstodon;

  Authenticator authenticator{};

  bool fetch_success = authenticator.FetchToken();

  EXPECT_TRUE(fetch_success);
}

TEST(KStodonTests, VerifyAuth) {
  using namespace kstodon;

  Authenticator authenticator{};

  bool    has_valid_token = authenticator.HasValidToken();
  bool    verified_token  = authenticator.VerifyToken();
  Account account         = authenticator.GetAccount();

  std::cout << account << std::endl;

  EXPECT_TRUE(has_valid_token);
  EXPECT_TRUE(verified_token);
  EXPECT_EQ(account.username, "logicp");
}
