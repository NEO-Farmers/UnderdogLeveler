class SWShopOffer
{
	string Id;
	string DisplayName;
	string Description;
	int Cost;
	string Category;

	void SWShopOffer()
	{
		Cost = 0;
	}
}

class SecondWindShopMenu extends ScriptedWidgetEventHandler
{
	protected Widget m_Root;
	protected Widget m_RowsHost;
	protected Widget m_BackButton;
	protected TextWidget m_GritText;
	protected TextWidget m_StatusText;

	protected ref array<Widget> m_RowWidgets;
	protected ref array<ref SWShopOffer> m_Offers;
	protected int m_Grit;
	protected bool m_IsOpen;
	protected bool m_InputLockActive;
	protected float m_ScrollOffset;
	protected float m_MaxScrollOffset;

	void SecondWindShopMenu()
	{
		m_RowWidgets = new array<Widget>;
		m_Offers = new array<ref SWShopOffer>;
		m_Grit = 0;
		m_IsOpen = false;
		m_InputLockActive = false;
		m_ScrollOffset = 0.0;
		m_MaxScrollOffset = 0.0;
	}

	void EnsureLayout()
	{
		if (m_Root)
			return;

		m_Root = GetGame().GetWorkspace().CreateWidgets(SW_LAYOUT_SHOP);
		if (!m_Root)
			return;

		m_RowsHost = m_Root.FindAnyWidget("SWRowsHost");
		m_BackButton = m_Root.FindAnyWidget("SWBackButton");
		m_GritText = TextWidget.Cast(m_Root.FindAnyWidget("SWGritText"));
		m_StatusText = TextWidget.Cast(m_Root.FindAnyWidget("SWStatusText"));

		m_Root.SetHandler(this);
		if (m_BackButton)
			m_BackButton.SetHandler(this);

		ApplyStyle();
	}

