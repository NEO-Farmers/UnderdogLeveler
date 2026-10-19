modded class MissionServer
{
	override void OnInit()
	{
		super.OnInit();
		SecondWindServer.GetInstance().ReloadSettings();
		Print("[UnderdogLeveler] MissionServer.OnInit complete.");
	}

	override void TickScheduler(float timeslice)
	{
		super.TickScheduler(timeslice);
		SecondWindServer.GetInstance().Tick();
		FlushSecondWindNotifications();
	}

	override void InvokeOnConnect(PlayerBase player, PlayerIdentity identity)
	{
		super.InvokeOnConnect(player, identity);

		if (GetGame().IsServer() && player && identity)
		{
			SecondWindServer.GetInstance().EnsurePlayer(player);
			SendSecondWindSettings(player);
		}
	}

	override void OnMissionFinish()
	{
		SecondWindServer.GetInstance().SaveAll();
		super.OnMissionFinish();
	}
}
