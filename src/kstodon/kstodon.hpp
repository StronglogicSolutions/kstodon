#ifndef __KSTODON_HPP__
#define __KSTODON_HPP__

#include <iostream>
#include <ctime>
#include <cpr/cpr.h>
#include <string>

#include <nlohmann/json.hpp>

#include "common/auth/auth.hpp"

namespace kstodon {

/**
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ Interfaces ░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class SecureClient {
public:
virtual ~SecureClient() {}
virtual bool HasAuth() = 0;
};

class MastodonStatusClient {
public:
using UserID   = std::string;
using StatusID = uint64_t;

virtual ~MastodonStatusClient() {}
virtual Status              FetchStatus(StatusID id) = 0;
virtual std::vector<Status> FetchUserStatuses(UserID id) = 0;
virtual bool                PostStatus(Status status) = 0;
};

class MastodonMediaClient {
public:
virtual ~MastodonMediaClient() {}
virtual Media PostMedia(File file) = 0;
virtual bool  PostStatus(Status status, std::vector<File> media) = 0;
};

/**
  @class
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ KLytics ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class Client : public SecureClient,
               public MastodonStatusClient,
               public MastodonMediaClient {
public:
Client();

virtual ~Client() override {}

virtual bool                HasAuth() override;
virtual Status              FetchStatus(StatusID id) override;
virtual std::vector<Status> FetchUserStatuses(UserID id) override;
virtual bool                PostStatus(Status status) override;
virtual bool                PostStatus(Status status, std::vector<File> media) override;
virtual Media               PostMedia(File file) override;

private:
using json = nlohmann::json;
Authenticator m_authenticator;
};

} // namespace kstodon

#endif // __KSTODON_HPP__