	// Colors are applied in code (SetColor) so they reliably override the
	// DayZDefaultPanel style and any layout default text color.
	protected void ApplyStyle()
	{
		if (!m_Root)
			return;

		Widget tint = m_Root.FindAnyWidget("SWScreenTint");
		Widget background = m_Root.FindAnyWidget("SWBackground");
		Widget topBar = m_Root.FindAnyWidget("SWTopBar");
		Widget wallet = m_Root.FindAnyWidget("SWWalletCard");
		Widget footer = m_Root.FindAnyWidget("SWFooterBar");
		TextWidget title = TextWidget.Cast(m_Root.FindAnyWidget("SWTitleText"));
		TextWidget subtitle = TextWidget.Cast(m_Root.FindAnyWidget("SWSubtitleText"));
		TextWidget grit = TextWidget.Cast(m_Root.FindAnyWidget("SWGritText"));
		TextWidget info = TextWidget.Cast(m_Root.FindAnyWidget("SWInfoText"));
		TextWidget status = TextWidget.Cast(m_Root.FindAnyWidget("SWStatusText"));
		TextWidget backText = TextWidget.Cast(m_Root.FindAnyWidget("SWBackText"));

		if (tint) tint.SetColor(ARGB(255, 0, 0, 0));
		if (background) background.SetColor(ARGB(255, 6, 9, 11));
		if (topBar) topBar.SetColor(ARGB(255, 20, 28, 34));
		if (wallet) wallet.SetColor(ARGB(255, 10, 13, 16));
		if (footer) footer.SetColor(ARGB(255, 20, 28, 34));
		if (m_RowsHost) m_RowsHost.SetColor(ARGB(255, 4, 6, 8));
		if (m_BackButton) m_BackButton.SetColor(ARGB(255, 28, 38, 46));

		if (title) title.SetColor(ARGB(255, 88, 200, 230));
		if (subtitle) subtitle.SetColor(ARGB(255, 150, 165, 180));
		if (grit) grit.SetColor(ARGB(255, 120, 230, 200));
		if (info) info.SetColor(ARGB(255, 170, 178, 186));
		if (status) status.SetColor(ARGB(255, 235, 240, 245));
		if (backText) backText.SetColor(ARGB(255, 230, 230, 230));
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

	void SetOffers(array<ref SWShopOffer> offers)
	{
		m_Offers = offers;
	}

	void UpdateGrit(int grit)
	{
		m_Grit = grit;
		if (m_GritText)
			m_GritText.SetText(string.Format("Grit: %1", m_Grit));
	}

	void SetStatus(string status)
	{
		if (m_StatusText)
			m_StatusText.SetText(status);
	}

	void Show()
	{
		EnsureLayout();
		if (!m_Root)
			return;

		m_ScrollOffset = 0.0;
		ApplyStyle();
		ApplyOverlayPriority();
		m_Root.Show(true);
		m_IsOpen = true;
		Rebuild();
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

	void ClearRows()
	{
		m_RowWidgets.Clear();

		if (!m_RowsHost)
			return;

		Widget child = m_RowsHost.GetChildren();
		while (child)
		{
			Widget next = child.GetSibling();
			child.Unlink();
			child = next;
		}
	}

	protected void UpdateScrollBounds()
	{
		m_MaxScrollOffset = 0.0;
		if (!m_Offers)
			return;

		float contentHeight = 0.02 + (m_Offers.Count() * 0.082);
		float viewportHeight = 1.0;
		m_MaxScrollOffset = contentHeight - viewportHeight;
		if (m_MaxScrollOffset < 0.0)
			m_MaxScrollOffset = 0.0;

		if (m_ScrollOffset < 0.0)
			m_ScrollOffset = 0.0;
		if (m_ScrollOffset > m_MaxScrollOffset)
			m_ScrollOffset = m_MaxScrollOffset;
	}

	protected void UpdateRowPositions()
	{
		if (!m_RowWidgets)
			return;

		float y = 0.02 - m_ScrollOffset;
		for (int i = 0; i < m_RowWidgets.Count(); i++)
		{
			Widget row = m_RowWidgets.Get(i);
			if (!row)
				continue;

			row.SetPos(0.01, y);
			if (y < -0.082 || y > 1.0)
				row.Show(false);
			else
				row.Show(true);

			y = y + 0.082;
		}
	}

	protected void ScrollRows(int direction)
	{
		float step = 0.082;
		m_ScrollOffset = m_ScrollOffset + (direction * step);

		if (m_ScrollOffset < 0.0)
			m_ScrollOffset = 0.0;
		if (m_ScrollOffset > m_MaxScrollOffset)
			m_ScrollOffset = m_MaxScrollOffset;

		UpdateRowPositions();
	}

	protected bool IsWidgetInside(Widget child, Widget container)
	{
		if (!child || !container)
			return false;

		Widget current = child;
		while (current)
		{
			if (current == container)
				return true;
			current = current.GetParent();
		}

		return false;
	}

	void Rebuild()
	{
		ClearRows();
		UpdateGrit(m_Grit);

		if (m_StatusText)
			m_StatusText.SetText("Scroll with the mouse wheel, then click a card. ESC or B returns to the Field Report.");

		if (!m_RowsHost || !m_Offers)
			return;

		float y = 0.02;
		for (int i = 0; i < m_Offers.Count(); i++)
		{
			SWShopOffer offer = m_Offers.Get(i);
			if (!offer)
				continue;

			Widget row = GetGame().GetWorkspace().CreateWidgets(SW_LAYOUT_SHOP_ROW, m_RowsHost);
			if (!row)
				continue;

			row.SetName(string.Format("SWRow_%1", i));
			row.SetPos(0.01, y);
			row.SetSize(0.98, 0.078);
			row.SetHandler(this);
			m_RowWidgets.Insert(row);
			y = y + 0.082;

			TextWidget idx = TextWidget.Cast(row.FindAnyWidget("SWRowIndexText"));
			TextWidget category = TextWidget.Cast(row.FindAnyWidget("SWRowCategoryText"));
			TextWidget name = TextWidget.Cast(row.FindAnyWidget("SWRowNameText"));
			TextWidget desc = TextWidget.Cast(row.FindAnyWidget("SWRowDescText"));
			TextWidget cost = TextWidget.Cast(row.FindAnyWidget("SWRowCostText"));
			TextWidget state = TextWidget.Cast(row.FindAnyWidget("SWRowStateText"));

			if (idx) idx.SetText((i + 1).ToString());
			if (category) category.SetText(offer.Category);
			if (name) name.SetText(offer.DisplayName);
			if (desc) desc.SetText(offer.Description);
			if (cost) cost.SetText(string.Format("%1 GR", offer.Cost));
			if (state)
			{
				if (offer.Cost <= m_Grit)
					state.SetText("Available");
				else
					state.SetText(string.Format("Need %1", offer.Cost - m_Grit));
			}

			ApplyRowVisualState(row, offer);
		}

		UpdateScrollBounds();
		UpdateRowPositions();
	}

	void RefreshRows()
	{
		UpdateGrit(m_Grit);

		if (!m_RowWidgets || !m_Offers)
			return;

		for (int i = 0; i < m_RowWidgets.Count(); i++)
		{
			if (i >= m_Offers.Count())
				break;

			Widget row = m_RowWidgets.Get(i);
			SWShopOffer offer = m_Offers.Get(i);
			if (!row || !offer)
				continue;

			TextWidget cost = TextWidget.Cast(row.FindAnyWidget("SWRowCostText"));
			TextWidget state = TextWidget.Cast(row.FindAnyWidget("SWRowStateText"));
			if (cost) cost.SetText(string.Format("%1 GR", offer.Cost));
			if (state)
			{
				if (offer.Cost <= m_Grit)
					state.SetText("Available");
				else
					state.SetText(string.Format("Need %1", offer.Cost - m_Grit));
			}

			ApplyRowVisualState(row, offer);
		}
	}

	protected void ApplyRowVisualState(Widget row, SWShopOffer offer)
	{
		if (!row || !offer)
			return;

		bool canBuy = (offer.Cost <= m_Grit);

		TextWidget idx = TextWidget.Cast(row.FindAnyWidget("SWRowIndexText"));
		TextWidget category = TextWidget.Cast(row.FindAnyWidget("SWRowCategoryText"));
		TextWidget name = TextWidget.Cast(row.FindAnyWidget("SWRowNameText"));
		TextWidget desc = TextWidget.Cast(row.FindAnyWidget("SWRowDescText"));
		TextWidget cost = TextWidget.Cast(row.FindAnyWidget("SWRowCostText"));
		TextWidget state = TextWidget.Cast(row.FindAnyWidget("SWRowStateText"));
		Widget accent = row.FindAnyWidget("SWRowAccent");
		Widget card = row.FindAnyWidget("SWRowCard");

		row.SetColor(ARGB(255, 10, 13, 16));
		if (card) card.SetColor(ARGB(255, 8, 10, 12));
		if (idx) idx.SetColor(ARGB(255, 255, 255, 255));
		if (category) category.SetColor(ARGB(255, 120, 150, 170));
		if (name) name.SetColor(ARGB(255, 240, 245, 250));
		if (desc) desc.SetColor(ARGB(255, 180, 186, 194));

		if (cost)
		{
			if (canBuy)
				cost.SetColor(ARGB(255, 120, 230, 200));
			else
				cost.SetColor(ARGB(255, 240, 150, 150));
		}
		if (state)
		{
			if (canBuy)
				state.SetColor(ARGB(255, 150, 235, 210));
			else
				state.SetColor(ARGB(255, 240, 175, 175));
		}
		if (accent)
		{
			if (canBuy)
				accent.SetColor(ARGB(255, 60, 170, 150));
			else
				accent.SetColor(ARGB(255, 140, 80, 80));
		}
	}

	bool CanPurchaseIndex(int index)
	{
		return index >= 0 && m_Offers && index < m_Offers.Count();
	}

	string GetOfferIdByIndex(int index)
	{
		if (!CanPurchaseIndex(index))
			return "";
		return m_Offers.Get(index).Id;
	}

	protected int GetOfferIndexFromWidget(Widget w)
	{
		Widget current = w;
		while (current)
		{
			string name = current.GetName();
			if (name == "SWBackButton" || name == "SWBackText")
				return -2;

			if (name.Contains("SWRow_"))
			{
				string indexString = name.Substring(6, name.Length() - 6);
				return indexString.ToInt();
			}

			current = current.GetParent();
		}

		return -1;
	}

	override bool OnMouseWheel(Widget w, int x, int y, int wheel)
	{
		if (!w || wheel == 0)
			return false;

		if (!IsWidgetInside(w, m_RowsHost))
			return false;

		int direction = 0;
		if (wheel < 0)
			direction = 1;
		else if (wheel > 0)
			direction = -1;

		if (direction == 0)
			return false;

		ScrollRows(direction);
		return true;
	}

	override bool OnMouseButtonUp(Widget w, int x, int y, int button)
	{
		if (button != MouseState.LEFT)
			return false;

		return OnClick(w, x, y, button);
	}

	override bool OnClick(Widget w, int x, int y, int button)
	{
		int index = GetOfferIndexFromWidget(w);

		if (index == -2)
		{
			MissionBase missionBack = MissionBase.Cast(GetGame().GetMission());
			if (missionBack)
				missionBack.BackFromSWShop();
			return true;
		}

		if (CanPurchaseIndex(index))
		{
			MissionBase mission = MissionBase.Cast(GetGame().GetMission());
			if (mission)
			{
				mission.RequestSWPurchase(GetOfferIdByIndex(index));
				SetStatus("Requisition request sent...");
			}
			return true;
		}

		return false;
	}
}
