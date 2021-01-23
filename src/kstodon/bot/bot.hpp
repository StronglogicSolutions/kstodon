#ifndef __BOT_HPP__
#define __BOT_HPP__

#include "kstodon/kstodon.hpp"

class ConversationTracker {
public:
virtual ~ConversationTracker() {}
virtual std::vector<Conversation> FindReplies() = 0;
};

inline std::vector<Conversation> ParseRepliesFromConversations(std::vector<Conversation> conversations, std::vector<uint64_t> status_ids) {
  std::vector<Conversation> replies{};

  for (auto&& conversation : conversations) {
    auto status = conversation.statuses.front();

    if (std::find(status_ids.cbegin(), status_ids.cend(), string_to_uint64(status.replying_to_id)) != status_ids.cend())
      replies.emplace_back(std::move(conversation));
  }

  return replies;
}

namespace kstodon {
class Bot : public ConversationTracker {

public:
Bot()
: m_client(Client{}) {
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

bool ReplyToStatus(Status status) {
  Status placeholder_response{};
  placeholder_response.replying_to_id = std::to_string(status.id);
  placeholder_response.content = status.account."This is the response. Take it or leave it.";
  placeholder_response.visibility = status.visibility;
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
