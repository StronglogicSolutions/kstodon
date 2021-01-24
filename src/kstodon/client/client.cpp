#include "client.hpp"

namespace kstodon {
using namespace constants::MastodonOnline;

Client::Client(const std::string& username)
: m_authenticator(Authenticator{username}) {
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

  if (r.status_code >= 400) {
    // Error handling
    std::string error_message{
      "Request failed with message: " + r.error.message
    };
  }

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

/**
 * @brief Chunk Message
 *
 * @param   [in]  {std::string} message
 * @returns [out] {std::vector<std::string>}
 */
std::vector<std::string> ChunkMessage(const std::string& message) {
  using namespace constants;

  const uint32_t MAX_CHUNK_SIZE = MASTODON_CHAR_LIMIT - 5;

  std::vector<std::string>     chunks{};
  const std::string::size_type message_size = message.size();
  const std::string::size_type num_of_chunks{message.size() / MAX_CHUNK_SIZE + 1};
  uint32_t                     chunk_index{1};
  std::string::size_type       bytes_chunked{0};

  chunks.reserve(num_of_chunks);

  while (bytes_chunked < message_size) {
    const std::string::size_type size_to_chunk =
      (bytes_chunked + MAX_CHUNK_SIZE > message_size) ?
      (message_size - bytes_chunked) :
      MAX_CHUNK_SIZE;

    std::string oversized_chunk = message.substr(bytes_chunked, size_to_chunk);

    const std::string::size_type ws_idx = oversized_chunk.find_last_of(" ");
    const std::string::size_type pd_idx = oversized_chunk.find_last_of(".");
    const std::string::size_type index  = (ws_idx > pd_idx) ? ws_idx : pd_idx;

    chunks.emplace_back(
      std::string{
        oversized_chunk.substr(0, index) +
        std::to_string(chunk_index++) + '/' + std::to_string(num_of_chunks) // i/n
      }
    );

    bytes_chunked += index;
  }

  return chunks;
}

/**
 * PostStatus
 *
 * TODO: considering returning Status object (updated)
 *
 * @param   [in]  {Status} status
 * @returns [out] {bool}
 */
bool Client::PostStatus(Status status) {
  using namespace constants;
  using json = nlohmann::json;

  const std::string URL = BASE_URL + PATH.at(STATUSES_INDEX);
  const std::vector<std::string> messages = ChunkMessage(status.content);
  std::string reply_to_id = status.replying_to_id;

  for (const auto& message : messages) {
    Status outgoing_status = Status::create_instance_with_message(status, message, reply_to_id);

    RequestResponse response{cpr::Post(
      cpr::Url{URL},
      cpr::Header{
        {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
      },
      cpr::Body{outgoing_status.postdata()}
    )};

    if (response.error)
      throw request_error(response.GetError());

    Status returned_status = JSONToStatus(response.json());

    if (returned_status.content.empty())
      return false;

    SaveStatusID(returned_status.id, m_authenticator.GetUsername());
    reply_to_id = std::to_string(returned_status.id);
  }

  return true;
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

std::vector<Conversation> Client::FetchConversations() {
  using namespace constants;

  const std::string CONVERSATION_URL = BASE_URL + PATH.at(CONVERSATION_INDEX);

  RequestResponse response{cpr::Get(
    cpr::Url{CONVERSATION_URL},
    cpr::Header{
      {HEADER_NAMES.at(HEADER_AUTH_INDEX), m_authenticator.GetBearerAuth()}
    }
  )};

  if (response.error) {
    throw request_error{response.GetError()};
  }

  return JSONToConversation(response.json());
}

Account Client::GetAccount() {
  return m_authenticator.GetAccount();
}

std::string Client::GetUsername() {
  return m_authenticator.GetUsername();
}

} // namespace kstodon
