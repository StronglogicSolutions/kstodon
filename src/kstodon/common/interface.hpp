#pragma once

#include "types.hpp"

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

