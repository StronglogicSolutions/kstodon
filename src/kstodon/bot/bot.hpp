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
    if (
      std::find(
        status_ids.cbegin(),
        status_ids.cend(),
        string_to_uint64(conversation.status.replying_to_id)) != status_ids.cend())
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

std::string PlatformFromURL(const std::string& url) {
  const std::string::size_type begin_length{8};
  auto begin = url.find_first_of("https://");
  std::string return_s{};
  if (begin != std::string::npos) {
  auto end = url.find_last_of('@');
    if (end != std::string::npos) {
      return_s = url.substr(begin + begin_length, (end - (begin + begin_length - 1)));
    }
  }
  return return_s;
}

/**
 * MakeMention
 *
 * TODO: Create full HTML mention:
 * <span class=\"h-card\">
 *   <a href=\"https://mastodon.online/@username\" class=\"u-url mention\">
 *     @<span>username</span>
 *   </a>
 * </span>
 *
 * @param status
 * @return std::string
 */
std::string MakeMention(const Status& status) {
  if (!status.account.username.empty() && !status.account.url.empty()) {
    return '@' + status.account.username + '@' + PlatformFromURL(status.account.url);
  }
  return "";
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
