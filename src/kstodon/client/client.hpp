#pragma once

#include <deque>

#include "kstodon/common/auth/auth.hpp"
#include "kstodon/common/interface.hpp"

namespace kstodon {
/**
  @class
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ KStodon ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class Client : public SecureClient,
               public MastodonStatusClient,
               public MastodonMediaClient {
public:
Client(const std::string& username = "");

virtual ~Client() override {}

virtual bool                HasAuth() override;
virtual Status              FetchStatus(StatusID id) override;
virtual std::vector<Status> FetchUserStatuses(UserID id) override;
 std::vector<Status> FetchChildStatuses(StatusID id);
virtual bool                PostStatus(Status status) override;
virtual bool                PostStatus(Status status, std::vector<File> media) override;
        bool                PostStatus(Status status, std::vector<std::string> media);
virtual Media               PostMedia(File file) override;
virtual Account             GetAccount() override;
        std::string         GetUsername();
        bool                SetUser(const std::string& username);

        bool                HasPostedStatuses() const;
        Status              GetPostedStatus();

std::vector<Conversation>   FetchConversations();

private:
using json = nlohmann::json;
Authenticator      m_authenticator;
std::deque<Status> m_statuses;
};

} // namespace kstodon
