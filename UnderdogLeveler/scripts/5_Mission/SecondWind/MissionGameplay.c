modded class MissionGameplay
{
	protected int m_SWLastToggleAt;

	protected bool IsTypingInChat()
	{
		Widget focus = GetFocus();
		if (!focus)
			return false;

		string name = focus.GetName();
		name.ToLower();

		if (name.Contains("chat"))
			return true;
		if (EditBoxWidget.Cast(focus))
			return true;
		if (MultilineEditBoxWidget.Cast(focus))
			return true;

		return false;
	}

	protected bool SWIsThrottled()
	{
		if (!GetGame())
			return false;

		int now = GetGame().GetTime();
		if (now - m_SWLastToggleAt < 250)
			return true;

		m_SWLastToggleAt = now;
		return false;
	}

	override void OnUpdate(float timeslice)
	{
		super.OnUpdate(timeslice);

		if (m_SWStatsMenu && m_SWStatsMenu.IsOpen())
			m_SWStatsMenu.RefreshInputContext();
		else if (m_SWShopMenu && m_SWShopMenu.IsOpen())
			m_SWShopMenu.RefreshInputContext();
	}

	override void OnKeyPress(int key)
	{
		if (m_SWStatsMenu && m_SWStatsMenu.IsOpen())
		{
			if (key == KeyCode.KC_ESCAPE)
			{
				ExitSWStats();
				GetUApi().SupressNextFrame(true);
				return;
			}

			if (key == KeyCode.KC_B)
			{
				OpenSWShopFromStats();
				GetUApi().SupressNextFrame(true);
				return;
			}

			GetUApi().SupressNextFrame(true);
			return;
		}

		if (m_SWShopMenu && m_SWShopMenu.IsOpen())
		{
			if (key == KeyCode.KC_ESCAPE || key == KeyCode.KC_B)
			{
				BackFromSWShop();
				GetUApi().SupressNextFrame(true);
				return;
			}

			if (key >= KeyCode.KC_1 && key <= KeyCode.KC_9)
			{
				int shopIndex = key - KeyCode.KC_1;
				if (m_SWShopMenu.CanPurchaseIndex(shopIndex))
					RequestSWPurchase(m_SWShopMenu.GetOfferIdByIndex(shopIndex));
				return;
			}

			GetUApi().SupressNextFrame(true);
			return;
		}

		if (SWKeyMatchesHotkey(key) && !IsTypingInChat() && !SWIsThrottled())
		{
			if (m_SWEnabled)
				ToggleSWStats();
			return;
		}

		super.OnKeyPress(key);
	}
}
