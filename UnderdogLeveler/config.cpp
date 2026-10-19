class CfgPatches
{
	class UnderdogLeveler
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data", "JM_CF_Scripts"};
	};
};

class CfgMods
{
	class UnderdogLeveler
	{
		type = "mod";
		dir = "UnderdogLeveler";
		name = "Underdog Leveler";
		tooltip = "Underdog Leveler [Killfeed | Leaderboard | Storefront]";
		overview = "Helps struggling deathmatch players earn Grit and requisition gear, life-saving boons, and a comeback from the Field Report.";
		author = "Underdog Leveler Team";
		version = "1.0.0";
		hideName = 0;
		hidePicture = 0;
		inputs = "UnderdogLeveler/data/inputs.xml";
		dependencies[] = {"Game", "World", "Mission"};
		class defs
		{
			class gameScriptModule
			{
				value = "";
				files[] = {"UnderdogLeveler/scripts/3_Game"};
			};
			class worldScriptModule
			{
				value = "";
				files[] = {"UnderdogLeveler/scripts/4_World"};
			};
			class missionScriptModule
			{
				value = "";
				files[] = {"UnderdogLeveler/scripts/5_Mission"};
			};
		};
	};
};
