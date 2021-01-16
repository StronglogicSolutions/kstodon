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

TEST(KStodonTests, DISABLED_VerifyAuth) {
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

TEST(KStodonTests, ClientHasAuth) {
  using namespace kstodon;

  Client client{};

  bool client_has_auth = client.HasAuth();

  EXPECT_TRUE(client_has_auth);
}

TEST(KStodonTests, DISABLED_FetchStatus) {
  using namespace kstodon;

  // uint64_t TEST_ID{105528916039043648};
  uint64_t TEST_ID{105551968129592648};

  Client client{};

  Status status = client.FetchStatus(TEST_ID);

  EXPECT_TRUE(status.id == TEST_ID);
}

TEST(KStodonTests, DISABLED_FetchUserStatuses) {
  using namespace kstodon;

  Client::UserID user_id{"154950"};
  Client         client{};

  std::vector<Status> statuses = client.FetchUserStatuses(user_id);

  for (const auto& status : statuses) {
    std::cout << status << std::endl;
  }

  EXPECT_FALSE(statuses.empty());
}

TEST(KStodonTests, DISABLED_PostStatus) {
  using namespace kstodon;

  Client client{};
  Status status{};

  status.content = "Test content from KStodon!";

  bool has_auth = client.HasAuth();

  bool result   = client.PostStatus(status);

  EXPECT_TRUE(result);
}

TEST(KstodonTests, PostMedia) {
  using namespace kstodon;
  const std::string TEST_PATH{get_executable_cwd() + "../test/ut_kstodon/data/test_file.jpg"};

  Client client{};
  File   file{};

  file.path        = TEST_PATH;
  file.description = "This is a test file!";

  bool path_is_valid     = file.GetBytes().size() > 0;
  bool is_media_client   = (dynamic_cast<MastodonMediaClient*>(&client) != nullptr);
  bool media_post_result = client.PostMedia(file);

  EXPECT_TRUE(is_media_client);
  EXPECT_TRUE(media_post_result);
}