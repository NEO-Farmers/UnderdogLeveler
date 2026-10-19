// Underdog Leveler - Comeback Support
// A deathmatch handicap system for players who keep coming back.

static const string SW_MOD_NAME = "UnderdogLeveler";

static const string SW_PROFILE_ROOT = "$profile:UnderdogLeveler";
static const string SW_PROFILE_SETTINGS_DIR = "$profile:UnderdogLeveler/Settings";
static const string SW_PROFILE_DB_DIR = "$profile:UnderdogLeveler/Players";
static const string SW_SETTINGS_FILE = "$profile:UnderdogLeveler/Settings/UnderdogLevelerSettings.json";

static const string SW_LAYOUT_SHOP = "UnderdogLeveler/gui/layouts/secondwind_shop.layout";
static const string SW_LAYOUT_SHOP_ROW = "UnderdogLeveler/gui/layouts/secondwind_shop_row.layout";
static const string SW_LAYOUT_STATS = "UnderdogLeveler/gui/layouts/secondwind_stats.layout";
static const string SW_LAYOUT_LEADERBOARD_ROW = "UnderdogLeveler/gui/layouts/secondwind_leaderboard_row.layout";

static const int SW_SAVE_INTERVAL_MS = 30000;

// Maximum number of entries shown on the Field Report leaderboard.
static const int SW_LEADERBOARD_LIMIT = 10;

// Boon identifiers (stored as strings in the player profile).
static const string SW_BOON_SECOND_WIND = "second_wind"; // regeneration
static const string SW_BOON_IRON_WILL = "iron_will";     // damage resistance
static const string SW_BOON_LAST_STAND = "last_stand";   // one-time death save

// Shop offer categories (used for display only).
static const string SW_CATEGORY_WEAPON = "WEAPON";
static const string SW_CATEGORY_AMMO = "AMMO";
static const string SW_CATEGORY_EXPLOSIVE = "EXPLOSIVE";
static const string SW_CATEGORY_TRAP = "TRAP";
static const string SW_CATEGORY_MEDICAL = "MEDICAL";
static const string SW_CATEGORY_UTILITY = "UTILITY";
static const string SW_CATEGORY_BOON = "BOON";

enum SecondWindBoonId
{
	SW_BOON_NONE = 0,
	SW_BOON_SECOND_WIND_ID = 1,
	SW_BOON_IRON_WILL_ID = 2,
	SW_BOON_LAST_STAND_ID = 3
};
