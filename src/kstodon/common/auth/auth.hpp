#ifndef __AUTH_HPP__
#define __AUTH_HPP__

#include <INIReader.h>
#include <nlohmann/json.hpp>
#include <cpr/cpr.h>

#include "kstodon/common/constants.hpp"
#include "kstodon/common/util.hpp"

inline const std::string get_executable_cwd() {
  char* path = realpath("/proc/self/exe", NULL);
  char* name = basename(path);
  return std::string{path, path + strlen(path) - strlen(name)};
}

struct Credentials {
std::string id;
std::string name;
std::string website;
std::string redirect_uri;
std::string scope;
std::string client_id;
std::string client_secret;
std::string vapid_key;
std::string code;

bool is_valid() {
  return
    !id.empty() &&
    !name.empty() &&
    !website.empty() &&
    !redirect_uri.empty() &&
    !scope.empty() &&
    !client_id.empty() &&
    !client_secret.empty() &&
    !vapid_key.empty() &&
    !code.empty();
}
};

struct Auth {
std::string access_token;
std::string token_type;
std::string scope;
std::string created_at;

bool is_valid() {
  return (
    !access_token.empty() &&
    !token_type.empty()   &&
    !scope.empty()        &&
    !created_at.empty()
  );
}
};

inline nlohmann::json LoadJSONFile(std::string path) {
  using namespace nlohmann;
  return json::parse(ReadFromFile(path), nullptr, constants::JSON_PARSE_NO_THROW);
}

inline bool ValidateCredentialsJSON(nlohmann::json json_file) {
  return(
    !json_file.is_null()                &&
    json_file.is_object()               &&
    json_file.contains("id")            &&
    json_file.contains("name")          &&
    json_file.contains("website")       &&
    json_file.contains("redirect_uri")  &&
    json_file.contains("scope")         &&
    json_file.contains("client_id")     &&
    json_file.contains("client_secret") &&
    json_file.contains("vapid_key")     &&
    json_file.contains("code")
  );
}

inline Credentials ParseCredentialsFromJSON(nlohmann::json json_file) {
  using json = nlohmann::json;

  Credentials creds{};

  if (ValidateCredentialsJSON(json_file)) {
    creds.id =            SanitizeJSON(json_file["id"].dump());
    creds.name =          SanitizeJSON(json_file["name"].dump());
    creds.website =       SanitizeJSON(json_file["website"].dump());
    creds.redirect_uri =  SanitizeJSON(json_file["redirect_uri"].dump());
    creds.scope        =  SanitizeJSON(json_file["scope"].dump());
    creds.client_id =     SanitizeJSON(json_file["client_id"].dump());
    creds.client_secret = SanitizeJSON(json_file["client_secret"].dump());
    creds.vapid_key =     SanitizeJSON(json_file["vapid_key"].dump());
    creds.code =          SanitizeJSON(json_file["code"].dump());
  }

  return creds;
}

inline bool ValidateAuthJSON(nlohmann::json json_file) {
  return(
    !json_file.is_null()               &&
    json_file.is_object()              &&
    json_file.contains("access_token") &&
    json_file.contains("token_type")   &&
    json_file.contains("scope")        &&
    json_file.contains("created_at")
  );
}

inline std::string AuthToJSON(Auth auth) {
  nlohmann::json auth_json{};
  auth_json["access_token"] = auth.access_token;
  auth_json["token_type"]   = auth.token_type;
  auth_json["scope"]        = auth.scope;
  auth_json["created_at"]   = auth.created_at;

  return auth_json.dump();
}

inline Auth ParseAuthFromJSON(nlohmann::json json_file) {
  using json = nlohmann::json;

  Auth auth{};

  if (ValidateAuthJSON(json_file)) {
    auth.access_token =  SanitizeJSON(json_file["access_token"].dump());
    auth.token_type   =  SanitizeJSON(json_file["token_type"].dump());
    auth.scope        =  SanitizeJSON(json_file["scope"].dump());
    auth.created_at   =  SanitizeJSON(json_file["created_at"].dump());
  }

  return auth;
}


