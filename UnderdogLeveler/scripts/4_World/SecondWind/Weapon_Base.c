modded class Weapon_Base
{
	override void EEFired(int muzzleType, int mode, string ammoType)
	{
		super.EEFired(muzzleType, mode, ammoType);

		if (!GetGame().IsServer())
			return;

		PlayerBase shooter = PlayerBase.Cast(GetHierarchyRootPlayer());
		if (!shooter || !shooter.GetIdentity())
			return;

		if (!shooter.SWShouldCountShotFired(this, muzzleType, mode, ammoType))
			return;

		SecondWindServer.GetInstance().AddShotFired(shooter);
	}
}
