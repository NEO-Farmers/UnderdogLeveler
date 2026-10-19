# Underdog Leveler [Killfeed | Leaderboard | Storefront]

A DayZ Standalone server mod that is the opposite of a kill-streak reward mod.
Instead of rewarding skilled players for farming kills, **Underdog Leveler** helps
the people who keep dying and coming back: they earn **Grit**, then spend it in the
**Field Requisition** storefront on scoped rifles, ammunition, grenades, landmines,
bear traps, lockpicks, medical gear, and life-saving **Boons**.

Everything is server-authoritative and persisted per Steam64 ID under the server
profile folder, so a struggling player's Grit and lifetime stats survive restarts
and reconnects. Press **Y** to open the **Field Report** (your stats + the
leaderboard), and use the **Open Store** button to enter the storefront.

---

## How it works

### Earning Grit

Grit is earned two ways, both tuned toward the underdog:

1. **Survival stipend** — a little Grit per interval spent alive
   (`GritPerMinuteAlive`, capped per life by `MaxSurvivalStipendPerLife`).
2. **PvP death consolation** — when another *player* kills you, you receive a
   consolation payout (`PvPDeathBaseGrit`).

Struggling players earn more:

- **Underdog multiplier** — if your lifetime K/D is below `UnderdogKDPermille`
  (default 0.5), death consolation is multiplied by `UnderdogMultiplierPercent`
  (default 150%).
- **Comeback streak** — each consecutive PvP death without a kill adds
  `DownstreakBonusPerDeath` Grit (capped by `MaxDownstreakBonus`).

### Anti-abuse / anti-farm checks

Consolation is only granted for genuine PvP deaths:

- **No suicide payout** — self-inflicted deaths award nothing and reset the streak.
- **No environment payout** — falls, starvation, dehydration, drowning, bleed-outs.
- **No zombie/animal payout**.
- **Minimum alive time** — `MinAliveSecondsBeforeDeathCounts` (default 90s).
- **Cooldown** — `DeathConsolationCooldownSeconds` (default 45s).
- **Rolling window cap** — `MaxDeathConsolationsPerWindow` per
  `DeathConsolationWindowSeconds` (default 8 per 10 minutes).
- **Same-killer farm decay** — repeated deaths to the same killer decay to
  `FarmDecayMinimumGrit`.
- **Population gate** — Grit requires at least `MinPlayersForGrit` players online,
  and survival stipend only starts after `MinDeathsToStartSurvivalStipend` deaths
  (or a low K/D).

### Field Report (stats + leaderboard)

Press **Y** to open the Field Report. It shows your personal stats (name, rank,
K/D, kills, deaths, headshots, accuracy, longest shot, Grit) and a ranked
leaderboard of the top 10 players by kills → K/D → longest shot. Your row is
highlighted in cyan.

### Field Requisition storefront

From the Field Report, click **Open Store**. Scroll with the **mouse wheel**,
click a card to purchase, and press **ESC/B** (or click Exit) to return to the
Field Report.

Default stock (all editable in the settings JSON): 5 scoped rifles, 4 ammo boxes,
frag grenades, land mine, bear trap, lockpicks, a field medic kit, an adrenaline
shot, and 3 Boons.

### Boons (purchasable perks)

Boons are life-based buffs that last until you die (`BoonsResetOnDeath = 1`):

- **Iron Will** — reduces incoming damage by `IronWillResistancePercent`
  (default 25%).
- **Second Breath** — passively regenerates `RegenHealPerSecond` (default 2 HP/s).
- **Last Stand** — once, when your health drops below `LastStandTriggerHealth`,
  you are healed to `LastStandRestoreHealth` and the boon is consumed.

---

## File layout

```
UnderdogLeveler/
├── config.cpp
├── mod.cpp
├── meta.cpp
├── data/
│   └── inputs.xml
├── gui/layouts/
│   ├── secondwind_shop.layout
│   ├── secondwind_shop_row.layout
│   ├── secondwind_stats.layout
│   └── secondwind_leaderboard_row.layout
└── scripts/
    ├── 3_Game/SecondWind/
    │   └── SecondWindConstants.c
    ├── 4_World/SecondWind/
    │   ├── SecondWindHelpers.c
    │   ├── SecondWindSettings.c
    │   ├── SecondWindPlayerStats.c
    │   ├── SecondWindServer.c
    │   └── PlayerBase.c
    └── 5_Mission/SecondWind/
        ├── MissionBase.c
        ├── MissionServer.c
        ├── MissionGameplay.c
        ├── SecondWindShopMenu.c
        └── SecondWindStatsMenu.c
```

## Building the PBO

1. Point the Addon Builder at the `Addons` folder (the parent of `UnderdogLeveler`).
2. Select the `UnderdogLeveler` folder.
3. Pack to `@UnderdogLeveler/Addons/UnderdogLeveler.pbo`.

The `@UnderdogLeveler` mod folder is:

```
@UnderdogLeveler/
└── Addons/
    └── UnderdogLeveler.pbo
```

## Installing on the server

1. Copy `@UnderdogLeveler` into your DayZ server root (next to `DayZServer.exe`).
2. Add `@UnderdogLeveler` to the server launch parameters: `-mod=@UnderdogLeveler`.
3. Start the server. On first boot the mod writes a default
   `$profile:UnderdogLeveler/Settings/UnderdogLevelerSettings.json`.

## Configuration

Settings live at `<server profile>/UnderdogLeveler/Settings/UnderdogLevelerSettings.json`
and are created with defaults on first run. The `Offers` array is fully editable
(class names, costs, quantities, or new offers). Player data (Grit, K/D, boons) is
stored at `<server profile>/UnderdogLeveler/Players/<Steam64ID>.json`.

## Notes / caveats

- Requires Community Framework (`JM_CF_Scripts`); it must load before this mod.
- The mod is loaded with `-mod` so the client also needs it (the Field Report and
  storefront UI run client-side).
- Deathmatch servers that wipe inventory on respawn will drop requisitioned gear
  as expected — this is intentional and keeps the economy healthy.
