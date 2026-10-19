class SWLeaderboardEntry
{
	bool IsSelf;
	string Name;
	int Kills;
	int Deaths;
	string Kd;
	int Headshots;
	string Longest;

	void SWLeaderboardEntry()
	{
		IsSelf = false;
		Name = "";
		Kills = 0;
		Deaths = 0;
		Kd = "";
		Headshots = 0;
		Longest = "";
	}
}

class SecondWindStatsMenu extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected TextWidget m_BodyText;
	protected Widget m_StoreButton;
	protected Widget m_LeaderboardHost;
	protected ref array<Widget> m_LeaderboardRows;
	protected ref array<ref SWLeaderboardEntry> m_LeaderboardEntries;
	protected bool m_IsOpen;
	protected bool m_InputLockActive;

	void SecondWindStatsMenu()
	{
		m_IsOpen = false;
		m_InputLockActive = false;
		m_LeaderboardRows = new array<Widget>;
		m_LeaderboardEntries = new array<ref SWLeaderboardEntry>;
	}

	void EnsureLayout()
	{
		if (m_Root)
			return;

		m_Root = GetGame().GetWorkspace().CreateWidgets(SW_LAYOUT_STATS);
		if (!m_Root)
			return;

		m_BodyText = TextWidget.Cast(m_Root.FindAnyWidget("SWStatsBodyText"));
		m_StoreButton = m_Root.FindAnyWidget("SWStatsStoreButton");
		m_LeaderboardHost = m_Root.FindAnyWidget("SWLeaderboardHost");

		m_Root.SetHandler(this);
		if (m_StoreButton)
			m_StoreButton.SetHandler(this);

		ApplyStyle();
	}

	// Colors are applied in code (SetColor) so they reliably override the
	// DayZDefaultPanel style and any layout default text color.
	protected void ApplyStyle()
	{
		if (!m_Root)
			return;

		Widget tint = m_Root.FindAnyWidget("SWStatsScreenTint");
		Widget background = m_Root.FindAnyWidget("SWStatsBackground");
		Widget bodyHost = m_Root.FindAnyWidget("SWStatsBodyHost");
		Widget header = m_Root.FindAnyWidget("SWLeaderboardHeader");
		Widget storeButton = m_Root.FindAnyWidget("SWStatsStoreButton");
		Widget exitButton = m_Root.FindAnyWidget("SWStatsExitButton");
		TextWidget title = TextWidget.Cast(m_Root.FindAnyWidget("SWStatsTitleText"));
		TextWidget body = TextWidget.Cast(m_Root.FindAnyWidget("SWStatsBodyText"));
		TextWidget storeText = TextWidget.Cast(m_Root.FindAnyWidget("SWStatsStoreText"));
		TextWidget exitText = TextWidget.Cast(m_Root.FindAnyWidget("SWStatsExitText"));

		if (tint) tint.SetColor(ARGB(255, 0, 0, 0));
		if (background) background.SetColor(ARGB(255, 6, 9, 11));
		if (bodyHost) bodyHost.SetColor(ARGB(255, 4, 6, 8));
		if (header) header.SetColor(ARGB(255, 20, 28, 34));
		if (m_LeaderboardHost) m_LeaderboardHost.SetColor(ARGB(255, 4, 6, 8));
		if (storeButton) storeButton.SetColor(ARGB(255, 28, 38, 46));
		if (exitButton) exitButton.SetColor(ARGB(255, 28, 38, 46));

		if (title) title.SetColor(ARGB(255, 88, 200, 230));
		if (body) body.SetColor(ARGB(255, 230, 236, 242));
		if (storeText) storeText.SetColor(ARGB(255, 230, 230, 230));
		if (exitText) exitText.SetColor(ARGB(255, 230, 230, 230));

		// Header column labels.
		TextWidget hRank = TextWidget.Cast(m_Root.FindAnyWidget("SWLbHeaderRank"));
		TextWidget hName = TextWidget.Cast(m_Root.FindAnyWidget("SWLbHeaderName"));
		TextWidget hKills = TextWidget.Cast(m_Root.FindAnyWidget("SWLbHeaderKills"));
		TextWidget hDeaths = TextWidget.Cast(m_Root.FindAnyWidget("SWLbHeaderDeaths"));
		TextWidget hKd = TextWidget.Cast(m_Root.FindAnyWidget("SWLbHeaderKd"));
		TextWidget hLongest = TextWidget.Cast(m_Root.FindAnyWidget("SWLbHeaderLongest"));
		if (hRank) hRank.SetColor(ARGB(255, 150, 165, 180));
		if (hName) hName.SetColor(ARGB(255, 150, 165, 180));
		if (hKills) hKills.SetColor(ARGB(255, 150, 165, 180));
		if (hDeaths) hDeaths.SetColor(ARGB(255, 150, 165, 180));
		if (hKd) hKd.SetColor(ARGB(255, 150, 165, 180));
		if (hLongest) hLongest.SetColor(ARGB(255, 150, 165, 180));
	}

	protected void ApplyOverlayPriority()
	{
		if (!m_Root)
			return;

		m_Root.SetSort(501, true);
		m_Root.SetFlags(m_Root.GetFlags() | WidgetFlags.RENDER_ALWAYS, true);
		SetFocus(m_Root);
	}

	protected void ClearOverlayPriority()
	{
		if (!m_Root)
			return;

		if (GetGame() && GetGame().GetWorkspace())
		{
			SetFocus(null);
		}
	}

	protected void ApplyInputLock(bool state)
	{
		MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
		if (!mission)
			return;

		if (state)
		{
			if (m_InputLockActive)
				return;

			mission.AddActiveInputExcludes({"inventory"});
			mission.AddActiveInputRestriction(EInputRestrictors.INVENTORY);
			m_InputLockActive = true;
		}
		else
		{
			if (!m_InputLockActive)
				return;

			mission.RemoveActiveInputExcludes({"inventory"}, false);
			mission.RemoveActiveInputRestriction(EInputRestrictors.INVENTORY);
			m_InputLockActive = false;
		}
	}

	bool IsOpen()
	{
		return m_IsOpen;
	}

	void SetStats(string text)
	{
		if (m_BodyText)
			m_BodyText.SetText(text);
	}

	void ApplyLeaderboard(string body)
	{
		ref array<ref SWLeaderboardEntry> entries = new array<ref SWLeaderboardEntry>;

		TStringArray lines = new TStringArray;
		body.Split("\n", lines);

		for (int i = 0; i < lines.Count(); i++)
		{
			string line = lines.Get(i);
			if (line == "")
				continue;

			TStringArray parts = new TStringArray;
			line.Split("\t", parts);
			if (parts.Count() < 7)
				continue;

			ref SWLeaderboardEntry entry = new SWLeaderboardEntry();
			entry.IsSelf = (parts.Get(0).ToInt() == 1);
			entry.Name = parts.Get(1);
			entry.Kills = parts.Get(2).ToInt();
			entry.Deaths = parts.Get(3).ToInt();
			entry.Kd = parts.Get(4);
			entry.Headshots = parts.Get(5).ToInt();
			entry.Longest = parts.Get(6);
			entries.Insert(entry);
		}

		m_LeaderboardEntries = entries;
		RebuildLeaderboard();
	}

	void ClearLeaderboardRows()
	{
		m_LeaderboardRows.Clear();

		if (!m_LeaderboardHost)
			return;

		Widget child = m_LeaderboardHost.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			child.Unlink();
			child = next;
		}
	}

	void RebuildLeaderboard()
	{
		ClearLeaderboardRows();

		if (!m_LeaderboardHost || !m_LeaderboardEntries)
			return;

		float y = 0.0;
		for (int i = 0; i < m_LeaderboardEntries.Count(); i++)
		{
			SWLeaderboardEntry entry = m_LeaderboardEntries.Get(i);
			if (!entry)
				continue;

			Widget row = GetGame().GetWorkspace().CreateWidgets(SW_LAYOUT_LEADERBOARD_ROW, m_LeaderboardHost);
			if (!row)
				continue;

			row.SetName(string.Format("SWLbRow_%1", i));
			row.SetPos(0.0, y);
			row.SetSize(1.0, 0.04);
			m_LeaderboardRows.Insert(row);
			y = y + 0.04;

			TextWidget rankText = TextWidget.Cast(row.FindAnyWidget("SWLbRankText"));
			TextWidget nameText = TextWidget.Cast(row.FindAnyWidget("SWLbNameText"));
			TextWidget killsText = TextWidget.Cast(row.FindAnyWidget("SWLbKillsText"));
			TextWidget deathsText = TextWidget.Cast(row.FindAnyWidget("SWLbDeathsText"));
			TextWidget kdText = TextWidget.Cast(row.FindAnyWidget("SWLbKdText"));
			TextWidget longestText = TextWidget.Cast(row.FindAnyWidget("SWLbLongestText"));
			Widget accent = row.FindAnyWidget("SWLbAccent");

			if (rankText) rankText.SetText((i + 1).ToString());
			if (nameText) nameText.SetText(entry.Name);
			if (killsText) killsText.SetText(entry.Kills.ToString());
			if (deathsText) deathsText.SetText(entry.Deaths.ToString());
			if (kdText) kdText.SetText(entry.Kd);
			if (longestText) longestText.SetText(entry.Longest + "m");

			row.SetColor(ARGB(255, 10, 13, 16));
			if (accent) accent.SetColor(ARGB(255, 40, 60, 72));

			if (rankText) rankText.SetColor(ARGB(255, 255, 255, 255));
			if (nameText) nameText.SetColor(ARGB(255, 230, 236, 242));
			if (killsText) killsText.SetColor(ARGB(255, 230, 236, 242));
			if (deathsText) deathsText.SetColor(ARGB(255, 230, 236, 242));
			if (kdText) kdText.SetColor(ARGB(255, 230, 236, 242));
			if (longestText) longestText.SetColor(ARGB(255, 230, 236, 242));

			if (entry.IsSelf)
			{
				row.SetColor(ARGB(255, 14, 24, 30));
				if (rankText) rankText.SetColor(ARGB(255, 88, 200, 230));
				if (nameText) nameText.SetColor(ARGB(255, 88, 200, 230));
				if (accent) accent.SetColor(ARGB(255, 88, 200, 230));
			}
		}
	}

	void Show()
	{
		EnsureLayout();
		if (!m_Root)
			return;

		ApplyOverlayPriority();
		m_Root.Show(true);
		m_IsOpen = true;

		if (m_BodyText)
			m_BodyText.SetText("Loading stats...");

		GetGame().GetUIManager().ShowCursor(true);
		SetFocus(m_Root);
		ApplyInputLock(true);
	}

	void Hide()
	{
		if (!m_Root)
			return;

		m_Root.Show(false);
		m_IsOpen = false;
		ClearOverlayPriority();
		GetGame().GetUIManager().ShowCursor(false);
		SetFocus(null);
		ApplyInputLock(false);
		GetUApi().SupressNextFrame(true);
	}

	void RefreshInputContext()
	{
		if (!m_IsOpen || !m_Root)
			return;

		ApplyOverlayPriority();
		GetGame().GetUIManager().ShowCursor(true);
		SetFocus(m_Root);
		ApplyInputLock(true);
		GetUApi().SupressNextFrame(true);
	}

	protected int GetButtonFromWidget(Widget w)
	{
		Widget current = w;
		while (current)
		{
			string name = current.GetName();
			if (name == "SWStatsStoreButton" || name == "SWStatsStoreText")
				return 1;
			if (name == "SWStatsExitButton" || name == "SWStatsExitText")
				return 2;

			current = current.GetParent();
		}

		return 0;
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;

		return OnClick(w, x, y, button);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int which = GetButtonFromWidget(w);
		MissionBase mission = MissionBase.Cast(GetGame().GetMission());

		if (which == 1)
		{
			if (mission)
				mission.OpenSWShopFromStats();
			return true;
		}

		if (which == 2)
		{
			if (mission)
				mission.ExitSWStats();
			return true;
		}

		return false;
	}
}
