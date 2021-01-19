#include "kstodon.hpp"

namespace kstodon {
using namespace constants::MastodonOnline;

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

  const std::string STATUSES_URL = BASE_URL + PATH.at(STATUSES_INDEX) + "/" + std::to_string(id);

  cpr::Response r = cpr::Get(
    cpr::Url{STATUSES_URL}
  );

  if (!r.text.empty()) {
    return JSONToStatus(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
  }

  return Status{};
}

std::vector<Status> Client::FetchUserStatuses(UserID id) {
  using json = nlohmann::json;
  // api/v1/accounts/:id/statuses
  const std::string URL = BASE_URL + PATH.at(ACCOUNTS_INDEX) + '/' + id + "/statuses";

  cpr::Response r = cpr::Get(
    cpr::Url{URL}
  );

  if (!r.text.empty()) {
    return JSONToStatuses(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
  }

  return std::vector<Status>{};

}

bool Client::PostStatus(Status status) {
  using namespace constants;
  using json = nlohmann::json;

  const std::string URL = BASE_URL + PATH.at(STATUSES_INDEX);

  cpr::Response r = cpr::Post(
    cpr::Url{URL},
    cpr::Header{
      {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
    },
    cpr::Body{status.postdata()}
  );

  Status returned_status = JSONToStatus(json::parse(r.text, nullptr, JSON_PARSE_NO_THROW));

  if (!returned_status.content.empty()) {
    SaveStatusID(returned_status.id, m_authenticator.GetUsername());
  }
  return (r.status_code < 400);
}

Media Client::PostMedia(File file) {
  using namespace constants;

  const std::string URL{BASE_URL + PATH.at(MEDIA_INDEX)};

  cpr::Response r = cpr::Post(
    cpr::Url{URL},
    cpr::Header{
      {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
    },
    file.multiformdata()
  );

  std::cout << r.text << std::endl;

  if (r.status_code < 400) {
    return ParseMediaFromJSON(json::parse(r.text, nullptr, constants::JSON_PARSE_NO_THROW));
  }

  return Media{};
}

bool Client::PostStatus(Status status, std::vector<File> files) {
  for (const auto& file : files) {
    Media media = PostMedia(file);
    status.media.emplace_back(std::move(media));
  }
  return PostStatus(status);
}
} // namespace kstodon
