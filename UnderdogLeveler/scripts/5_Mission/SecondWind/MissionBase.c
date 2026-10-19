modded class MissionBase
{
	protected ref SecondWindShopMenu m_SWShopMenu;
	protected ref SecondWindStatsMenu m_SWStatsMenu;
	protected int m_SWGrit;
	protected bool m_SWEnabled;
	protected string m_SWHotkey;
	protected bool m_SWHasSettings;

	protected int m_SWPendingShopTotalChunks;
	protected int m_SWPendingShopReceived;
	protected int m_SWPendingShopGrit;
	protected string m_SWPendingShopBody;
	protected ref array<string> m_SWPendingShopChunks;

	protected int m_SWLeaderboardTotalChunks;
	protected int m_SWLeaderboardReceived;
	protected ref array<string> m_SWLeaderboardChunks;

	void MissionBase()
	{
		GetRPCManager().AddRPC(SW_MOD_NAME, "ServerHandleShopDataRequest", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ServerHandlePurchaseRequest", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ClientReceiveSettings", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ClientReceiveNotification", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ClientReceiveShopDataMeta", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ClientReceiveShopDataChunk", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ClientReceivePurchaseResult", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ServerHandleStatsRequest", this, SingleplayerExecutionType.Server);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ClientReceiveStatsData", this, SingleplayerExecutionType.Client);
		GetRPCManager().AddRPC(SW_MOD_NAME, "ClientReceiveLeaderboardChunk", this, SingleplayerExecutionType.Client);

		m_SWGrit = 0;
		m_SWEnabled = true;
		m_SWHotkey = "Y";
		m_SWHasSettings = false;
		m_SWPendingShopTotalChunks = 0;
		m_SWPendingShopReceived = 0;
		m_SWPendingShopGrit = 0;
		m_SWPendingShopBody = "";
		m_SWPendingShopChunks = new array<string>;
		m_SWLeaderboardTotalChunks = 0;
		m_SWLeaderboardReceived = 0;
		m_SWLeaderboardChunks = new array<string>;
	}

	bool IsSWEnabled()
	{
		return m_SWEnabled;
	}

	string GetSWHotkey()
	{
		string hotkey = m_SWHotkey;
		hotkey.Trim();
		hotkey.ToUpper();
		if (hotkey == "")
			hotkey = "Y";
		return hotkey;
	}

	int GetSWGrit()
	{
		return m_SWGrit;
	}

	// ------------------------------------------------------------------ shop open / close / requests

	void ToggleSWShop()
	{
		if (!m_SWShopMenu)
			m_SWShopMenu = new SecondWindShopMenu();

		if (!m_SWShopMenu.IsOpen())
		{
			m_SWShopMenu.Show();
			RequestSWShopData();
		}
		else
		{
			m_SWShopMenu.Hide();
		}
	}

	void ExitSWShop()
	{
		if (m_SWShopMenu && m_SWShopMenu.IsOpen())
			m_SWShopMenu.Hide();
	}

	void RequestSWShopData()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		GetRPCManager().SendRPC(SW_MOD_NAME, "ServerHandleShopDataRequest", null, true, null, player);
	}

	void RequestSWPurchase(string offerId)
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player || offerId == "")
			return;

		Param1<string> payload = new Param1<string>(offerId);
		GetRPCManager().SendRPC(SW_MOD_NAME, "ServerHandlePurchaseRequest", payload, true, null, player);
	}

	// ------------------------------------------------------------------ stats open / close / requests

	void ToggleSWStats()
	{
		if (!m_SWStatsMenu)
			m_SWStatsMenu = new SecondWindStatsMenu();

		if (!m_SWStatsMenu.IsOpen())
		{
			m_SWStatsMenu.Show();
			RequestSWStats();
		}
		else
		{
			m_SWStatsMenu.Hide();
		}
	}

	void ExitSWStats()
	{
		if (m_SWStatsMenu && m_SWStatsMenu.IsOpen())
			m_SWStatsMenu.Hide();
	}

	void RequestSWStats()
	{
		PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());
		if (!player)
			return;

		GetRPCManager().SendRPC(SW_MOD_NAME, "ServerHandleStatsRequest", null, true, null, player);
	}

	void OpenSWShopFromStats()
	{
		ExitSWStats();
		ToggleSWShop();
	}

	// Return from the shop back to the Field Report (stats + leaderboard).
	void BackFromSWShop()
	{
		ExitSWShop();
		ToggleSWStats();
	}

	// ------------------------------------------------------------------ hotkey helpers

	protected string SWKeyCodeToName(int key)
	{
		switch (key)
		{
			case KeyCode.KC_A: return "A";
			case KeyCode.KC_B: return "B";
			case KeyCode.KC_C: return "C";
			case KeyCode.KC_D: return "D";
			case KeyCode.KC_E: return "E";
			case KeyCode.KC_F: return "F";
			case KeyCode.KC_G: return "G";
			case KeyCode.KC_H: return "H";
			case KeyCode.KC_I: return "I";
			case KeyCode.KC_J: return "J";
			case KeyCode.KC_K: return "K";
			case KeyCode.KC_L: return "L";
			case KeyCode.KC_M: return "M";
			case KeyCode.KC_N: return "N";
			case KeyCode.KC_O: return "O";
			case KeyCode.KC_P: return "P";
			case KeyCode.KC_Q: return "Q";
			case KeyCode.KC_R: return "R";
			case KeyCode.KC_S: return "S";
			case KeyCode.KC_T: return "T";
			case KeyCode.KC_U: return "U";
			case KeyCode.KC_V: return "V";
			case KeyCode.KC_W: return "W";
			case KeyCode.KC_X: return "X";
			case KeyCode.KC_Y: return "Y";
			case KeyCode.KC_Z: return "Z";
			case KeyCode.KC_F1: return "F1";
			case KeyCode.KC_F2: return "F2";
			case KeyCode.KC_F3: return "F3";
			case KeyCode.KC_F4: return "F4";
			case KeyCode.KC_F5: return "F5";
			case KeyCode.KC_F6: return "F6";
			case KeyCode.KC_F7: return "F7";
			case KeyCode.KC_F8: return "F8";
			case KeyCode.KC_F9: return "F9";
			case KeyCode.KC_F10: return "F10";
			case KeyCode.KC_F11: return "F11";
			case KeyCode.KC_F12: return "F12";
		}
		return "";
	}

	protected bool SWKeyMatchesHotkey(int key)
	{
		string expected = GetSWHotkey();
		string actual = SWKeyCodeToName(key);
		if (actual == "")
			return false;
		return (actual == expected || ("KC_" + actual) == expected);
	}

	// ------------------------------------------------------------------ server -> client RPC handlers

	void ClientReceiveSettings(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<string, int> data;
		if (!ctx.Read(data))
			return;

		m_SWHotkey = data.param1;
		m_SWEnabled = (data.param2 != 0);
		m_SWHasSettings = true;
	}

	void ClientReceiveNotification(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> data;
		if (!ctx.Read(data))
			return;

		NotificationSystem.AddNotificationExtended(4.0, "Underdog Leveler", data.param1, "set:dayz_gui image:icon_info");
	}

	void ClientReceiveShopDataMeta(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param2<int, int> data;
		if (!ctx.Read(data))
			return;

		m_SWPendingShopTotalChunks = Math.Max(1, data.param2);
		m_SWPendingShopReceived = 0;
		m_SWPendingShopGrit = data.param1;
		m_SWPendingShopBody = "";

		if (!m_SWPendingShopChunks)
			m_SWPendingShopChunks = new array<string>;
		else
			m_SWPendingShopChunks.Clear();

		for (int i = 0; i < m_SWPendingShopTotalChunks; i++)
			m_SWPendingShopChunks.Insert("");
	}

	void ClientReceiveShopDataChunk(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<int, int, string> data;
		if (!ctx.Read(data))
			return;

		if (!m_SWPendingShopChunks || m_SWPendingShopChunks.Count() != m_SWPendingShopTotalChunks)
		{
			m_SWPendingShopChunks = new array<string>;
			m_SWPendingShopTotalChunks = Math.Max(1, data.param2);
			for (int i = 0; i < m_SWPendingShopTotalChunks; i++)
				m_SWPendingShopChunks.Insert("");
			m_SWPendingShopReceived = 0;
		}

		if (data.param1 < 0 || data.param1 >= m_SWPendingShopChunks.Count())
			return;

		if (m_SWPendingShopChunks.Get(data.param1) == "")
			m_SWPendingShopReceived++;

		m_SWPendingShopChunks.Set(data.param1, data.param3);

		if (m_SWPendingShopReceived >= m_SWPendingShopTotalChunks)
		{
			string body = "";
			for (int j = 0; j < m_SWPendingShopChunks.Count(); j++)
				body = body + m_SWPendingShopChunks.Get(j);

			m_SWGrit = m_SWPendingShopGrit;
			ApplySWShopData(body);
		}
	}

	void ClientReceivePurchaseResult(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<bool, string, int> data;
		if (!ctx.Read(data))
			return;

		m_SWGrit = data.param3;

		if (m_SWShopMenu)
		{
			m_SWShopMenu.UpdateGrit(m_SWGrit);
			m_SWShopMenu.SetStatus(data.param2);
			m_SWShopMenu.RefreshRows();
		}

		if (data.param1)
			NotificationSystem.AddNotificationExtended(4.0, "Underdog Leveler", data.param2, "set:dayz_gui image:icon_info");
		else
			NotificationSystem.AddNotificationExtended(6.0, "Underdog Leveler", data.param2, "set:dayz_gui image:icon_info");
	}

	// ------------------------------------------------------------------ shop data assembly (client)

	void ApplySWShopData(string body)
	{
		if (!m_SWShopMenu)
			m_SWShopMenu = new SecondWindShopMenu();

		ref array<ref SWShopOffer> offers = new array<ref SWShopOffer>;

		TStringArray lines = new TStringArray;
		body.Split("\n", lines);

		for (int i = 0; i < lines.Count(); i++)
		{
			string line = lines.Get(i);
			if (line == "")
				continue;

			TStringArray parts = new TStringArray;
			line.Split("\t", parts);
			if (parts.Count() < 5)
				continue;

			ref SWShopOffer offer = new SWShopOffer();
			offer.Id = parts.Get(0);
			offer.DisplayName = parts.Get(1);
			offer.Description = parts.Get(2);
			offer.Cost = parts.Get(3).ToInt();
			offer.Category = parts.Get(4);
			offers.Insert(offer);
		}

		m_SWShopMenu.SetOffers(offers);
		m_SWShopMenu.UpdateGrit(m_SWGrit);
		m_SWShopMenu.Rebuild();
	}

	// ------------------------------------------------------------------ client -> server RPC handlers

	void ServerHandleShopDataRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity())
			return;

		SecondWindServer server = SecondWindServer.GetInstance();
		server.EnsurePlayer(player);

		string body = server.BuildShopText();
		int chunkSize = 700;
		int bodyLength = body.Length();
		int totalChunks = 1;
		Print("[UnderdogLeveler] Shop data requested. bodyLength=" + bodyLength.ToString());

		if (bodyLength > 0)
			totalChunks = Math.Ceil(bodyLength / (chunkSize * 1.0));

		Param2<int, int> meta = new Param2<int, int>(server.GetGrit(player), totalChunks);
		GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveShopDataMeta", meta, true, player.GetIdentity(), null);

		if (bodyLength <= 0)
		{
			Param3<int, int, string> emptyChunk = new Param3<int, int, string>(0, 1, "");
			GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveShopDataChunk", emptyChunk, true, player.GetIdentity(), null);
			return;
		}

		int offset = 0;
		int chunkIndex = 0;
		while (offset < bodyLength)
		{
			int remaining = bodyLength - offset;
			int take = chunkSize;
			if (remaining < take)
				take = remaining;

			string chunk = body.Substring(offset, take);
			Param3<int, int, string> payload = new Param3<int, int, string>(chunkIndex, totalChunks, chunk);
			GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveShopDataChunk", payload, true, player.GetIdentity(), null);

			offset = offset + take;
			chunkIndex++;
		}
	}

	void ServerHandlePurchaseRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity())
			return;

		Param1<string> data;
		if (!ctx.Read(data))
			return;

		string resultMessage = "Purchase failed.";
		bool success = SecondWindServer.GetInstance().PurchaseOffer(player, data.param1, resultMessage);

		Param3<bool, string, int> response = new Param3<bool, string, int>(success, resultMessage, SecondWindServer.GetInstance().GetGrit(player));
		GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceivePurchaseResult", response, true, player.GetIdentity(), null);
	}

	void ServerHandleStatsRequest(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		PlayerBase player = PlayerBase.Cast(target);
		if (!player || !player.GetIdentity())
			return;

		SecondWindServer server = SecondWindServer.GetInstance();
		server.EnsurePlayer(player);

		int rank = server.GetLeaderboardRank(player);
		string statsBody = server.BuildStatsText(player, rank);
		Print("[UnderdogLeveler] Stats requested. rank=" + rank.ToString());
		Param1<string> statsPayload = new Param1<string>(statsBody);
		GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveStatsData", statsPayload, true, player.GetIdentity(), null);

		string lbBody = server.BuildLeaderboardText(player);
		Print("[UnderdogLeveler] Leaderboard requested. bodyLength=" + lbBody.Length().ToString());
		SendLeaderboardChunks(player, lbBody);
	}

	protected void SendLeaderboardChunks(PlayerBase player, string body)
	{
		int chunkSize = 700;
		int bodyLength = body.Length();
		int totalChunks = 1;
		if (bodyLength > 0)
			totalChunks = Math.Ceil(bodyLength / (chunkSize * 1.0));

		if (bodyLength <= 0)
		{
			Param3<int, int, string> emptyChunk = new Param3<int, int, string>(0, 1, "");
			GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveLeaderboardChunk", emptyChunk, true, player.GetIdentity(), null);
			return;
		}

		int offset = 0;
		int chunkIndex = 0;
		while (offset < bodyLength)
		{
			int remaining = bodyLength - offset;
			int take = chunkSize;
			if (remaining < take)
				take = remaining;

			string chunk = body.Substring(offset, take);
			Param3<int, int, string> payload = new Param3<int, int, string>(chunkIndex, totalChunks, chunk);
			GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveLeaderboardChunk", payload, true, player.GetIdentity(), null);

			offset = offset + take;
			chunkIndex++;
		}
	}

	void ClientReceiveStatsData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param1<string> data;
		if (!ctx.Read(data))
			return;

		Print("[UnderdogLeveler] Client received stats. body=" + data.param1);

		if (!m_SWStatsMenu)
			m_SWStatsMenu = new SecondWindStatsMenu();

		m_SWStatsMenu.SetStats(data.param1);
	}

	void ClientReceiveLeaderboardChunk(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
	{
		Param3<int, int, string> data;
		if (!ctx.Read(data))
			return;

		if (!m_SWLeaderboardChunks || m_SWLeaderboardChunks.Count() != m_SWLeaderboardTotalChunks)
		{
			m_SWLeaderboardChunks = new array<string>;
			m_SWLeaderboardTotalChunks = Math.Max(1, data.param2);
			for (int i = 0; i < m_SWLeaderboardTotalChunks; i++)
				m_SWLeaderboardChunks.Insert("");
			m_SWLeaderboardReceived = 0;
		}

		if (data.param1 < 0 || data.param1 >= m_SWLeaderboardChunks.Count())
			return;

		if (m_SWLeaderboardChunks.Get(data.param1) == "")
			m_SWLeaderboardReceived++;

		m_SWLeaderboardChunks.Set(data.param1, data.param3);

		if (m_SWLeaderboardReceived >= m_SWLeaderboardTotalChunks)
		{
			string body = "";
			for (int j = 0; j < m_SWLeaderboardChunks.Count(); j++)
				body = body + m_SWLeaderboardChunks.Get(j);

			Print("[UnderdogLeveler] Client received leaderboard. bodyLength=" + body.Length().ToString());

			if (!m_SWStatsMenu)
				m_SWStatsMenu = new SecondWindStatsMenu();

			m_SWStatsMenu.ApplyLeaderboard(body);
		}
	}

	protected void SendSecondWindSettings(PlayerBase player)
	{
		if (!player || !player.GetIdentity())
			return;

		SecondWindServer server = SecondWindServer.GetInstance();
		Param2<string, int> payload = new Param2<string, int>(server.GetHotkey(), SecondWindHelpers.BoolToInt(server.IsEnabled()));
		GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveSettings", payload, true, player.GetIdentity(), player);
	}

	protected void FlushSecondWindNotifications()
	{
		SecondWindServer server = SecondWindServer.GetInstance();
		ref map<string, ref array<string>> pending = server.TakePendingNotifications();

		if (!pending || pending.Count() == 0)
			return;

		foreach (string playerId, array<string> messages : pending)
		{
			if (!messages || messages.Count() == 0)
				continue;

			PlayerBase player = server.FindOnlinePlayerById(playerId);
			if (!player || !player.GetIdentity())
				continue;

			for (int i = 0; i < messages.Count(); i++)
			{
				Param1<string> msg = new Param1<string>(messages.Get(i));
				GetRPCManager().SendRPC(SW_MOD_NAME, "ClientReceiveNotification", msg, true, player.GetIdentity(), player);
			}
		}
	}
}
