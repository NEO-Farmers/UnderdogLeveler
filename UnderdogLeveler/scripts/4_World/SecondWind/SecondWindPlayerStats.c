class SecondWindPlayerStats
{
	string PlayerId;
	string PlayerName;

	int Grit;
	int TotalGritEarned;
	int PlayerKills;
	int PlayerDeaths;
	int TotalDeaths;
	int DeathStreak;
	float TimeAliveSeconds;

	int ShotsFired;
	int ShotsHit;
	int Headshots;
	float LongestShotDistance;

	ref array<string> Boons;

	void SecondWindPlayerStats()
	{
		Boons = new array<string>;
	}

	void Reset()
	{
		Grit = 0;
		TotalGritEarned = 0;
		PlayerKills = 0;
		PlayerDeaths = 0;
		TotalDeaths = 0;
		DeathStreak = 0;
		TimeAliveSeconds = 0;
		ShotsFired = 0;
		ShotsHit = 0;
		Headshots = 0;
		LongestShotDistance = 0;
		if (!Boons)
			Boons = new array<string>;
		Boons.Clear();
	}

	float GetAccuracy()
	{
		if (ShotsFired <= 0)
			return 0.0;
		return (ShotsHit * 100.0) / ShotsFired;
	}

	float GetKD()
	{
		if (PlayerDeaths <= 0)
			return PlayerKills * 1.0;
		return PlayerKills / (PlayerDeaths * 1.0);
	}

	bool HasBoon(string boonId)
	{
		if (!Boons || boonId == "")
			return false;

		for (int i = 0; i < Boons.Count(); i++)
		{
			if (Boons.Get(i) == boonId)
				return true;
		}

		return false;
	}

	void AddBoon(string boonId)
	{
		if (boonId == "")
			return;

		if (!Boons)
			Boons = new array<string>;

		if (!HasBoon(boonId))
			Boons.Insert(boonId);
	}

	void RemoveBoon(string boonId)
	{
		if (!Boons)
			return;

		for (int i = Boons.Count() - 1; i >= 0; i--)
		{
			if (Boons.Get(i) == boonId)
				Boons.Remove(i);
		}
	}

	void ClearBoons()
	{
		if (!Boons)
			Boons = new array<string>;
		Boons.Clear();
	}

	void Save()
	{
		SecondWindSettings.EnsureFolders();
		JsonFileLoader<SecondWindPlayerStats>.JsonSaveFile(GetFilePath(), this);
	}

	string GetFilePath()
	{
		return string.Format("%1/%2.json", SW_PROFILE_DB_DIR, PlayerId);
	}

	bool IsValidForPlayer(string playerId)
	{
		return PlayerId == playerId && PlayerId != "";
	}

	static ref SecondWindPlayerStats LoadOrCreate(string playerId, string playerName)
	{
		string filePath = string.Format("%1/%2.json", SW_PROFILE_DB_DIR, playerId);

		ref SecondWindPlayerStats data = new SecondWindPlayerStats();
		if (FileExist(filePath))
			JsonFileLoader<SecondWindPlayerStats>.JsonLoadFile(filePath, data);

		if (!data.IsValidForPlayer(playerId))
		{
			data = new SecondWindPlayerStats();
			data.PlayerId = playerId;
			data.PlayerName = playerName;
			data.Reset();
		}

		if (playerName != "")
			data.PlayerName = playerName;

		if (!data.Boons)
			data.Boons = new array<string>;

		data.Save();
		return data;
	}
}

// Per-session runtime state (never persisted to disk). Tracks spawn time,
// stipend cadence and death-consolation anti-abuse counters for one player.
class SecondWindRuntime
{
	int SpawnedAtMs;
	int LastStipendAtMs;
	int StipendEarnedThisLife;
	int LastDeathConsolationAtMs;
	int DeathConsolationWindowStartMs;
	int RecentConsolations;
	string LastKillerId;
	int LastRegenAtMs;

	void SecondWindRuntime()
	{
		SpawnedAtMs = 0;
		LastStipendAtMs = 0;
		StipendEarnedThisLife = 0;
		LastDeathConsolationAtMs = 0;
		DeathConsolationWindowStartMs = 0;
		RecentConsolations = 0;
		LastKillerId = "";
		LastRegenAtMs = 0;
	}
}
