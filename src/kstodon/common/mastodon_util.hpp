#pragma once

#include "types.hpp"

inline Account ParseAccountFromJSON(nlohmann::json data) {
  Account account{};
  if (!data.is_null()) {
    account.id              = GetJSONStringValue(data, "id");
    account.username        = GetJSONStringValue(data, "username");
    account.acct            = GetJSONStringValue(data, "acct");
    account.display_name    = GetJSONStringValue(data, "display_name");
    account.locked          = GetJSONBoolValue  (data, "locked");
    account.bot             = GetJSONBoolValue  (data, "bot");
    account.discoverable    = GetJSONBoolValue  (data, "discoverable");
    account.group           = GetJSONBoolValue  (data, "group");
    account.created_at      = GetJSONStringValue(data, "created_at");
    account.note            = GetJSONStringValue(data, "note");
    account.url             = GetJSONStringValue(data, "url");
    account.avatar          = GetJSONStringValue(data, "avatar");
    account.header          = GetJSONStringValue(data, "header");
    account.locked          = GetJSONBoolValue  (data, "locked");
    account.last_status_at  = GetJSONStringValue(data, "last_status_at");
    account.followers_count = GetJSONValue<uint32_t>(data, "followers_count");
    account.following_count = GetJSONValue<uint32_t>(data, "following_count");
    account.statuses_count  = GetJSONValue<uint32_t>(data, "statuses_count");

    nlohmann::json fields = data["source"]["fields"];

    for (const auto& field : fields) {
      account.fields.emplace_back(AccountField{
        .name  = GetJSONStringValue(field, "name"),
        .value = GetJSONStringValue(field, "value")
      });
    }
  }

  return account;
}

inline std::vector<Tag> ParseTagsFromJSON(nlohmann::json data) {
  std::vector<Tag> tags_v{};

  if (!data.is_null()) {
    for (const auto& tag : data) {
      tags_v.emplace_back(Tag{
        .name = tag["name"],
        .url  = tag["url"]
      });
    }
  }

  return tags_v;
}

inline std::vector<Mention> ParseMentionsFromJSON(nlohmann::json data) {
  std::vector<Mention> mentions{};

  if (!data.is_null()) {
    for (const auto& mention : data) {
      mentions.emplace_back(Mention{
        .acct     = mention["acct"],
        .id       = mention["id"],
        .url      = mention["url"],
        .username = mention["username"]
      });
    }
  }

  return mentions;
}

inline Media ParseMediaFromJSON(nlohmann::json data) {
  Media media{};

  if (!data.is_null() && data.is_object()) {
    media.id                 = GetJSONStringValue(data, "id");
    media.type               = GetJSONStringValue(data, "type");
    media.url                = GetJSONStringValue(data, "url");
    media.preview_url        = GetJSONStringValue(data, "preview_url");
    media.remote_url         = GetJSONStringValue(data, "remote_url");
    media.preview_remote_url = GetJSONStringValue(data, "preview_remote_url");
    media.text_url           = GetJSONStringValue(data, "text_url");
    media.description        = GetJSONStringValue(data, "description");
    media.blurhash           = GetJSONStringValue(data, "blurhash");
    media.meta               = MediaMetadata{};

    if (data["meta"].contains("original")) {
      auto original = data["meta"]["original"];

      media.meta.original = MetaDetails{
        .width    = GetJSONValue<uint32_t>(original, "width"),
        .height   = GetJSONValue<uint32_t>(original, "height"),
        .size     = GetJSONStringValue(original, "size"),
        .aspect   = GetJSONValue<float>(original, "aspect")
      };
    }

    if (data["meta"].contains("small")) {
      auto small = data["meta"]["small"];

      media.meta.small = MetaDetails{
        .width    = small["width"],
        .height   = small["height"],
        .size     = small["size"],
        .aspect   = small["aspect"]
      };
    }
  }

  return media;
}

inline std::vector<Media> ParseMediaFromJSONArr(nlohmann::json data) {
  std::vector<Media> media_v{};

  if (!data.is_null() && data.is_array()) {
    for (const auto& item : data) {
      media_v.emplace_back(ParseMediaFromJSON(item));
    }
  }

  return media_v;
}

inline std::vector<Account> ParseAccountsFromJSON(nlohmann::json data) {
  std::vector<Account> accounts{};
  if (!data.is_null() && data.is_array()) {
    for (const auto& item : data) {
      if (!item.is_null() && item.is_object()) {
        accounts.emplace_back(ParseAccountFromJSON(item));
      }
    }
  }
  return accounts;
}

/**
 * @brief
 *
 * @param data
 * @return Status
 */
inline Status JSONToStatus(nlohmann::json data) {
  Status status{};
  try {
    if (!data.is_null() && data.is_object()) {
    status.id                  = std::stoul(data["id"].get<std::string>());
    status.created_at          = GetJSONStringValue    (data, "created_at");
    status.replying_to_id      = GetJSONStringValue    (data, "in_reply_to_id");
    status.replying_to_account = GetJSONStringValue    (data, "in_reply_to_account_id");
    status.sensitive           = GetJSONBoolValue      (data, "sensitive");
    status.spoiler             = GetJSONStringValue    (data, "spoiler_text");
    status.visibility          = GetJSONStringValue    (data, "visibility");
    status.language            = GetJSONStringValue    (data, "language");
    status.uri                 = GetJSONStringValue    (data, "uri");
    status.url                 = GetJSONStringValue    (data, "url");
    status.replies             = GetJSONValue<uint32_t>(data, "replies_count");
    status.reblogs             = GetJSONValue<uint32_t>(data, "reblogs_count");
    status.favourites          = GetJSONValue<uint32_t>(data, "favourites_count");
    status.content             = GetJSONStringValue    (data, "content");
    status.reblog              = ParseAccountFromJSON  (data["reblog"]);;
    status.application.name    = GetJSONStringValue    (data["application"], "name");
    status.application.url     = GetJSONStringValue    (data["application"], "website");
    status.account             = ParseAccountFromJSON  (data["account"]);
    status.media               = ParseMediaFromJSONArr (data["media_attachments"]);
    status.mentions            = ParseMentionsFromJSON (data["mentions"]);
    status.tags                = ParseTagsFromJSON     (data["tags"]);
    status.emojis              = GetJSONValue<std::vector<std::string>>(data, "emojis");
  }

  return status;
  } catch (std::exception& e) {
    std::cout << e.what();
  }
  return status;
}

inline std::vector<Status> JSONToStatuses(nlohmann::json data) {
  std::vector<Status> statuses{};

  for (const auto& status_data : data) {
    statuses.emplace_back(JSONToStatus(status_data));
  }

  return statuses;
}

inline std::vector<Conversation> JSONToConversation(nlohmann::json data) {
  std::vector<Conversation> conversations{};

  if (!data.is_null() && data.is_array()) {
    for (const auto& item : data) {
      if (!item.is_null() && item.is_object()) {
        conversations.emplace_back(
          Conversation{
            .id       = GetJSONStringValue(item, "id"),
            .unread   = GetJSONBoolValue(item, "unread"),
            .accounts = ParseAccountsFromJSON(item["accounts"]),
            .statuses = std::vector<Status>{JSONToStatus(item["last_status"])}
          }
        );
      }
    }
  }
  return conversations;
}
