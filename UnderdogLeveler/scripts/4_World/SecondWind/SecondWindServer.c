class SecondWindServer
{
	protected static ref SecondWindServer s_Instance;

	protected ref SecondWindSettings m_Settings;
	protected ref map<string, ref SecondWindPlayerStats> m_PlayerStats;
	protected ref map<string, ref SecondWindRuntime> m_Runtime;
	protected ref map<string, ref array<string>> m_PendingNotifications;
	protected int m_LastSaveMs;
	protected int m_LastPlayerTickMs;

	static SecondWindServer GetInstance()
	{
		if (!s_Instance)
			s_Instance = new SecondWindServer();
		return s_Instance;
	}

	void SecondWindServer()
	{
		m_PlayerStats = new map<string, ref SecondWindPlayerStats>;
		m_Runtime = new map<string, ref SecondWindRuntime>;
		m_LastSaveMs = 0;
		m_LastPlayerTickMs = 0;
		m_PendingNotifications = new map<string, ref array<string>>;
		m_Settings = SecondWindSettings.Load();
		LoadOfflinePlayersFromDisk();

		Print("[UnderdogLeveler] Server initialised. Enabled=" + m_Settings.EnableSystem.ToString() + " Hotkey=" + m_Settings.Hotkey);
	}

	// ------------------------------------------------------------------ settings

	SecondWindSettings GetSettings()
	{
		if (!m_Settings)
			m_Settings = SecondWindSettings.Load();
		return m_Settings;
	}

	void ReloadSettings()
	{
		m_Settings = SecondWindSettings.Load();
	}

	string GetHotkey()
	{
		SecondWindSettings settings = GetSettings();
		if (!settings || settings.Hotkey == "")
			return "Y";
		return settings.Hotkey;
	}

	bool IsEnabled()
	{
		SecondWindSettings settings = GetSettings();
		return (settings && settings.EnableSystem == 1);
	}

	// ------------------------------------------------------------------ identity / player access

	protected string GetPlayerId(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return "";
		string id = player.GetIdentity().GetPlainId();
		if (id == "")
			id = player.GetIdentity().GetId();
		return id;
	}

	PlayerBase FindOnlinePlayerById(string playerId)
	{
		if (playerId == "")
			return null;

		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase pb = PlayerBase.Cast(players.Get(i));
			if (!pb || !pb.GetIdentity())
				continue;

			string liveId = pb.GetIdentity().GetPlainId();
			if (liveId == "")
				liveId = pb.GetIdentity().GetId();

			if (liveId == playerId)
				return pb;
		}
		return null;
	}

	void EnsurePlayer(PlayerBase player)
	{
		if (!GetGame() || !GetGame().IsServer() || !player || !player.GetIdentity())
			return;

		string playerId = GetPlayerId(player);
		string playerName = player.GetIdentity().GetName();

		if (!m_PlayerStats.Contains(playerId))
			m_PlayerStats.Insert(playerId, SecondWindPlayerStats.LoadOrCreate(playerId, playerName));

		SecondWindPlayerStats stats = m_PlayerStats.Get(playerId);
		if (stats && playerName != "")
			stats.PlayerName = playerName;

		EnsureRuntime(playerId);
	}

	SecondWindPlayerStats GetStats(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return null;

		EnsurePlayer(player);
		return m_PlayerStats.Get(GetPlayerId(player));
	}

	protected SecondWindRuntime EnsureRuntime(string playerId)
	{
		if (playerId == "")
			return null;

		if (!m_Runtime.Contains(playerId))
			m_Runtime.Insert(playerId, new SecondWindRuntime());

		return m_Runtime.Get(playerId);
	}

	SecondWindRuntime GetRuntime(PlayerBase player)
	{
		string playerId = GetPlayerId(player);
		if (playerId == "")
			return null;

		return EnsureRuntime(playerId);
	}

	// ------------------------------------------------------------------ persistence

	void LoadOfflinePlayersFromDisk()
	{
		SecondWindSettings.EnsureFolders();

		string path = string.Format("%1/*.json", SW_PROFILE_DB_DIR);
		string fileName;
		FileAttr fileAttr;

		FindFileHandle handle = FindFile(path, fileName, fileAttr, 0);
		if (fileName != "")
			LoadOfflinePlayerFile(fileName);

		while (FindNextFile(handle, fileName, fileAttr))
		{
			if (fileName != "")
				LoadOfflinePlayerFile(fileName);
		}

		CloseFindFile(handle);
	}

	void LoadOfflinePlayerFile(string fileName)
	{
		if (!fileName.Contains(".json"))
			return;

		string playerId = fileName;
		playerId.Replace(".json", "");

		int slash = playerId.LastIndexOf("/");
		int bslash = playerId.LastIndexOf("\\");
		if (slash > -1)
			playerId = playerId.Substring(slash + 1, playerId.Length() - (slash + 1));
		if (bslash > -1)
			playerId = playerId.Substring(bslash + 1, playerId.Length() - (bslash + 1));

		if (playerId == "")
			return;

		if (!m_PlayerStats.Contains(playerId))
		{
			ref SecondWindPlayerStats stats = SecondWindPlayerStats.LoadOrCreate(playerId, "");
			if (stats)
				m_PlayerStats.Insert(playerId, stats);
		}
	}

	void SaveIfNeeded(SecondWindPlayerStats stats)
	{
		if (stats)
			stats.Save();
	}

	void SaveAll()
	{
		foreach (string playerId, SecondWindPlayerStats stats : m_PlayerStats)
		{
			if (stats)
				stats.Save();
		}
	}

	// ------------------------------------------------------------------ per-frame tick

	void Tick()
	{
		if (!GetGame() || !GetGame().IsServer())
			return;

		int now = GetGame().GetTime();

		if (now - m_LastSaveMs >= SW_SAVE_INTERVAL_MS)
		{
			m_LastSaveMs = now;
			SaveAll();
		}

		if (now - m_LastPlayerTickMs >= 1000)
		{
			m_LastPlayerTickMs = now;
			TickAllPlayers(now);
		}
	}

	void TickAllPlayers(int now)
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);

		for (int i = 0; i < players.Count(); i++)
		{
			PlayerBase pb = PlayerBase.Cast(players.Get(i));
			if (!pb || !pb.GetIdentity() || !pb.IsAlive())
				continue;

			OnPlayerAliveTick(pb, now);
		}
	}

	// ------------------------------------------------------------------ grit currency

	void AwardGrit(PlayerBase player, int amount, string reason)
	{
		if (amount <= 0 || !player)
			return;

		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return;

		stats.Grit = stats.Grit + amount;
		stats.TotalGritEarned = stats.TotalGritEarned + amount;
		SaveIfNeeded(stats);

		QueueNotification(player, string.Format("+%1 Grit (%2). Balance: %3", amount, reason, stats.Grit));
	}

	bool SpendGrit(PlayerBase player, int cost)
	{
		SecondWindPlayerStats stats = GetStats(player);
		if (!stats || cost < 0)
			return false;
		if (stats.Grit < cost)
			return false;

		stats.Grit = stats.Grit - cost;
		SaveIfNeeded(stats);
		return true;
	}

	void RefundGrit(PlayerBase player, int amount)
	{
		if (amount <= 0)
			return;

		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return;

		stats.Grit = stats.Grit + amount;
		SaveIfNeeded(stats);
	}

	int GetGrit(PlayerBase player)
	{
		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return 0;
		return stats.Grit;
	}

	// ------------------------------------------------------------------ boons

	bool HasBoon(PlayerBase player, string boonId)
	{
		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return false;
		return stats.HasBoon(boonId);
	}

	void GrantBoon(PlayerBase player, string boonId)
	{
		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return;

		stats.AddBoon(boonId);
		SaveIfNeeded(stats);
		QueueNotification(player, string.Format("Boon granted: %1", GetBoonLabel(boonId)));
	}

	string GetBoonLabel(string boonId)
	{
		if (boonId == SW_BOON_IRON_WILL)
			return "Iron Will";
		if (boonId == SW_BOON_SECOND_WIND)
			return "Second Breath";
		if (boonId == SW_BOON_LAST_STAND)
			return "Last Stand";
		return boonId;
	}

	protected void TryTriggerLastStand(PlayerBase player, SecondWindPlayerStats stats)
	{
		if (!player || !stats || !stats.HasBoon(SW_BOON_LAST_STAND))
			return;

		SecondWindSettings settings = GetSettings();

		float hp = player.GetHealth();
		if (hp <= 0 || hp >= settings.LastStandTriggerHealth * 1.0)
			return;

		stats.RemoveBoon(SW_BOON_LAST_STAND);
		float need = settings.LastStandRestoreHealth * 1.0 - hp;
		if (need > 0)
			player.AddHealth(need);

		SaveIfNeeded(stats);
		QueueNotification(player, "Your Last Stand boon kept you alive!");
	}

	protected void ApplyRegen(PlayerBase player, SecondWindRuntime runtime, int now)
	{
		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return;

		if (!stats.HasBoon(SW_BOON_SECOND_WIND))
		{
			runtime.LastRegenAtMs = now;
			return;
		}

		SecondWindSettings settings = GetSettings();
		if (settings.RegenHealPerSecond <= 0)
			return;

		if (now - runtime.LastRegenAtMs < 1000)
			return;

		runtime.LastRegenAtMs = now;

		if (player.GetHealth() < player.GetMaxHealth())
		{
			float add = settings.RegenHealPerSecond * 1.0;
			float room = player.GetMaxHealth() - player.GetHealth();
			if (add > room)
				add = room;
			if (add > 0)
				player.AddHealth(add);
		}
	}

	int GetOnlinePlayerCount()
	{
		array<Man> players = new array<Man>;
		GetGame().GetPlayers(players);
		return players.Count();
	}

	// ------------------------------------------------------------------ per-second alive processing

	void OnPlayerAliveTick(PlayerBase player, int now)
	{
		if (!player || !player.GetIdentity() || !player.IsAlive())
			return;

		SecondWindRuntime runtime = GetRuntime(player);
		if (!runtime)
			return;

		if (runtime.SpawnedAtMs <= 0)
		{
			runtime.SpawnedAtMs = now;
			runtime.LastStipendAtMs = now;
			runtime.StipendEarnedThisLife = 0;
			runtime.LastRegenAtMs = now;
		}

		SecondWindSettings settings = GetSettings();

		if (settings.EnableSystem == 1)
			ProcessSurvivalStipend(player, runtime, settings, now);

		ApplyRegen(player, runtime, now);
		TryTriggerLastStand(player, GetStats(player));
	}

	protected void ProcessSurvivalStipend(PlayerBase player, SecondWindRuntime runtime, SecondWindSettings settings, int now)
	{
		// Require a real population so grit cannot be farmed on an empty server.
		if (GetOnlinePlayerCount() < settings.MinPlayersForGrit)
			return;

		// The stipend only ticks for players who are genuinely struggling,
		// so experienced players cannot just camp for free grit.
		SecondWindPlayerStats stipendStats = GetStats(player);
		if (stipendStats)
		{
			bool underdog = (stipendStats.PlayerDeaths >= settings.MinDeathsToStartSurvivalStipend);
			if (!underdog && stipendStats.PlayerDeaths > 0 && stipendStats.GetKD() < (settings.UnderdogKDPermille / 1000.0))
				underdog = true;
			if (!underdog)
				return;
		}

		int aliveSeconds = (now - runtime.SpawnedAtMs) / 1000;
		if (aliveSeconds < settings.MinAliveSecondsBeforeDeathCounts)
			return;

		if (now - runtime.LastStipendAtMs < settings.SurvivalStipendIntervalSeconds * 1000)
			return;

		int earned = settings.GritPerMinuteAlive * settings.SurvivalStipendIntervalSeconds / 60;
		if (earned <= 0)
			earned = settings.GritPerMinuteAlive;

		int remaining = settings.MaxSurvivalStipendPerLife - runtime.StipendEarnedThisLife;
		if (remaining <= 0)
			return;

		if (earned > remaining)
			earned = remaining;

		runtime.StipendEarnedThisLife = runtime.StipendEarnedThisLife + earned;
		runtime.LastStipendAtMs = now;

		AwardGrit(player, earned, "survival");
	}

	// ------------------------------------------------------------------ hit processing (boons + shot stats)

	void OnPlayerHit(PlayerBase player, float healthBefore, float healthAfter, EntityAI source, string dmgZone, string ammo)
	{
		if (!player)
			return;

		TrackShotHit(player, source, dmgZone, ammo);

		if (!player.IsAlive())
			return;

		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return;

		SecondWindSettings settings = GetSettings();

		float lost = healthBefore - healthAfter;
		if (lost > 0 && stats.HasBoon(SW_BOON_IRON_WILL))
		{
			float resist = settings.IronWillResistancePercent / 100.0;
			float restore = lost * resist;

			if (restore > 0)
			{
				float max = player.GetMaxHealth();
				float current = player.GetHealth();
				if (current < max)
				{
					float room = max - current;
					if (restore > room)
						restore = room;
					if (restore > 0)
						player.AddHealth(restore);
				}
			}
		}

		TryTriggerLastStand(player, stats);
	}

	protected void TrackShotHit(PlayerBase victim, EntityAI source, string dmgZone, string ammo)
	{
		if (ammo == "")
			return;

		PlayerBase shooter = SecondWindHelpers.GetSourcePlayerFromObject(source);
		if (!shooter || !shooter.GetIdentity())
			return;

		if (shooter == victim)
			return;

		if (!victim.SWShouldCountIncomingHit(shooter, ammo))
			return;

		float distance = vector.Distance(shooter.GetPosition(), victim.GetPosition());
		AddShotHit(shooter, SecondWindHelpers.IsHeadZone(dmgZone), distance);
	}

	void AddShotFired(PlayerBase shooter)
	{
		SecondWindPlayerStats stats = GetStats(shooter);
		if (!stats)
			return;

		stats.ShotsFired++;
		SaveIfNeeded(stats);
	}

	void AddShotHit(PlayerBase shooter, bool headshot, float distance)
	{
		SecondWindPlayerStats stats = GetStats(shooter);
		if (!stats)
			return;

		stats.ShotsHit++;

		if (stats.ShotsFired < stats.ShotsHit)
			stats.ShotsFired = stats.ShotsHit;

		if (headshot)
			stats.Headshots++;

		if (distance > stats.LongestShotDistance)
			stats.LongestShotDistance = distance;

		SaveIfNeeded(stats);
	}

	string BuildStatsText(PlayerBase player, int rank)
	{
		SecondWindPlayerStats stats = GetStats(player);
		if (!stats)
			return "No stats yet.";

		string name = stats.PlayerName;
		if (name == "")
			name = "Survivor";

		string text = "";
		text = text + name + "\n";
		if (rank > 0)
			text = text + "Rank: #" + rank.ToString() + "     Grit: " + stats.Grit.ToString() + "\n";
		else
			text = text + "Grit: " + stats.Grit.ToString() + "\n";
		text = text + "------------------------------\n";
		text = text + "Kills:         " + stats.PlayerKills.ToString() + "\n";
		text = text + "Deaths:        " + stats.PlayerDeaths.ToString() + "\n";
		text = text + "K/D:           " + SecondWindHelpers.FormatFloat(stats.GetKD(), 2) + "\n";
		text = text + "Longest Shot:  " + SecondWindHelpers.FormatFloat(stats.LongestShotDistance, 1) + "m\n";
		text = text + "Headshots:     " + stats.Headshots.ToString() + "\n";
		text = text + "Accuracy:      " + SecondWindHelpers.FormatFloat(stats.GetAccuracy(), 1) + "%\n";
		text = text + "Shots Fired:   " + stats.ShotsFired.ToString() + "\n";
		text = text + "Shots Hit:     " + stats.ShotsHit.ToString();
		return text;
	}

	// ------------------------------------------------------------------ leaderboard

	protected ref array<ref SecondWindPlayerStats> GetLeaderboardEntries()
	{
		ref array<ref SecondWindPlayerStats> entries = new array<ref SecondWindPlayerStats>;

		foreach (string playerId, SecondWindPlayerStats stats : m_PlayerStats)
		{
			if (stats)
				entries.Insert(stats);
		}

		SortLeaderboardEntries(entries);
		return entries;
	}

	protected void SortLeaderboardEntries(array<ref SecondWindPlayerStats> entries)
	{
		int n = entries.Count();
		for (int i = 0; i < n - 1; i++)
		{
			int best = i;
			for (int j = i + 1; j < n; j++)
			{
				if (LeaderboardIsBetter(entries.Get(j), entries.Get(best)))
					best = j;
			}

			if (best != i)
			{
				SecondWindPlayerStats tmp = entries.Get(i);
				entries.Set(i, entries.Get(best));
				entries.Set(best, tmp);
			}
		}
	}

	protected bool LeaderboardIsBetter(SecondWindPlayerStats a, SecondWindPlayerStats b)
	{
		if (!a)
			return false;
		if (!b)
			return true;

		if (a.PlayerKills != b.PlayerKills)
			return a.PlayerKills > b.PlayerKills;

		float kdA = a.GetKD();
		float kdB = b.GetKD();
		if (kdA != kdB)
			return kdA > kdB;

		return a.LongestShotDistance > b.LongestShotDistance;
	}

	int GetLeaderboardRank(PlayerBase player)
	{
		string selfId = GetPlayerId(player);
		if (selfId == "")
			return 0;

		ref array<ref SecondWindPlayerStats> entries = GetLeaderboardEntries();
		for (int i = 0; i < entries.Count(); i++)
		{
			SecondWindPlayerStats stats = entries.Get(i);
			if (stats && stats.PlayerId == selfId)
				return i + 1;
		}

		return 0;
	}

	string BuildLeaderboardText(PlayerBase player)
	{
		string selfId = GetPlayerId(player);
		ref array<ref SecondWindPlayerStats> entries = GetLeaderboardEntries();

		string body = "";
		bool first = true;

		int limit = entries.Count();
		if (limit > SW_LEADERBOARD_LIMIT)
			limit = SW_LEADERBOARD_LIMIT;

		for (int i = 0; i < limit; i++)
		{
			SecondWindPlayerStats stats = entries.Get(i);
			if (!stats)
				continue;

			int isSelf = 0;
			if (selfId != "" && stats.PlayerId == selfId)
				isSelf = 1;

			string name = stats.PlayerName;
			if (name == "")
				name = "Survivor";

			string line = isSelf.ToString() + "\t" + name + "\t" + stats.PlayerKills.ToString() + "\t" + stats.PlayerDeaths.ToString() + "\t" + SecondWindHelpers.FormatFloat(stats.GetKD(), 2) + "\t" + stats.Headshots.ToString() + "\t" + SecondWindHelpers.FormatFloat(stats.LongestShotDistance, 1);

			if (!first)
				body = body + "\n";
			body = body + line;
			first = false;
		}

		return body;
	}

	// ------------------------------------------------------------------ death processing

	void OnPlayerDied(PlayerBase victim, Object killer)
	{
		if (!victim)
			return;

		bool wasHuman = (victim.GetIdentity() != null);
		if (!wasHuman)
			return;

		SecondWindPlayerStats stats = GetStats(victim);
		if (!stats)
			return;

		SecondWindSettings settings = GetSettings();

		stats.TotalDeaths++;
		int now = GetGame().GetTime();

		if (settings.BoonsResetOnDeath == 1)
			stats.ClearBoons();

		SecondWindRuntime runtime = GetRuntime(victim);

		// A life ended - the next time we see this player alive we reset the clock.
		int aliveMs = 0;
		if (runtime)
		{
			if (runtime.SpawnedAtMs > 0)
				aliveMs = now - runtime.SpawnedAtMs;
			runtime.SpawnedAtMs = 0;
			runtime.StipendEarnedThisLife = 0;
		}

		PlayerBase killerPlayer = SecondWindHelpers.GetSourcePlayerFromObject(killer);

		if (!killerPlayer || !killerPlayer.GetIdentity())
		{
			// Environment, fall, starvation, zombie or animal: no consolation.
			// Leave the comeback streak intact so genuine PvP deaths still count.
			SaveIfNeeded(stats);
			return;
		}

		if (killerPlayer == victim)
		{
			// Suicide (self-inflicted). No consolation, and it breaks the streak
			// so it cannot be farmed before a single real death.
			stats.DeathStreak = 0;
			SaveIfNeeded(stats);
			return;
		}

		// A genuine PvP death.
		SecondWindPlayerStats killerStats = GetStats(killerPlayer);
		if (killerStats)
		{
			killerStats.PlayerKills++;
			killerStats.DeathStreak = 0;
			SaveIfNeeded(killerStats);
		}

		stats.PlayerDeaths++;
		stats.DeathStreak++;

		AwardDeathConsolation(victim, stats, runtime, killerPlayer, settings, now, aliveMs);
		SaveIfNeeded(stats);
	}

	protected void AwardDeathConsolation(PlayerBase victim, SecondWindPlayerStats stats, SecondWindRuntime runtime, PlayerBase killerPlayer, SecondWindSettings settings, int now, int aliveMs)
	{
		if (settings.EnableSystem != 1)
			return;

		if (!runtime)
			return;

		// Require a real population so consolation cannot be farmed on an empty server.
		if (GetOnlinePlayerCount() < settings.MinPlayersForGrit)
			return;

		// Must have survived long enough for the death to matter.
		if (aliveMs < settings.MinAliveSecondsBeforeDeathCounts * 1000)
			return;

		// Cooldown between consolations.
		if (now - runtime.LastDeathConsolationAtMs < settings.DeathConsolationCooldownSeconds * 1000)
			return;

		// Rolling window cap.
		if (runtime.DeathConsolationWindowStartMs == 0 || (now - runtime.DeathConsolationWindowStartMs) > settings.DeathConsolationWindowSeconds * 1000)
		{
			runtime.DeathConsolationWindowStartMs = now;
			runtime.RecentConsolations = 0;
		}

		if (runtime.RecentConsolations >= settings.MaxDeathConsolationsPerWindow)
			return;

		string killerId = killerPlayer.GetIdentity().GetPlainId();
		if (killerId == "")
			killerId = killerPlayer.GetIdentity().GetId();

		int baseGrit = settings.PvPDeathBaseGrit;

		// Underdog multiplier: struggling players earn more.
		float kd = stats.GetKD();
		if (stats.PlayerDeaths > 0 && kd < (settings.UnderdogKDPermille / 1000.0))
			baseGrit = baseGrit * settings.UnderdogMultiplierPercent / 100;

		// Comeback streak bonus: keep coming back, keep earning a little more.
		int streakBonus = stats.DeathStreak * settings.DownstreakBonusPerDeath;
		if (streakBonus > settings.MaxDownstreakBonus)
			streakBonus = settings.MaxDownstreakBonus;

		int total = baseGrit + streakBonus;

		// Same-killer farm decay: repeated deaths to the same player quickly taper off.
		if (runtime.LastKillerId == killerId && (now - runtime.LastDeathConsolationAtMs) < settings.FarmDecayWindowSeconds * 1000)
		{
			if (total > settings.FarmDecayMinimumGrit)
				total = settings.FarmDecayMinimumGrit;
		}

		if (total <= 0)
			total = 1;

		runtime.LastDeathConsolationAtMs = now;
		runtime.RecentConsolations++;
		runtime.LastKillerId = killerId;

		AwardGrit(victim, total, "death consolation");
	}

	// ------------------------------------------------------------------ shop

	string BuildShopText()
	{
		SecondWindSettings settings = GetSettings();
		string body = "";
		bool first = true;

		if (settings && settings.Offers)
		{
			for (int i = 0; i < settings.Offers.Count(); i++)
			{
				SecondWindShopOffer offer = settings.Offers.Get(i);
				if (!offer)
					continue;

				string desc = offer.Description;
				desc.Replace("\t", " ");
				desc.Replace("\n", " ");

				string line = offer.Id + "\t" + offer.DisplayName + "\t" + desc + "\t" + offer.Cost.ToString() + "\t" + offer.Category;
				if (!first)
					body = body + "\n";
				body = body + line;
				first = false;
			}
		}

		return body;
	}

	bool PurchaseOffer(PlayerBase player, string offerId, out string resultMessage)
	{
		resultMessage = "Purchase failed.";

		if (!player || !player.GetIdentity())
		{
			resultMessage = "Invalid player.";
			return false;
		}

		SecondWindSettings settings = GetSettings();
		if (settings.EnableSystem != 1)
		{
			resultMessage = "Underdog Leveler is disabled on this server.";
			return false;
		}

		SecondWindShopOffer offer = settings.FindOffer(offerId);
		if (!offer)
		{
			resultMessage = "Offer not found.";
			return false;
		}

		if (offer.BoonId != "")
		{
			if (!SpendGrit(player, offer.Cost))
			{
				resultMessage = "Not enough Grit.";
				return false;
			}

			GrantBoon(player, offer.BoonId);
			resultMessage = string.Format("Boon granted: %1", GetBoonLabel(offer.BoonId));
			return true;
		}

		if (!SpendGrit(player, offer.Cost))
		{
			resultMessage = "Not enough Grit.";
			return false;
		}

		if (!SpawnOfferItems(player, offer))
		{
			RefundGrit(player, offer.Cost);
			resultMessage = "Could not deliver the items - inventory may be full.";
			return false;
		}

		resultMessage = string.Format("Purchased %1", offer.DisplayName);
		return true;
	}

	EntityAI SpawnItemForPlayer(PlayerBase player, string className)
	{
		if (!player || className == "" || !player.GetInventory())
			return null;

		EntityAI item = EntityAI.Cast(player.GetInventory().CreateInInventory(className));
		if (item)
			return item;

		// Fallback: drop at the player's feet so a bulky reward is never lost.
		vector pos = player.GetPosition();
		vector dir = player.GetDirection();
		dir.Normalize();
		pos = pos + (dir * 0.6);
		pos[1] = GetGame().SurfaceY(pos[0], pos[2]) + 0.1;

		EntityAI dropped = EntityAI.Cast(GetGame().CreateObjectEx(className, pos, 0));
		if (dropped)
			dropped.SetPosition(pos);

		return dropped;
	}

	void ApplyAttachments(EntityAI parent, array<string> attachmentClassNames)
	{
		if (!parent || !parent.GetInventory() || !attachmentClassNames || attachmentClassNames.Count() == 0)
			return;

		for (int i = 0; i < attachmentClassNames.Count(); i++)
		{
			string cls = attachmentClassNames.Get(i);
			cls.Trim();
			if (cls == "")
				continue;

			parent.GetInventory().CreateAttachment(cls);
		}
	}

	bool SpawnOfferItems(PlayerBase player, SecondWindShopOffer offer)
	{
		if (!player || !offer || offer.ClassName == "" || !player.GetInventory())
			return false;

		offer.EnsureArrays();

		int quantity = offer.Quantity;
		if (quantity <= 0)
			quantity = 1;

		bool any = false;

		for (int i = 0; i < quantity; i++)
		{
			EntityAI item = SpawnItemForPlayer(player, offer.ClassName);
			if (item)
			{
				ApplyAttachments(item, offer.SpawnAttachments);
				any = true;
			}
		}

		for (int e = 0; e < offer.ExtraItems.Count(); e++)
		{
			string entry = offer.ExtraItems.Get(e);
			string cls = entry;
			int extraQty = 1;

			int sep = entry.IndexOf(":");
			if (sep > -1)
			{
				cls = entry.Substring(0, sep);
				string qtyText = entry.Substring(sep + 1, entry.Length() - (sep + 1));
				qtyText.Trim();
				if (qtyText != "")
					extraQty = qtyText.ToInt();
			}

			cls.Trim();
			if (cls == "")
				continue;
			if (extraQty <= 0)
				extraQty = 1;

			for (int k = 0; k < extraQty; k++)
			{
				EntityAI extra = SpawnItemForPlayer(player, cls);
				if (extra)
					any = true;
			}
		}

		return any;
	}

	// ------------------------------------------------------------------ client messaging (queued; flushed from the Mission module)

	void QueueNotification(PlayerBase player, string message)
	{
		if (!player || !player.GetIdentity() || message == "")
			return;

		string playerId = GetPlayerId(player);
		if (playerId == "")
			return;

		if (!m_PendingNotifications)
			m_PendingNotifications = new map<string, ref array<string>>;

		if (!m_PendingNotifications.Contains(playerId))
			m_PendingNotifications.Insert(playerId, new array<string>);

		m_PendingNotifications.Get(playerId).Insert(message);
	}

	ref map<string, ref array<string>> TakePendingNotifications()
	{
		if (!m_PendingNotifications || m_PendingNotifications.Count() == 0)
			return null;

		ref map<string, ref array<string>> result = m_PendingNotifications;
		m_PendingNotifications = new map<string, ref array<string>>;
		return result;
	}
}
