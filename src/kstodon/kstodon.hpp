#ifndef __KSTODON_HPP__
#define __KSTODON_HPP__

#include <iostream>
#include <ctime>
#include <cpr/cpr.h>
#include <string>

#include <nlohmann/json.hpp>

#include "common/auth/auth.hpp"

namespace kstodon {

struct Status{
uint32_t id;
};

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
virtual ~MastodonStatusClient() {}
virtual Status FetchStatus(uint32_t id) = 0;
};

/**
  @class
  ┌───────────────────────────────────────────────────────────┐
  │░░░░░░░░░░░░░░░░░░░░░░░░░░░ KLytics ░░░░░░░░░░░░░░░░░░░░░░░│
  └───────────────────────────────────────────────────────────┘
*/

class Client : public SecureClient,
               public MastodonStatusClient {
public:
Client();

virtual ~Client() override {}

virtual bool HasAuth() override;
virtual Status FetchStatus(uint32_t id) override;

private:
Authenticator m_authenticator;
};

} // namespace kstodon

#endif // __KSTODON_HPP__
