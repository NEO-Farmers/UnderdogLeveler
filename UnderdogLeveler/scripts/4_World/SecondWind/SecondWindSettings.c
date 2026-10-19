// Shop offer definition. Boon offers leave ClassName empty and set BoonId;
// item offers set ClassName and may carry attachments plus a list of extra
// loose items ("ClassName:Quantity" strings) such as starter ammunition.

class SecondWindShopOffer
{
	string Id;
	string DisplayName;
	string Description;
	string Category;
	string ClassName;
	int Quantity;
	int Cost;
	ref array<string> SpawnAttachments;
	ref array<string> ExtraItems;
	string BoonId;

	void SecondWindShopOffer()
	{
		Quantity = 1;
		Cost = 10;
		SpawnAttachments = new array<string>;
		ExtraItems = new array<string>;
		BoonId = "";
	}

	void EnsureArrays()
	{
		if (!SpawnAttachments)
			SpawnAttachments = new array<string>;
		if (!ExtraItems)
			ExtraItems = new array<string>;
	}
}

class SecondWindSettings
{
	int EnableSystem;

	// Grit earning - survival participation stipend.
	int GritPerMinuteAlive;
	int SurvivalStipendIntervalSeconds;
	int MaxSurvivalStipendPerLife;

	// Grit earning - PvP death consolation.
	int PvPDeathBaseGrit;
	int UnderdogKDPermille;
	int UnderdogMultiplierPercent;
	int DownstreakBonusPerDeath;
	int MaxDownstreakBonus;

	// Anti-abuse.
	int MinAliveSecondsBeforeDeathCounts;
	int DeathConsolationCooldownSeconds;
	int DeathConsolationWindowSeconds;
	int MaxDeathConsolationsPerWindow;
	int FarmDecayWindowSeconds;
	int FarmDecayMinimumGrit;

	// Population / underdog gating (anti-camp).
	int MinPlayersForGrit;
	int MinDeathsToStartSurvivalStipend;

	// Boons.
	int BoonsResetOnDeath;
	int IronWillResistancePercent;
	int RegenHealPerSecond;
	int LastStandTriggerHealth;
	int LastStandRestoreHealth;

	string Hotkey;

	ref array<ref SecondWindShopOffer> Offers;

	void SecondWindSettings()
	{
		EnableSystem = 1;
		GritPerMinuteAlive = 2;
		SurvivalStipendIntervalSeconds = 60;
		MaxSurvivalStipendPerLife = 20;

		PvPDeathBaseGrit = 12;
		UnderdogKDPermille = 500;
		UnderdogMultiplierPercent = 150;
		DownstreakBonusPerDeath = 2;
		MaxDownstreakBonus = 30;

		MinAliveSecondsBeforeDeathCounts = 90;
		DeathConsolationCooldownSeconds = 45;
		DeathConsolationWindowSeconds = 600;
		MaxDeathConsolationsPerWindow = 8;
		FarmDecayWindowSeconds = 300;
		FarmDecayMinimumGrit = 3;

		MinPlayersForGrit = 4;
		MinDeathsToStartSurvivalStipend = 3;

		BoonsResetOnDeath = 1;
		IronWillResistancePercent = 25;
		RegenHealPerSecond = 2;
		LastStandTriggerHealth = 20;
		LastStandRestoreHealth = 50;

		Hotkey = "Y";

		Offers = new array<ref SecondWindShopOffer>;
		PopulateDefaultOffers();
	}

