#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "kstodon/client/client.hpp"

namespace kstodon {
class Bot : public ConversationTracker {

public:
Bot(const std::string& username = "")
: m_client(Client{
  username
  })
{
  if (!m_client.HasAuth()) {
    throw std::invalid_argument{"Client was not able to authenticate"};
  }
}

std::vector<Conversation> FindReplies() override {
  std::vector<uint64_t> status_ids = GetSavedStatusIDs(m_client.GetUsername());
  std::vector<Conversation> conversations;
  std::vector<Conversation> replies{};

  try {
    conversations = m_client.FetchConversations();
  }
  catch (const request_error& e) {
    log(e.what());
  }

  replies = ParseRepliesFromConversations(conversations, status_ids);

  return replies;
}

bool PostStatus(Status status, std::vector<File> files) {
  return m_client.PostStatus(status, files);
}

bool ReplyToStatus(Status status, std::string message = "This is the response. Take it or leave it.") {
  Status placeholder_response{};
  placeholder_response.replying_to_id = std::to_string(status.id);
  placeholder_response.content        = MakeMention(status) + message;
  placeholder_response.visibility     = status.visibility;
  if (m_client.PostStatus(placeholder_response)) {
    return RemoveStatusID(m_client.GetUsername(), string_to_uint64(status.replying_to_id));
  }

  return false;
}

private:
Client m_client;
};

} // namespace kstodon

#endif // __BOT_HPP__
