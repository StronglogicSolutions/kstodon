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

inline bool SaveStatusID(uint64_t status_id, std::string username) {
  using namespace nlohmann;
  json database_json;
  json loaded_json = LoadJSONFile(constants::DB_JSON_PATH);

  if (loaded_json.is_null())
    database_json = {"status", {status_id}};
  else
  {
  json user_status_array_json = database_json["status"][username];
    if (!user_status_array_json.empty()) {
      for (const auto& id : user_status_array_json.get<const std::vector<uint64_t>>())
        if (id == status_id) return false; // Already exists
    }
    database_json["status"][username].emplace_back(status_id);
  }

  SaveToFile(database_json.dump(), constants::DB_JSON_PATH);

  return true;
}

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
