#include "kstodon.test.hpp"


TEST(KStodonTests, DISABLED_Instantiate) {
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
  EXPECT_EQ(account.id, "154950");
}

TEST(KStodonTests, DISABLED_ClientHasAuth) {
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

  std::cout << status << std::endl;

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

TEST(KStodonTests, DISABLED_PostMedia) {
  using namespace kstodon;

  Client client{};
  File   file{TEST_PATH};

  file.description = "This is a test file!";

  bool   path_is_valid     = file.GetBytes().size() > 0;
  bool   is_media_client   = (dynamic_cast<MastodonMediaClient*>(&client) != nullptr);
  Media  media             = client.PostMedia(file);
  bool   media_post_result = !media.id.empty();
  Status status{};

  status.content = "This is a test!";
  status.media.emplace_back(std::move(media));

  bool status_post_result = client.PostStatus(status);

  EXPECT_TRUE(is_media_client);
  EXPECT_TRUE(media_post_result);
  EXPECT_TRUE(status_post_result);
}

TEST(KStodonTests, DISABLED_PostStatusWithFile) {
  using namespace kstodon;

  Status status{};
  status.replying_to_id = "105586303075566178";
  status.visibility = constants::StatusVisibility::DIRECT;

  status.content = "not this again!";

  bool result = Client{}.PostStatus(
    status, std::vector<File>{
      File{TEST_PATH}
    }
  );

  EXPECT_TRUE(result);
}


TEST(KStodonTests, DISABLED_FindAndReply) {
  using namespace kstodon;

  Bot bot{};

  std::vector<Conversation> replies = bot.FindReplies();

  for (const auto& reply : replies) {
    if (reply.status.is_valid())
      bot.ReplyToStatus(reply.status, "I don't suck, you suck!");
  }

  EXPECT_FALSE(replies.empty());
}

TEST(KStodonTests, DISABLED_GetChildren) {
  using namespace kstodon;

  Client client{};

  auto statuses = client.FetchChildStatuses(105652931846646786);

  for (const auto& status : statuses) {
    log(status);
  }

  EXPECT_FALSE(statuses.empty());
}

// TEST(CPRTest, FetchAndSaveFile)
// {
//   const std::string url{"https://instagram.fymy1-2.fna.fbcdn.net/v/t51.2885-15/e15/152779540_420270432612013_5563537731454901171_n.jpg?_nc_ht=instagram.fymy1-2.fna.fbcdn.net&_nc_cat=101&_nc_ohc=z188AsgLchcAX9D32AK&tp=1&oh=0ab6260752f52c2e8d2a5806fd34e958&oe=605E07B3&ig_cache_key=MjUxNTkzODcyNTM0ODAwMzE4Mg%3D%3D.2"};

//   std::string filename = kstodon::FetchTemporaryFile(url);
//   // std::string file     = kstodon::ReadFromFile(filename);

//   EXPECT_EQ(filename, "temp_file.jpg");
//   EXPECT_FALSE(file.empty());

// }

TEST(KStodonTest, FetchTempAndPostStatus)
{
  const std::string url1{"https://files.mastodon.online/site_uploads/files/000/000/001/original/rio2.png"};

  const std::string url2{"https://files.mastodon.online/media_attachments/files/105/688/686/326/414/629/original/03f9acea7be02b29.jpeg"};

  // std::string file     = kstodon::ReadFromFile(filename);

  kstodon::Bot bot{};

  kstodon::Status status{"How do these two go together?"};
  status.visibility = kstodon::constants::StatusVisibility::PRIVATE;

  auto result = bot.PostStatus(status, std::vector<std::string>{url1, url2});


  EXPECT_TRUE(result);
}