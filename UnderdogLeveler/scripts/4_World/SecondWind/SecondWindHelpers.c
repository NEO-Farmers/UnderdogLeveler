class SecondWindHelpers
{
	static string SanitizeCell(string value)
	{
		value.Replace("\t", " ");
		value.Replace("\n", " ");
		value.Replace("\r", " ");
		return value;
	}

	static PlayerBase GetSourcePlayerFromObject(Object source)
	{
		if (!source)
			return null;

		PlayerBase player = PlayerBase.Cast(source);
		if (player)
			return player;

		EntityAI entity = EntityAI.Cast(source);
		if (!entity)
			return null;

		player = PlayerBase.Cast(entity.GetHierarchyRootPlayer());
		if (player)
			return player;

		return null;
	}

	static bool TypeContainsToken(string typeName, array<string> tokens)
	{
		if (typeName == "" || !tokens)
			return false;

		string lowerType = typeName;
		lowerType.ToLower();

		for (int i = 0; i < tokens.Count(); i++)
		{
			string token = tokens.Get(i);
			token.Trim();
			if (token == "")
				continue;

			if (typeName == token)
				return true;

			string lowerToken = token;
			lowerToken.ToLower();

			if (lowerType.Contains(lowerToken))
				return true;
		}

		return false;
	}

	static bool IsZombieClass(EntityAI entity)
	{
		if (!entity)
			return false;

		array<string> tokens = {"zombie", "infected", "Zmb_"};
		return TypeContainsToken(entity.GetType(), tokens);
	}

	static bool IsAnimalClass(EntityAI entity)
	{
		if (!entity)
			return false;

		array<string> tokens = {"animal_", "Animal_", "AmbientAnimal", "CanisLupus", "UrsusArctos", "SusScrofa", "CervusElaphus", "CapreolusCapreolus", "OvisAries", "GallusGallus", "VulpesVulpes"};
		return TypeContainsToken(entity.GetType(), tokens);
	}

	static int ClampInt(int value, int minValue, int maxValue)
	{
		if (value < minValue)
			return minValue;
		if (value > maxValue)
			return maxValue;
		return value;
	}

	static int BoolToInt(bool value)
	{
		if (value)
			return 1;
		return 0;
	}

	static bool IsHeadZone(string dmgZone)
	{
		string zone = dmgZone;
		zone.ToLower();
		return zone.Contains("head");
	}

	static string FormatFloat(float value, int decimals)
	{
		string s = value.ToString();
		int dot = s.IndexOf(".");
		if (dot == -1)
			return s;

		int targetLen = dot + 1 + decimals;
		if (targetLen < s.Length())
			s = s.Substring(0, targetLen);

		return s;
	}
}