	void PopulateDefaultOffers()
	{
		if (!Offers)
			Offers = new array<ref SecondWindShopOffer>;

		Offers.Clear();

		AddWeaponOffer("scout_rifle", "Scout Rifle", "Bolt-action Mosin with a PU scope and 20 rounds.", "Mosin9130", "PUScopeOptic", "", "", 0, "Ammo_762x54", 20, 60);
		AddWeaponOffer("marksman_rifle", "Marksman Rifle", "Blaze 95 hunting rifle with optic and 20 rounds.", "B95", "HuntingOptic", "", "", 0, "Ammo_308Win", 20, 100);
		AddWeaponOffer("storm_carbine", "Storm Carbine", "AKM with PSO-1, handguard, 2 mags and 60 rounds.", "AKM", "PSO1Optic", "AK_PlasticHndgrd", "Mag_AKM_30Rnd", 2, "Ammo_762x39", 60, 140);
		AddWeaponOffer("spec_rifle", "Spec-Ops Rifle", "M4A1 with ACOG, handguard, 2 mags and 60 rounds.", "M4A1", "ACOGOptic", "M4_RISHndgrd", "Mag_STANAG_30Rnd", 2, "Ammo_556x45", 60, 200);
		AddWeaponOffer("backup_smg", "Backup SMG", "MP5K with reflex sight, 2 mags and 60 rounds.", "MP5K", "ReflexOptic", "", "Mag_MP5_30Rnd", 2, "Ammo_9x19", 60, 90);

		AddItemOffer("ammo_762x54", "7.62x54 Ammo", "Two 20-round boxes for the Mosin.", SW_CATEGORY_AMMO, "AmmoBox_762x54_20Rnd", 2, 25);
		AddItemOffer("ammo_762x39", "7.62x39 Ammo", "Two 20-round boxes for the AKM/SKS.", SW_CATEGORY_AMMO, "AmmoBox_762x39_20Rnd", 2, 25);
		AddItemOffer("ammo_556", "5.56 Ammo", "Two 20-round boxes for the M4A1.", SW_CATEGORY_AMMO, "AmmoBox_556x45_20Rnd", 2, 25);
		AddItemOffer("ammo_9x19", "9mm Ammo", "Two 25-round boxes for the MP5K.", SW_CATEGORY_AMMO, "AmmoBox_9x19_25Rnd", 2, 20);

		AddItemOffer("frag_grenade", "Frag Grenades", "Two RGD-5 fragmentation grenades.", SW_CATEGORY_EXPLOSIVE, "RGD5Grenade", 2, 30);
		AddItemOffer("land_mine", "Anti-Personnel Mine", "A pressure-activated land mine.", SW_CATEGORY_TRAP, "LandMineTrap", 1, 40);
		AddItemOffer("bear_trap", "Bear Trap", "A heavy jaw trap for locking down a doorway.", SW_CATEGORY_TRAP, "BearTrap", 1, 30);
		AddItemOffer("lockpick_set", "Lockpick Set", "Two lockpicks for securing or breaching doors.", SW_CATEGORY_UTILITY, "Lockpick", 2, 20);

		AddMedicOffer("field_medic_kit", "Field Medic Kit", "Three bandages, morphine and epinephrine.", 3, 25);
		AddItemOffer("adrenaline_shot", "Adrenaline Shot", "Epinephrine to jolt you back to your feet.", SW_CATEGORY_MEDICAL, "Epinephrine", 1, 15);

		AddBoonOffer("boon_iron_will", "Boon: Iron Will", "You shrug off a share of every hit. Lasts until you die.", SW_BOON_IRON_WILL, 150);
		AddBoonOffer("boon_second_breath", "Boon: Second Breath", "Your wounds slowly close over time. Lasts until you die.", SW_BOON_SECOND_WIND, 120);
		AddBoonOffer("boon_last_stand", "Boon: Last Stand", "Once, a killing blow leaves you standing at half health. Consumed on use.", SW_BOON_LAST_STAND, 180);
	}

	void AddWeaponOffer(string id, string name, string desc, string className, string optic, string handguard, string magClass, int magQty, string ammoClass, int ammoQty, int cost)
	{
		ref SecondWindShopOffer offer = new SecondWindShopOffer();
		offer.Id = id;
		offer.DisplayName = name;
		offer.Description = desc;
		offer.Category = SW_CATEGORY_WEAPON;
		offer.ClassName = className;
		offer.Quantity = 1;
		offer.Cost = cost;
		if (optic != "")
			offer.SpawnAttachments.Insert(optic);
		if (handguard != "")
			offer.SpawnAttachments.Insert(handguard);
		if (magClass != "" && magQty > 0)
			offer.ExtraItems.Insert(magClass + ":" + magQty.ToString());
		if (ammoClass != "" && ammoQty > 0)
			offer.ExtraItems.Insert(ammoClass + ":" + ammoQty.ToString());
		Offers.Insert(offer);
	}

	void AddItemOffer(string id, string name, string desc, string category, string className, int quantity, int cost)
	{
		ref SecondWindShopOffer offer = new SecondWindShopOffer();
		offer.Id = id;
		offer.DisplayName = name;
		offer.Description = desc;
		offer.Category = category;
		offer.ClassName = className;
		offer.Quantity = quantity;
		offer.Cost = cost;
		Offers.Insert(offer);
	}

