#include "kstodon.hpp"

namespace kstodon {
Client::Client()
: m_authenticator(Authenticator{}) {
  if (!m_authenticator.HasValidToken()) {
    m_authenticator.FetchToken();
  }

  if (!m_authenticator.HasValidToken())
    throw std::invalid_argument{"Client was unable to authenticate."};
}

bool Client::HasAuth() {
  return m_authenticator.IsAuthenticated();
}


Status Client::FetchStatus(uint64_t id) {
  using json = nlohmann::json;
  using namespace constants::MastodonOnline;

  const std::string STATUSES_URL = BASE_URL + PATH.at(STATUSES_INDEX) + "/" + std::to_string(id);

  cpr::Response r = cpr::Get(
    cpr::Url{STATUSES_URL}
  );

  if (!r.text.empty()) {
    return JSONToStatus(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
  }

  return Status{};
}



} // namespace kstodon

