modded class PlayerBase
{
	protected int m_SWLastShotCountMs;
	protected string m_SWLastShotSignature;
	protected int m_SWLastHitCountMs;
	protected string m_SWLastHitAmmo;
	protected string m_SWLastHitShooterId;

	bool SWShouldCountShotFired(Weapon_Base weapon, int muzzleType, int mode, string ammoType)
	{
		int now = GetGame().GetTime();
		string signature = "";
		if (weapon)
			signature = weapon.GetType();
		signature = signature + "|" + muzzleType.ToString() + "|" + mode.ToString() + "|" + ammoType;

		if (m_SWLastShotSignature != "" && now == m_SWLastShotCountMs && signature == m_SWLastShotSignature)
			return false;

		m_SWLastShotCountMs = now;
		m_SWLastShotSignature = signature;
		return true;
	}

	bool SWShouldCountIncomingHit(PlayerBase sourcePlayer, string ammoType)
	{
		if (!sourcePlayer || !sourcePlayer.GetIdentity())
			return false;

		int now = GetGame().GetTime();
		string shooterId = sourcePlayer.GetIdentity().GetPlainId();

		if (m_SWLastHitShooterId != "" && (now - m_SWLastHitCountMs) <= 25 && ammoType == m_SWLastHitAmmo && shooterId == m_SWLastHitShooterId)
			return false;

		m_SWLastHitCountMs = now;
		m_SWLastHitAmmo = ammoType;
		m_SWLastHitShooterId = shooterId;
		return true;
	}

	override void OnConnect()
	{
		super.OnConnect();

		if (GetGame().IsServer() && GetIdentity())
			SecondWindServer.GetInstance().EnsurePlayer(this);
	}

	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

		if (GetGame().IsServer())
			SecondWindServer.GetInstance().OnPlayerDied(this, killer);
	}

	override void EEHitBy(TotalDamageResult damageResult, int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos, float speedCoef)
	{
		bool isServer = GetGame().IsServer();
		float healthBefore = 0.0;

		if (isServer)
			healthBefore = GetHealth();

		super.EEHitBy(damageResult, damageType, source, component, dmgZone, ammo, modelPos, speedCoef);

		if (isServer)
			SecondWindServer.GetInstance().OnPlayerHit(this, healthBefore, GetHealth(), source, dmgZone, ammo);
	}
}