	void AddMedicOffer(string id, string name, string desc, int bandageQty, int cost)
	{
		ref SecondWindShopOffer offer = new SecondWindShopOffer();
		offer.Id = id;
		offer.DisplayName = name;
		offer.Description = desc;
		offer.Category = SW_CATEGORY_MEDICAL;
		offer.ClassName = "BandageDressing";
		offer.Quantity = bandageQty;
		offer.Cost = cost;
		offer.ExtraItems.Insert("Morphine:1");
		offer.ExtraItems.Insert("Epinephrine:1");
		Offers.Insert(offer);
	}

	void AddBoonOffer(string id, string name, string desc, string boonId, int cost)
	{
		ref SecondWindShopOffer offer = new SecondWindShopOffer();
		offer.Id = id;
		offer.DisplayName = name;
		offer.Description = desc;
		offer.Category = SW_CATEGORY_BOON;
		offer.ClassName = "";
		offer.Quantity = 1;
		offer.Cost = cost;
		offer.BoonId = boonId;
		Offers.Insert(offer);
	}

	void EnsureDefaults()
	{
		if (SurvivalStipendIntervalSeconds < 1)
			SurvivalStipendIntervalSeconds = 60;
		if (MinAliveSecondsBeforeDeathCounts < 0)
			MinAliveSecondsBeforeDeathCounts = 0;
		if (DeathConsolationCooldownSeconds < 0)
			DeathConsolationCooldownSeconds = 0;
		if (DeathConsolationWindowSeconds < 1)
			DeathConsolationWindowSeconds = 600;
		if (MaxDeathConsolationsPerWindow < 1)
			MaxDeathConsolationsPerWindow = 1;
		if (FarmDecayWindowSeconds < 0)
			FarmDecayWindowSeconds = 0;
		if (FarmDecayMinimumGrit < 1)
			FarmDecayMinimumGrit = 1;
		if (MinPlayersForGrit < 1)
			MinPlayersForGrit = 1;
		if (MinDeathsToStartSurvivalStipend < 0)
			MinDeathsToStartSurvivalStipend = 0;
		if (IronWillResistancePercent < 0)
			IronWillResistancePercent = 0;
		if (IronWillResistancePercent > 90)
			IronWillResistancePercent = 90;
		if (RegenHealPerSecond < 0)
			RegenHealPerSecond = 0;
		if (LastStandTriggerHealth < 1)
			LastStandTriggerHealth = 1;
		if (LastStandRestoreHealth <= LastStandTriggerHealth)
			LastStandRestoreHealth = LastStandTriggerHealth + 20;

		Hotkey.Trim();
		Hotkey.ToUpper();
		if (Hotkey == "")
			Hotkey = "Y";

		if (!Offers || Offers.Count() == 0)
			PopulateDefaultOffers();

		for (int i = 0; i < Offers.Count(); i++)
		{
			SecondWindShopOffer offer = Offers.Get(i);
			if (offer)
				offer.EnsureArrays();
		}
	}

	static void EnsureFolders()
	{
		MakeDirectory(SW_PROFILE_ROOT);
		MakeDirectory(SW_PROFILE_DB_DIR);
		MakeDirectory(SW_PROFILE_SETTINGS_DIR);
	}

	static ref SecondWindSettings Load()
	{
		EnsureFolders();

		ref SecondWindSettings settings = new SecondWindSettings();
		if (FileExist(SW_SETTINGS_FILE))
		{
			// The constructor pre-populates Offers; clear it before loading so a
			// JSON load can never duplicate the default list.
			if (settings.Offers)
				settings.Offers.Clear();
			JsonFileLoader<SecondWindSettings>.JsonLoadFile(SW_SETTINGS_FILE, settings);
		}

		settings.EnsureDefaults();
		JsonFileLoader<SecondWindSettings>.JsonSaveFile(SW_SETTINGS_FILE, settings);
		Print("[UnderdogLeveler] Settings written to " + SW_SETTINGS_FILE);
		return settings;
	}

	void Save()
	{
		EnsureDefaults();
		EnsureFolders();
		JsonFileLoader<SecondWindSettings>.JsonSaveFile(SW_SETTINGS_FILE, this);
	}

	SecondWindShopOffer FindOffer(string offerId)
	{
		if (offerId == "" || !Offers)
			return null;

		for (int i = 0; i < Offers.Count(); i++)
		{
			SecondWindShopOffer offer = Offers.Get(i);
			if (offer && offer.Id == offerId)
				return offer;
		}

		return null;
	}
}
