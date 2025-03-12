extern bool g_dataFetched;
extern matjson::Value g_sheetData;
extern std::string googleSheetsScript;

extern bool g_inClan;
extern std::string g_clanTag;
extern std::string g_username;

std::string addTagToUsername(const std::string& clanTag, const std::string& username);
float getNewScale(const std::string& clanTag, const float ogScale);