#ifndef __KSTODON_HPP__
#define __KSTODON_HPP__

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
virtual bool    HasAuth() = 0;
virtual Account GetAccount() = 0;
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
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░ HelperFns ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/
inline bool SaveStatusID(uint64_t status_id, std::string username) {
  using namespace nlohmann;
  json database_json;
  json loaded_json = LoadJSONFile(get_executable_cwd() + "../" + constants::DB_JSON_PATH);

  if (loaded_json.is_null())
    database_json = {"status", {status_id}};
  else
  {
    database_json = loaded_json;
    json user_status_array_json = database_json["status"][username];

    if (!user_status_array_json.empty()) {
      for (const auto& id : user_status_array_json.get<const std::vector<uint64_t>>())
        if (id == status_id) return false; // Already exists
    }

    database_json["status"][username].emplace_back(status_id);
  }

  SaveToFile(database_json, constants::DB_JSON_PATH);

  return true;
}

inline std::vector<uint64_t> GetSavedStatusIDs(std::string username) {
  using json = nlohmann::json;

  json db_json = LoadJSONFile(get_executable_cwd() + "../" + constants::DB_JSON_PATH);

  if (!db_json.is_null()                   &&
      db_json.contains("status")           &&
      db_json["status"].contains(username) &&
      !db_json["status"][username].is_null()) {
    return db_json["status"][username].get<std::vector<uint64_t>>();
  }

  return std::vector<uint64_t>{};
}

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
Client();

virtual ~Client() override {}

virtual bool                HasAuth() override;
virtual Status              FetchStatus(StatusID id) override;
virtual std::vector<Status> FetchUserStatuses(UserID id) override;
virtual bool                PostStatus(Status status) override;
virtual bool                PostStatus(Status status, std::vector<File> media) override;
virtual Media               PostMedia(File file) override;
virtual Account             GetAccount() override;
        std::string         GetUsername();

std::vector<Conversation>         FetchRepliesToStatus(uint64_t id);

private:
using json = nlohmann::json;
Authenticator m_authenticator;
};

} // namespace kstodon

#endif // __KSTODON_HPP__