class Authenticator {

public:

Authenticator() {
  INIReader reader{std::string{get_executable_cwd() + "../" + constants::DEFAULT_CONFIG_PATH}};

  if (reader.ParseError() < 0) {
    log("Error loading config");
    throw std::invalid_argument{"No configuration path"};
  }

  auto credentials = ParseCredentialsFromJSON(LoadJSONFile(constants::CONFIG_JSON_PATH));

  if (!credentials.is_valid()) {
    throw std::invalid_argument{"Credentials not found"};
  }

  m_credentials = credentials;

  auto auth = ParseAuthFromJSON(LoadJSONFile(constants::TOKEN_JSON_PATH));

  if (auth.is_valid()) {
    m_auth = auth;
  }
}

/**
 * FetchToken
 *
 * @returns [out] {bool}
 */
bool FetchToken() {
  using namespace constants::MastodonOnline;
  using json = nlohmann::json;

  const std::string AUTHORIZATION_CODE_GRANT_TYPE{"authorization_code"};
  const std::string AUTH_URL = BASE_URL + PATH.at(TOKEN_INDEX);
  std::string       response;
  std::string       status;

  if (m_credentials.is_valid()) {
    cpr::Response r = cpr::Post(
      cpr::Url{AUTH_URL},
      cpr::Body{std::string{
        "client_id=" + m_credentials.client_id +  "&" +
        "client_secret=" + m_credentials.client_secret + "&" +
        "redirect_uri=" + m_credentials.redirect_uri + "&" +
        "grant_type=" + AUTHORIZATION_CODE_GRANT_TYPE + "&" +
        "code=" + m_credentials.code + "&" +
        "scope=" + m_credentials.scope
      }}
    );

    response = r.text;
    status   = std::string{"Status code: " + r.status_code};

    if (!response.empty()) {
      json auth_json = json::parse(response, nullptr, constants::JSON_PARSE_NO_THROW);
      Auth auth      = ParseAuthFromJSON(auth_json);

      if (auth.is_valid()) {
        m_auth = auth;
        SaveToFile(AuthToJSON(auth), constants::TOKEN_JSON_PATH);
        return true;
      } else {
        log("Failed to parse token");
      }
    } else {
      log("Token request failed");
    }
    log(std::string{
      "Failed to fetch token.\n"
      "Code: "     + status + "\n"
      "Response: " + response
    });
  } else {
    log("Credentials are invalid");
  }

  return false;
}

bool VerifyToken() {
  using namespace constants;
  using namespace constants::MastodonOnline;

  const std::string URL = BASE_URL + PATH.at(TOKEN_VERIFY_INDEX);

  // curl \
	// -H 'Authorization: Bearer our_access_token_here' \
	// https://mastodon.example/api/v1/accounts/verify_credentials
  if (m_auth.is_valid()) {
    cpr::Response r = cpr::Get(
      cpr::Url{URL},
      cpr::Header{
        {HEADER_NAMES.at(HEADER_AUTH_INDEX), GetBearerAuth()}
     }
    );

    if (!r.text.empty()) {
      log(r.text);

      // TODO: Parse the following from text:
      /**
       * {"id":"154950","username":"logicp","acct":"logicp","display_name":"Rajinder","locked":false,"bot":false,"discoverable":null,"group":false,"created_at":"2021-01-09T04:13:14.779Z","note":"\u003cp\u003e🇮🇳 software and systems architect / musician. former personal trainer and public servant\u003c/p\u003e","url":"https://mastodon.online/@logicp","avatar":"https://files.mastodon.online/accounts/avatars/000/154/950/original/77ebf6320684a8ef.png","avatar_static":"https://files.mastodon.online/accounts/avatars/000/154/950/original/77ebf6320684a8ef.png","header":"https://files.mastodon.online/accounts/headers/000/154/950/original/9fd1ba851641386b.png","header_static":"https://files.mastodon.online/accounts/headers/000/154/950/original/9fd1ba851641386b.png","followers_count":7,"following_count":12,"statuses_count":21,"last_status_at":"2021-01-11","source":{"privacy":"public","sensitive":false,"language":null,"note":"🇮🇳 software and systems architect / musician. former personal trainer and public servant","fields":[{"name":"System","value":"Linux","verified_at":null},{"name":"Music","value":"Classical Jazz Flamenco","verified_at":null},{"name":"Languages","value":"Eng, Fra, Kor, NL, C++","verified_at":null},{"name":"Train daily","value":"easily","verified_at":null}],"follow_requests_count":0},"emojis":[],"fields":[{"name":"System","value":"Linux","verified_at":null},{"name":"Music","value":"Classical Jazz Flamenco","verified_at":null},{"name":"Languages","value":"Eng, Fra, Kor, NL, C++","verified_at":null},{"name":"Train daily","value":"easily","verified_at":null}]}
       *
       */
      return true;
    }
  }

  return false;
}


bool IsAuthenticated() {
  return m_authenticated;
}

bool HasValidToken() {
  return m_auth.is_valid();
}

void ClearToken() {
  m_auth = Auth{};
}

std::string GetBearerAuth() {
  if (m_auth.access_token.empty())
    return "";
  return std::string{"Bearer " + m_auth.access_token};
}

Credentials get_credentials() {
  return m_credentials;
}

private:
Credentials  m_credentials;
Auth         m_auth;
bool         m_authenticated;
};

#endif // __AUTH_HPP__
