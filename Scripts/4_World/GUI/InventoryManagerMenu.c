class InventoryManagerMenu extends UIScriptedMenu
{
//VARS
    private bool                 	m_Initialized;
    private bool                 	m_IsMenuOpen;

	PlayerBase 						m_player;

	// Useless widgets
    private TextWidget           	m_Title;
    private Widget               	m_pnl_bg;

    private Widget                  m_SeperationBar;
	
	// Main functional widgets
    private ButtonWidget         	m_BtnClose;

	private TextWidget           	m_ScreenMessage;

    private ButtonWidget         	m_BtnUpdateList;
    private ButtonWidget            m_BtnTogglePlayerPreview;

    private ButtonWidget         	m_BtnClearInventory;

    private ButtonWidget         	m_BtnRepairAll;

    //Players

    private ScrollWidget     	    m_ScrollerPlayerContainer;
    private GridSpacerWidget     	m_GridPlayerContainer;

    private PlayerPreviewWidget 	m_PlayerPreview;

    private TextListboxWidget    	m_PlayerList;

    ref array<EntityAI>             itemsArray = new array<EntityAI>;

	private int                     m_LastPlayerSelected = -1;
    private PlayerBase              m_LastPlayer;
    private ButtonWidget         	m_BtnTeleportToPlayer;
    private vector                  m_SelectedPlayerVector;

    ref array<ref Widget> m_ChildWidgets;
    ref array<InventoryItem> items = new array<InventoryItem>;
    ref array<EntityAI> entities = new array<EntityAI>;

    //Search Function
    private EditBoxWidget           m_SearchBox;
    private ButtonWidget            m_BtnSearch;
    private ButtonWidget            m_BtnClearSearch;

    //Object Spawner
    private Widget                  m_ObjectSpawner;
    private TextListboxWidget       m_ObjectList;
    private ButtonWidget            m_BtnAddItem;

    private ButtonWidget            m_BtnCategoryAll;
    private ButtonWidget            m_BtnCategoryEdible;
    private ButtonWidget            m_BtnCategoryItems;
    private ButtonWidget            m_BtnCategoryWeapon;
    private ButtonWidget            m_BtnCategoryClothing;
    private string                  m_CurrentCategory = "All";

    private ItemPreviewWidget       m_ItemPreview;
    private EntityAI                m_ItemPreviewItem;
    private string                  m_ItemName;

    private Widget                  m_ItemCondition;
    private ButtonWidget            m_BtnConditionRaise;
    private ButtonWidget            m_BtnConditionLower;

    private TextWidget              m_ItemHealth;
    private string                  m_ItemHealthLevel = "Pristine";

    private Widget                  m_ItemQuantity;
    private TextWidget              m_ItemQuantityLabel;
    private TextWidget              m_QuantitySliderLabel;
    private SliderWidget            m_QuantitySlider;

    private TextWidget              m_SelectedItem;
    private ButtonWidget            m_BtnSpawnInventory;
    private ButtonWidget            m_BtnSpawnOnPlayer;

    private CheckBoxWidget          m_ChkBoxShouldSpawnAttachments;


    ConfirmationWindow action;
    
    bool m_MouseOnSearch = false;

    int ItemPreview = 0;
    int ItemName = 1;
    int ItemQLabel = 2;
    int ItemQuantity = 3;
    int BtnSetQuantity = 4;
    int BtnMaxQuantity = 5;
    int BtnMinQuantity = 6;
    int BtnDelete = 7;
    int BtnTake = 8;
    int ItemHealth = 9;
    int BtnLowerHealth = 10;
    int BtnRaiseHealth = 11;
    int ItemID = 12;
    int ItemHealthLevel = 13;

    int PristineColor = ARGB (255, 34, 139, 34); 
    int WornColor = ARGB (255, 154, 205, 50); 
    int DamagedColor = ARGB (255, 255, 255, 0); 
    int BadlyDamagedColor = ARGB (255, 255, 69, 0); 
    int RuinedColor = ARGB (255, 139, 0, 0); 


//INIT

    void InventoryManagerMenu()
    {
        GetRusticModsCoreLogger().Log("InventoryManagerMenu", "CLIENT", "Created & Initialized Menu for " + m_player.GetIdentity().GetName());

        GetRPCManager().AddRPC("InventoryManager", "SyncPlayers", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("InventoryManager", "OnFoundPlayerRPC", this, SingleplayerExecutionType.Client);
        GetRPCManager().AddRPC("InventoryManager", "OnFinishRPC", this, SingleplayerExecutionType.Client);

        m_ChildWidgets = new array<ref Widget>;
    }

    void ~InventoryManagerMenu() 
    {
        PPEffects.SetBlurMenu( 0 );
        GetGame().GetUIManager().Back();
        g_Game.GetUIManager().ShowCursor(true);
        g_Game.GetUIManager().ShowUICursor(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetMission().PlayerControlEnable(false);
        GetGame().GetMission().GetHud().Show( true );

        if ( layoutRoot )
        	layoutRoot.Unlink();
		
        for (int i = 0; i < entities.Count(); i++)
        {
            ItemBase item = ItemBase.Cast(entities[i]);
            if (item)
            {
                item.Delete();
            }
        }

        if (m_ItemPreviewItem) m_ItemPreviewItem.Delete();
    }

    override void OnShow()
    {
        super.OnShow();
        PPEffects.SetBlurMenu( 0.5 );

		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(GetData, 1);
    }

    void GetData()
    {
        GetRPCManager().SendRPC("InventoryManager", "SyncPlayersRPC", null, true);
    }

    override void OnHide()
    {
        super.OnHide();
        PPEffects.SetBlurMenu( 0 );

        g_Game.GetUIManager().ShowCursor(true);
        g_Game.GetUIManager().ShowUICursor(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetMission().PlayerControlEnable(false);
        GetGame().GetMission().GetHud().Show( true );
    }

    override Widget Init()
    {
        if (!m_Initialized)
        {
            layoutRoot = GetGame().GetWorkspace().CreateWidgets( "InventoryManager/Data/GUI/layouts/InventoryManagerMenu.layout" );
            
			// Useless widgets
			m_Title	   			        = TextWidget.Cast( layoutRoot.FindAnyWidget( "Title" ) );
			m_pnl_bg   			        = layoutRoot.FindAnyWidget( "pnl_bg" );
			
			// Main functional widgets
            m_BtnClose 			        = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnClose" ) );
			m_ScreenMessage		        = TextWidget.Cast( layoutRoot.FindAnyWidget( "ScreenMessage" ) );

            m_SeperationBar 			= layoutRoot.FindAnyWidget( "SeperationBar0" );

            m_BtnUpdateList 		    = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnUpdateList" ) );
            m_BtnTogglePlayerPreview    = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnTogglePreview" ) );

            m_BtnClearInventory 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnClearInventory" ) );

            m_BtnRepairAll 				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnRepairAll" ) );

            // Players
            m_PlayerList 			    = TextListboxWidget.Cast( layoutRoot.FindAnyWidget( "PlayerList" ) );

            m_ScrollerPlayerContainer 	= ScrollWidget.Cast( layoutRoot.FindAnyWidget( "PlayerContainer" ) ); 
			m_GridPlayerContainer   	= GridSpacerWidget.Cast( layoutRoot.FindAnyWidget( "PlayerContainerGrid" ) );

            m_PlayerPreview 			= PlayerPreviewWidget.Cast( layoutRoot.FindAnyWidget( "PlayerPreview" ) );

            m_BtnTeleportToPlayer 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnTeleportToPlayer" ) );

            //Search Function
            m_SearchBox 				= EditBoxWidget.Cast( layoutRoot.FindAnyWidget( "SearchBox" ) );
            m_BtnSearch 				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnSearch" ) );
            m_BtnClearSearch 			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnClearSearch" ) );

            //Object Spawner
            m_ObjectSpawner 		    = Widget.Cast( layoutRoot.FindAnyWidget( "ObjectSpawner" ) );
            m_ObjectList 				= TextListboxWidget.Cast( layoutRoot.FindAnyWidget( "ObjectsList" ) );
            m_BtnAddItem 				= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnAddItem" ) );

            m_BtnCategoryAll 		    = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "CategoryAll" ) );
            m_BtnCategoryEdible 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "CategoryEdible" ) );
            m_BtnCategoryItems 	        = ButtonWidget.Cast( layoutRoot.FindAnyWidget( "CategoryItems" ) );
            m_BtnCategoryWeapon 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "CategoryWeapon" ) );
            m_BtnCategoryClothing 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "CategoryClothing" ) );

            m_ItemPreview 				= ItemPreviewWidget.Cast( layoutRoot.FindAnyWidget( "ItemPreview" ) );

            m_ItemCondition             = Widget.Cast( layoutRoot.FindAnyWidget( "ItemCondition" ) );
            m_BtnConditionRaise 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnConditionRaise" ) );
            m_BtnConditionLower 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "BtnConditionLower" ) );

            m_ItemHealth 				= TextWidget.Cast( layoutRoot.FindAnyWidget( "ItemHealth" ) );

            m_ItemQuantity	            = Widget.Cast( layoutRoot.FindAnyWidget( "ItemQuantity" ) );
            m_ItemQuantityLabel 		= TextWidget.Cast( layoutRoot.FindAnyWidget( "ItemQuantityLabel" ) );
            m_QuantitySliderLabel 		= TextWidget.Cast( layoutRoot.FindAnyWidget( "QuantityLabel" ) );
            m_QuantitySlider 			= SliderWidget.Cast( layoutRoot.FindAnyWidget( "ItemQuantitySlider" ) );

            m_SelectedItem              = TextWidget.Cast( layoutRoot.FindAnyWidget( "SelectedItem" ) );

            m_BtnSpawnInventory 		= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "SpawnInventory" ) );
            m_BtnSpawnOnPlayer 			= ButtonWidget.Cast( layoutRoot.FindAnyWidget( "SpawnOn" ) );

            m_ChkBoxShouldSpawnAttachments = CheckBoxWidget.Cast( layoutRoot.FindAnyWidget( "ShouldSpawnAttachments" ) );
			
            m_Initialized = true;
        }

        float textProportion = GetInventoryManagerMenuTextProportion();
        m_Title.SetTextProportion(textProportion);
        m_BtnUpdateList.SetTextProportion(textProportion);
        m_BtnTogglePlayerPreview.SetTextProportion(textProportion);
        m_BtnClearInventory.SetTextProportion(textProportion);
        m_BtnAddItem.SetTextProportion(textProportion);
        m_BtnRepairAll.SetTextProportion(textProportion);
        m_BtnClearSearch.SetTextProportion(textProportion);
        m_BtnCategoryAll.SetTextProportion(textProportion);
        m_ItemQuantityLabel.SetTextProportion(textProportion);
        m_SelectedItem.SetTextProportion(textProportion);
        m_BtnSpawnInventory.SetTextProportion(textProportion);
        m_BtnSpawnOnPlayer.SetTextProportion(textProportion);
        m_BtnTeleportToPlayer.SetTextProportion(textProportion);
        m_ScreenMessage.SetTextProportion(textProportion);

        return layoutRoot;
    }

//PLAYER FUNCTIONS

    void OnSelectPlayer(int playerId)
    {
        GetRPCManager().SendRPC("InventoryManager", "RequestPlayerRPC", new Param5<int, string, string, ItemBase, PlayerBase>(playerId, "OnFoundPlayerRPC", "", NULL, NULL), true); 
    }

    void OnFoundPlayerRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param3<PlayerBase, array<EntityAI>, vector> param;
        if(!ctx.Read(param)) return;

        itemsArray = param.param2;
        if (m_SearchBox.GetText() == "") 
        {
            itemsArray = SortItemsByDisplayName(itemsArray);
        } else { itemsArray = SortItemsByDisplayName(itemsArray, m_SearchBox.GetText()); }
        m_SelectedPlayerVector = param.param3;
        m_LastPlayer = param.param1;

        m_ScreenMessage.Show(false);
        m_BtnTeleportToPlayer.Show(false);
        if (!param.param1)
        {
            //GetRusticModsCoreLogger().Log("InventoryManagerMenu", "CLIENT", "Player returned null on OnFoundPlayerRPC. (Is the player far away?) Not showing player preview.");
            m_ScreenMessage.Show(true);
            m_ScreenMessage.SetText("#str_inventorymanagermenu_playernotfound");
            m_GridPlayerContainer.Show(false);
            m_BtnTeleportToPlayer.Show(true);
            return;
        }

        m_ScreenMessage.Show(false);
        m_BtnTeleportToPlayer.Show(false);
        InitializeWidgets();
    }

    void InitializeWidgets()
    {
        ClearAllChildWidgets();

        Widget childLayout;

        ItemPreviewWidget itemPreview;
        TextWidget itemName;
        EditBoxWidget itemQuantity;
        TextWidget itemHealth;
        ButtonWidget deleteButton;
        ButtonWidget takeButton;
        ButtonWidget itemID;

        m_Title.Show(false);

        m_BtnClearInventory.Show(true);
        m_BtnRepairAll.Show(true);
        m_GridPlayerContainer.Show(true);
        m_ObjectSpawner.Show(false);
        m_PlayerList.Show(false);
        m_BtnAddItem.Show(true);
        m_SeperationBar.Show(true); 
        m_PlayerPreview.Show(true);
        m_PlayerPreview.SetPlayer(m_LastPlayer);
        m_PlayerPreview.UpdateItemInHands(m_LastPlayer.GetHumanInventory().GetEntityInHands());
        m_GridPlayerContainer.Show(true);

        m_BtnClearSearch.Show(true);
        //m_BtnSearch.Show(true);
        m_SearchBox.Show(true);
    
        InventoryItem Iitem;
        
        for (int l = 0; l < itemsArray.Count(); l++)
        {
            Iitem = InventoryItem.Cast(itemsArray[l]);
            if (Iitem)
            {
                childLayout = GetGame().GetWorkspace().CreateWidgets("InventoryManager/Data/GUI/layouts/ItemListing.layout", m_GridPlayerContainer); 
                m_ChildWidgets.Insert(childLayout);

                array<Widget> children = new array<Widget>;
                children = GetAllChildren(childLayout.GetChildren());

                itemPreview = ItemPreviewWidget.Cast( children[ItemPreview] );
                itemName = TextWidget.Cast( children[ItemName] );
                itemQuantity = EditBoxWidget.Cast( children[ItemQuantity] );
                itemHealth = TextWidget.Cast( children[ItemHealth] );
                itemID = ButtonWidget.Cast( children[ItemID] );

                EntityAI PreviewItem = EntityAI.Cast(GetGame().CreateObject(Iitem.GetType(), "0 0 0", true, false, true)); 
                if (PreviewItem && entities) entities.Insert(PreviewItem);
                itemPreview.SetItem( PreviewItem );
                itemPreview.SetModelPosition(Vector(0, 0, 0)); 
                itemPreview.SetModelOrientation(Vector(0, 0, 0)); 

                itemName.SetText(Iitem.GetDisplayName());
                itemQuantity.SetText(GetStackCount(Iitem).ToString());
                itemID.SetText(GetSubstringBetweenAngles(Iitem.ToString()));

                
                switch (Iitem.GetHealthLevel())
                {
                    case 0:
                        itemHealth.SetText("P");
                        itemHealth.SetColor(PristineColor);
                        PreviewItem.SetHealthLevel(0);
                        break;

                    case 1:
                        itemHealth.SetText("W");
                        itemHealth.SetColor(WornColor);
                        PreviewItem.SetHealthLevel(1);
                        break;

                    case 2:
                        itemHealth.SetText("D");
                        itemHealth.SetColor(DamagedColor);
                        PreviewItem.SetHealthLevel(2);
                        break;

                    case 3:
                        itemHealth.SetText("BD");
                        itemHealth.SetColor(BadlyDamagedColor);
                        PreviewItem.SetHealthLevel(3);
                        break;

                    case 4:
                        itemHealth.SetText("R");
                        itemHealth.SetColor(RuinedColor);
                        PreviewItem.SetHealthLevel(4);
                        break;

                    default:
                        itemHealth.SetText("X");
                        break;
                }
            }
        }
    }

//CLICK HANDLING

    override bool OnClick(Widget w, int x, int y, int button)
    {
        Param1<int> playerData;

    	switch(w)
    	{
    		case m_PlayerList:
                if (FindPlayerInList(m_LastPlayerSelected) != -1)
                {
                    m_PlayerList.GetItemData( FindPlayerInList(m_LastPlayerSelected), 0, playerData );
                }

                    int SelectedPlayer = m_PlayerList.GetSelectedRow();

                    if (SelectedPlayer == -1) return true;

                    for (int i = 0; i < m_PlayerList.GetNumItems(); i++)
                    {
                        m_PlayerList.SetItemColor(i, 0, 0xFFFFFFFF); // White color
                    }
            
                    m_PlayerList.SetItemColor( SelectedPlayer, 0, 0xFFFF751A );
			        m_PlayerList.GetItemData( SelectedPlayer, 0, playerData );

			        m_LastPlayerSelected = playerData.param1;
                    OnSelectPlayer(m_LastPlayerSelected);

                break;

            case m_BtnTeleportToPlayer:
                if (m_player && m_SelectedPlayerVector)
                {
                    GetRPCManager().SendRPC("InventoryManager", "TeleportPlayerRPC", new Param2<PlayerBase, vector>(m_player, m_SelectedPlayerVector), true);
                    OnSelectPlayer(m_LastPlayerSelected);
                }
                break;
            
            case m_BtnSearch:
                //DEPRECATED
                if (m_ObjectSpawner.IsVisible())
                {   
                    UpdateItemList(m_SearchBox.GetText());
                } else {
                    SortItemsByDisplayName(itemsArray, m_SearchBox.GetText());
                    InitializeWidgets();
                }
                break;

            case m_BtnClearSearch:
                ClearAllChildWidgets();
                    if (m_LastPlayerSelected != -1)
                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnSelectPlayer, 1, false, m_LastPlayerSelected);
                break;

            case m_BtnUpdateList:
                GetRPCManager().SendRPC("InventoryManager", "SyncPlayersRPC", null, true);
                break;

            case m_BtnTogglePlayerPreview:
                if (m_PlayerPreview.IsVisible())
                {
                    m_PlayerList.Show(true);
                    m_PlayerPreview.Show(false);
                    m_SeperationBar.Show(false);
                } else {
                    m_PlayerList.Show(false);
                    m_PlayerPreview.Show(true);
                    m_SeperationBar.Show(true);
                }
                break;

			case m_BtnClose:
                this.Close();
            	break;
            
            case m_BtnClearInventory:
                if (action)
                {
                    GetRPCManager().SendRPC("InventoryManager", "SendMessageRPC", new Param4<PlayerIdentity, int, string, string>(m_player.GetIdentity(), 5, string.Empty, "#str_inventorymanagermenu_confirmationalreadyopen"), true);
                    break;
                }
                action = CreateConfirmationWindow("ClearInventory", NULL, m_LastPlayerSelected, "#str_inventorymanagermenu_clearinventoryquestion");
                break;

            case m_BtnRepairAll:
                if (action)
                {
                    GetRPCManager().SendRPC("InventoryManager", "SendMessageRPC", new Param4<PlayerIdentity, int, string, string>(m_player.GetIdentity(), 5, string.Empty, "#str_inventorymanagermenu_confirmationalreadyopen"), true);
                    break;
                }
                action = CreateConfirmationWindow("RepairAll", NULL, m_LastPlayerSelected, "#str_inventorymanagermenu_repairallquestion");
                break;
            
            case m_BtnAddItem:
                if (m_ObjectSpawner.IsVisible())
                {
                    m_GridPlayerContainer.Show(true);
                    m_ObjectSpawner.Show(false);
                    m_BtnClearInventory.Show(true);
                    m_BtnRepairAll.Show(true);
                    m_BtnUpdateList.Show(true);
                    m_BtnTogglePlayerPreview.Show(true);
                    break;
                }
                m_CurrentCategory = "All";
                m_GridPlayerContainer.Show(false);
                m_ObjectSpawner.Show(true);
                m_BtnClearInventory.Show(false);
                m_BtnRepairAll.Show(false);
                m_BtnUpdateList.Show(false);
                m_BtnTogglePlayerPreview.Show(false);
                UpdateItemList();
                break;
            
            case m_BtnCategoryAll:
                m_CurrentCategory = "All";
                UpdateItemList();
                break;

            case m_BtnCategoryEdible:
                m_CurrentCategory = "edible_base";
                UpdateItemList();
                break;
            case m_BtnCategoryItems:
                m_CurrentCategory = "inventory_base";
                UpdateItemList();
                break;
            case m_BtnCategoryWeapon:
                m_CurrentCategory = "weapon_base";
                UpdateItemList();
                break;
            case m_BtnCategoryClothing:
                m_CurrentCategory = "clothing_base";
                UpdateItemList();
                break;

            case m_ObjectList:
                m_ObjectList.GetItemText( m_ObjectList.GetSelectedRow(), 0, m_ItemName ); 
                if (m_ItemName)
                {
                    m_ItemPreviewItem = EntityAI.Cast(GetGame().CreateObject(m_ItemName, "0 0 0", true, false, true)); 
                    m_ItemPreview.SetItem( m_ItemPreviewItem );
                    m_ItemPreview.SetModelPosition(Vector(0, 0, 0)); 
                    m_ItemPreview.SetModelOrientation(Vector(0, 0, 0)); 

                    SetCondition("Pristine", PristineColor);

                    m_ItemCondition.Show(true);

                    ItemBase itemBase = ItemBase.Cast(m_ItemPreviewItem);
                    if (IsItemStackable(itemBase)) 
                    {
                        m_ItemQuantity.Show(true);
                        int min;
                        if (itemBase.GetQuantityMin() == 0) { min = 1; } else { min = itemBase.GetQuantityMin(); }
                        m_QuantitySlider.SetMinMax(min, itemBase.GetQuantityMax());
                        m_QuantitySlider.SetCurrent(itemBase.GetQuantityMax());
                        m_QuantitySliderLabel.SetText(itemBase.GetQuantityMax().ToString());
                        
                    } else {
                        m_ItemQuantity.Show(false);
                    }

                    //if item has attachments
                    TStringArray attachments = {};
                    itemBase.ConfigGetTextArray("attachments", attachments);
                    if (attachments.Count() > 0)
                    {
                        m_ChkBoxShouldSpawnAttachments.Show(true);
                    }

                    m_SelectedItem.Show(true);
                    m_SelectedItem.SetText(m_ItemName);
                    m_BtnSpawnInventory.Show(true);
                    m_BtnSpawnOnPlayer.Show(true);
                }
                break;

            case m_BtnSpawnInventory:
                GetRPCManager().SendRPC("InventoryManager", "SpawnItemRPC", new Param6<string, float, int, vector, PlayerBase, bool>(m_ItemName, m_QuantitySlider.GetCurrent(), GetHealthLevel(m_ItemHealthLevel), "0 0 0", m_LastPlayer, m_ChkBoxShouldSpawnAttachments.IsChecked()), true); 
                break;

            case m_BtnSpawnOnPlayer:
                GetRPCManager().SendRPC("InventoryManager", "SpawnItemRPC", new Param6<string, float, int, vector, PlayerBase, bool>(m_ItemName, m_QuantitySlider.GetCurrent(), GetHealthLevel(m_ItemHealthLevel), m_LastPlayer.GetPosition(), NULL, m_ChkBoxShouldSpawnAttachments.IsChecked()), true); 
                break;

            case m_BtnConditionRaise:
                switch (m_ItemHealthLevel)
                {
                    case "Worn":
                        SetCondition("Pristine", PristineColor);
                        break;
                    case "Damaged":
                        SetCondition("Worn", WornColor);
                        break;
                    case "Badly Damaged":
                        SetCondition("Damaged", DamagedColor);
                        break;
                    case "Ruined":
                        SetCondition("Badly Damaged", BadlyDamagedColor);
                        break;
                }
                break;
            case m_BtnConditionLower:
                switch (m_ItemHealthLevel)
                {
                    case "Pristine":
                        SetCondition("Worn", WornColor);
                        break;
                    case "Worn":
                        SetCondition("Damaged", DamagedColor);
                        break;
                    case "Damaged":
                        SetCondition("Badly Damaged", BadlyDamagedColor);
                        break;
                    case "Badly Damaged":
                        SetCondition("Ruined", RuinedColor);
                        break;
                }
                break;

            

            default:
                if (w.ClassName() == "ButtonWidget")
                {
                    ButtonWidget btn = ButtonWidget.Cast(w);
                    string buttonText;
                    btn.GetText(buttonText);
                    array<Widget> children = new array<Widget>;
                    children = GetAllChildren(w.GetParent());

                    ButtonWidget itemIDWidget = ButtonWidget.Cast(children[ItemID]);

                    EditBoxWidget itemQuantity = EditBoxWidget.Cast( children[ItemQuantity] );
                    TextWidget itemHealth = TextWidget.Cast( children[ItemHealth] );
                    Widget confirmationWindow;
                    switch (buttonText)
                    {
                        case "Yes":
                            if (action)
                            {  
                                action.ExecuteAction();
                                confirmationWindow = action.GetWindow();
                                confirmationWindow.Unlink();
                                delete action;
                                m_GridPlayerContainer.Show(true);
                            }
                            break;

                        case "No":
                            if (action)
                            {
                                confirmationWindow = action.GetWindow();
                                confirmationWindow.Unlink();
                                delete action;
                                m_GridPlayerContainer.Show(true);
                            }
                            break;
                    }
                    
                    string itemID;
                    itemIDWidget.GetText(itemID);
                    
                    for (int j = 0; j < items.Count(); j++)
                    {
                        ItemBase item = ItemBase.Cast(items[j]);
                        if (item)
                        {
                            if (itemID == GetSubstringBetweenAngles(item.ToString()))
                            {
                                switch (buttonText)
                                {
                                    case "Delete":
                                        if (action)
                                        {
                                            GetRPCManager().SendRPC("InventoryManager", "SendMessageRPC", new Param4<PlayerIdentity, int, string, string>(m_player.GetIdentity(), 5, string.Empty, "#str_inventorymanagermenu_confirmationalreadyopen"), true);
                                            break;
                                        }
                                        string text = (new CF_Localiser("str_inventorymanagermenu_deleteitem")).Format();
			                            text.Replace("{0}", item.GetDisplayName());
                                        action = CreateConfirmationWindow("Delete", item, m_LastPlayerSelected, text, m_LastPlayer);
                                        break;

                                    case "Take":
                                        GetRPCManager().SendRPC("InventoryManager", "RequestPlayerRPC", new Param5<int, string, string, ItemBase, PlayerBase>(m_LastPlayerSelected, "OnFinishRPC", "TakeItem", item, m_player), true);                                    
                                        break;

                                    case "LowerHealth":
                                        GetRPCManager().SendRPC("InventoryManager", "LowerHealthRPC", new Param2<ItemBase, PlayerBase>(item, m_LastPlayer), true);
                                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitializeWidgets, 100, false);
                                        break;
                                    
                                    case "RaiseHealth":
                                        GetRPCManager().SendRPC("InventoryManager", "RaiseHealthRPC", new Param2<ItemBase, PlayerBase>(item, m_LastPlayer), true);
                                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitializeWidgets, 100, false);
                                        break;

                                    case "Set":
                                        GetRPCManager().SendRPC("InventoryManager", "SetItemRPC", new Param3<ItemBase, int, PlayerBase>(item, itemQuantity.GetText().ToInt(), m_LastPlayer), true);
                                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitializeWidgets, 100, false);
                                        break;

                                    case "MaxQuantity":
                                        GetRPCManager().SendRPC("InventoryManager", "SetItemRPC", new Param3<ItemBase, int, PlayerBase>(item, item.GetQuantityMax(), m_LastPlayer), true);
                                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitializeWidgets, 100, false);
                                        break;

                                    case "MinQuantity": 
                                        GetRPCManager().SendRPC("InventoryManager", "SetItemRPC", new Param3<ItemBase, int, PlayerBase>(item, 1, m_LastPlayer), true);
                                        GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(InitializeWidgets, 100, false);
                                        break;
                                }
                            }
                        }
                    }
                }
                break;
    	}
        return super.OnClick(w, x, y, button);
    }

    override bool OnChange( Widget w, int x, int y, bool finished ) 
    {
        if ( w == m_QuantitySlider )
        {
            m_QuantitySliderLabel.SetText( m_QuantitySlider.GetCurrent().ToString() );
            return true;
        }

        if ( w == m_SearchBox && m_ObjectSpawner.IsVisible() )
        {
            UpdateItemList(m_SearchBox.GetText());
            return true;
        }

        if ( w == m_SearchBox && !m_ObjectSpawner.IsVisible() )
        {
            OnSelectPlayer(m_LastPlayerSelected);
            return true;
        }
		
		return false;
	}

    override bool OnMouseEnter( Widget w, int x, int y ) 
    {
		if ( w == m_SearchBox ) 
        {
			m_MouseOnSearch = true;
			return true;
		}
		return false;
	}

	override bool OnMouseLeave( Widget w, Widget enterW, int x, int y ) 
    {
		if ( w == m_SearchBox ) 
        {
			m_MouseOnSearch = false;
			return true;
		}
		return false;
	}

//HELPER FUNCTIONS

    float GetInventoryManagerMenuTextProportion()
	{
		string translated = (new CF_Localiser("str_rusticmods_get_language")).Format();
		switch (translated)
        {
            case "2": // german
				return 0.5;
			case "3": // russian
				return 0.5;
			case "4": // polish
				return 0.75;
            case "5": // hungarian
				return 0.75;
            case "6": // italian
				return 0.75;
            case "7": // spanish
				return 0.6;
            case "8": // french
				return 0.75;
			case "9": // chinese
				return 0.75;
            case "10": // japanese
				return 0.5;
            case "11": // portuguese
                return 0.75;
			case "12": // chinesesimplified
				return 0.75;
            default:
                return 1.0;
        }
	
		// Default for all other languages
		return 1.0;
	}

    int GetHealthLevel(string condition)
    {
        switch (condition)
        {
            case "Pristine":
                return 0;
            case "Worn":
                return 1;
            case "Damaged":
                return 2;
            case "Badly Damaged":
                return 3;
            case "Ruined":
                return 4;
        }
        return 0;
    }

    void SetCondition(string condition, int color)
    {
        m_ItemHealthLevel = condition;
        m_ItemHealth.SetColor(color);
        m_ItemHealth.SetText(condition);

        switch (condition)
        {
            case "Pristine":
                m_ItemPreviewItem.SetHealthLevel(0);
                break;
            case "Worn":
                m_ItemPreviewItem.SetHealthLevel(1);
                break;
            case "Damaged":
                m_ItemPreviewItem.SetHealthLevel(2);
                break;
            case "Badly Damaged":
                m_ItemPreviewItem.SetHealthLevel(3);
                break;
            case "Ruined":
                m_ItemPreviewItem.SetHealthLevel(4);
                break;
        }
    }

    void UpdateItemList(string searchTerm = "")
	{
		m_ObjectList.ClearItems();

        TStringArray cfgPaths = new TStringArray;
        cfgPaths.Insert( "CfgVehicles" );
        cfgPaths.Insert( "CfgWeapons" );
        cfgPaths.Insert( "CfgMagazines" );

        searchTerm.ToLower();

        for (int x = 0; x < cfgPaths.Count(); ++x)
        {
            string Config_Path = cfgPaths.Get(x);
            int nClasses = g_Game.ConfigGetChildrenCount(Config_Path);

            for ( int nClass = 0; nClass < nClasses; ++nClass )
            {
                string strName;
                GetGame().ConfigGetChildName( Config_Path, nClass, strName );

                int scope = GetGame().ConfigGetInt( Config_Path + " " + strName + " scope" );

                if ( scope == 0 || scope == 1)
                    continue;

                string lowerCasedName = strName;
                lowerCasedName.ToLower();

                if ( m_CurrentCategory == "All" || GetGame().IsKindOf( lowerCasedName, m_CurrentCategory ) )
                {
                    if ((searchTerm != "" && (!lowerCasedName.Contains(searchTerm)))) 
                    {
                        continue;
                    }
                    m_ObjectList.AddItem( strName, NULL, 0 );
                }
            }
        }
	}

    ConfirmationWindow CreateConfirmationWindow(string RPCAction, ItemBase item, int LastPlayerSelected, string Text, PlayerBase target_player = NULL)
    {
        Widget Confirmation = GetGame().GetWorkspace().CreateWidgets("InventoryManager/Data/GUI/layouts/Confirmation.layout", layoutRoot); 
        ConfirmationWindow action;
        Confirmation.GetScript(action);

        array<Widget> children = new array<Widget>;
        children = GetAllChildren(Confirmation);

        Widget window = Widget.Cast(children[0]);

        children = GetAllChildren(window);

        TextWidget.Cast(children[0]).SetTextProportion(GetInventoryManagerMenuTextProportion());
        TextWidget.Cast(children[1]).SetTextProportion(GetInventoryManagerMenuTextProportion());
        TextWidget.Cast(children[1]).SetText(Text);


        if (action)
        {
            action.SetAction(RPCAction);
            action.SetItem(item);
            action.SetLastPlayerSelected(LastPlayerSelected);
            action.SetMenu(this);
            action.SetWindow(Confirmation);
            if (target_player)
                action.SetTargetPlayer(target_player);
        }

        m_GridPlayerContainer.Show(false);
        return action;
    }

    void OnFinishRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param4<PlayerBase, string, ItemBase, PlayerBase> param;
        if(!ctx.Read(param)) return;

        PlayerBase player = param.param1;
        ItemBase item = param.param3;
        PlayerBase takeItemPlayer = param.param4;

        if (param.param2 == "ClearInventory")
        {
            GetRPCManager().SendRPC("InventoryManager", "ClearInventoryRPC", new Param1<PlayerBase>(player), true);
            ClearAllChildWidgets();
            if (m_LastPlayerSelected != -1)
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnSelectPlayer, 100, false, m_LastPlayerSelected);
        }

        if (param.param2 == "RepairAll")
        {
            GetRPCManager().SendRPC("InventoryManager", "RepairAllRPC", new Param1<PlayerBase>(player), true);
            ClearAllChildWidgets();
            if (m_LastPlayerSelected != -1)
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnSelectPlayer, 100, false, m_LastPlayerSelected);
        }

        if (param.param2 == "TakeItem")
        {
            GetRPCManager().SendRPC("InventoryManager", "TakeItemRPC", new Param3<ItemBase, PlayerBase, PlayerBase>(item, takeItemPlayer, player), true);
            ClearAllChildWidgets();
            if (m_LastPlayerSelected != -1)
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(OnSelectPlayer, 100, false, m_LastPlayerSelected);
        }
    }

    void SyncPlayers(CallType type, ref ParamsReadContext ctx, ref PlayerIdentity sender, ref Object target)
    {
        Param1<ref InventoryManagerPlayerDataArray> players;
        ref InventoryManagerPlayerDataArray playerDataArray;
        
        if (type == CallType.Client && GetGame().IsClient() || !GetGame().IsMultiplayer())
        {
            if (!ctx.Read(players))
                return;

            m_PlayerList.ClearItems();
            playerDataArray = players.param1;
        }

        if (layoutRoot.IsVisible())
        {
            int playerId;
            string playerName;

            for (int i = 0; i < playerDataArray.Count(); i++)
            {
                playerId = playerDataArray.Get(i).m_playerId;
                playerName = playerDataArray.Get(i).m_playerName;
                m_PlayerList.AddItem(playerName, new Param1<int>(playerId), 0);
            }

            if (FindPlayerInList(m_LastPlayerSelected) != -1 && FindPlayerInList(m_LastPlayerSelected) < playerDataArray.Count())
            {
                Param1<ref InventoryManagerPlayerData> playerData;

                m_PlayerList.SetItemColor( FindPlayerInList(m_LastPlayerSelected), 0, 0xFFFF751A );
				m_PlayerList.GetItemData( FindPlayerInList(m_LastPlayerSelected), 0, playerData );
            }
        }   
    }

    array<EntityAI> SortItemsByDisplayName(array<EntityAI> itemsArray, string searchTerm = "")
    {
        ref map<string, ref array<EntityAI>> displayNameMap = new map<string, ref array<EntityAI>>;

        InventoryItem Iitem;
        items.Clear();
        for (int i = 0; i < itemsArray.Count(); i++)
        {
            Iitem = InventoryItem.Cast(itemsArray[i]);
            items.Insert(Iitem);

            EntityAI item = itemsArray[i];
            string displayName = item.GetDisplayName();

            string displayNameCheck = displayName;
            displayNameCheck.ToLower();
            searchTerm.ToLower();

            if (!displayNameMap.Contains(displayName))
            {
                if (searchTerm == "") displayNameMap.Insert(displayName, new array<EntityAI>);

                if (searchTerm != "" && displayNameCheck.Contains(searchTerm))
                {
                    displayNameMap.Insert(displayName, new array<EntityAI>);
                }
            }

            displayNameMap[displayName].Insert(item);
        }

        array<string> sortedDisplayNames = new array<string>;
        for (int j = 0; j < displayNameMap.Count(); j++)
        {
            sortedDisplayNames.Insert(displayNameMap.GetKey(j));
        }
        sortedDisplayNames.Sort();

        itemsArray.Clear();
        foreach (string sortedDisplayName : sortedDisplayNames)
        {
            array<EntityAI> items = displayNameMap[sortedDisplayName];
            items.Sort();

            for (int k = 0; k < items.Count(); k++)
            {
                itemsArray.Insert(items[k]);
            }
        }

        displayNameMap.Clear();
        return itemsArray;
    }

    private int FindPlayerInList(int playerId)
    {
        Param1<int> plyData;

		for (int idx = 0; idx < m_PlayerList.GetNumItems(); idx++) {
			m_PlayerList.GetItemData( idx, 0, plyData );

			if (plyData.param1 == playerId) return idx;
		}

		return -1;
    }

    bool IsItemStackable(ItemBase item)
    {
        return item.CanBeSplit() || item.HasQuantity();
    }

    string GetSubstringBetweenAngles(string input)
    {
        int startIndex = input.IndexOf("<");
        int endIndex = input.IndexOf(">");
    
        if (startIndex != -1 && endIndex != -1 && endIndex > startIndex)
        {
            return input.Substring(startIndex + 1, endIndex - startIndex - 1);
        }
    
        return "";
    }

    array<Widget> GetAllChildren(Widget parent)
    {
        array<Widget> children = new array<Widget>();
        Widget child = parent.GetChildren(); 

        while (child)
        {
            children.Insert(child);
            child = child.GetSibling();
        }

        return children;
    }
	
    void ClearAllChildWidgets()
    {
        for (int i = 0; i < m_ChildWidgets.Count(); i++)
        {
            Widget child = m_ChildWidgets[i];
            if (child)
            {
                child.Unlink();
            }
        }
        m_ChildWidgets.Clear();
    }

	void SetPlayer(PlayerBase player)
	{
		m_player = player;
	}

    bool IsMenuOpen() 
    {
        return m_IsMenuOpen;
    }

    bool IsMouseOnSearch()
    {
        return m_MouseOnSearch;
    }

    void SetMenuOpen(bool isMenuOpen) 
    {
        m_IsMenuOpen = isMenuOpen;
    }
	
	bool ShouldShowScrollBarPC()
    {
        float x, y;
        m_pnl_bg.GetScreenSize( x, y );
        return m_ScrollerPlayerContainer.GetContentHeight() > y;
    }

    int GetStackCount(ItemBase item)
    {
        if (item.IsAmmoPile())
        {
            auto mag = Magazine.Cast(item);
			return mag.GetAmmoCount();
        }
        else if (IsItemStackable(item))
        {
            return item.GetQuantity();
        }

        return 1;
    }
	
    override void Update(float timeslice)
    {
        super.Update(timeslice);

        if ( ShouldShowScrollBarPC() )
            m_ScrollerPlayerContainer.SetAlpha(1.0);
        else
            m_ScrollerPlayerContainer.SetAlpha(0.0);
        
    }
};

class ConfirmationWindow
{
    string RPCAction;
    int m_LastPlayerSelected;
    ItemBase item;
    InventoryManagerMenu menu;
    Widget Window;
    PlayerBase target_player;

    void SetWindow(Widget window)
    {
        Window = window;
    }

    Widget GetWindow()
    {
        return Window;
    }

    void SetAction(string action)
    {
        RPCAction = action;
    }

    void SetItem(ItemBase item)
    {
        this.item = item;
    }

    void SetLastPlayerSelected(int player)
    {
        m_LastPlayerSelected = player;
    }

    void SetMenu(InventoryManagerMenu menu)
    {
        this.menu = menu;
    }

    void SetTargetPlayer(PlayerBase player)
    {
        target_player = player;
    }

    void ExecuteAction()
    {
        if (RPCAction == "ClearInventory")
        {
            GetRPCManager().SendRPC("InventoryManager", "RequestPlayerRPC", new Param5<int, string, string, ItemBase, PlayerBase>(m_LastPlayerSelected, "OnFinishRPC", "ClearInventory", NULL, NULL), true);
        }

        if (RPCAction == "RepairAll")
        {
            GetRPCManager().SendRPC("InventoryManager", "RequestPlayerRPC", new Param5<int, string, string, ItemBase, PlayerBase>(m_LastPlayerSelected, "OnFinishRPC", "RepairAll", NULL, NULL), true);
        }

        if (RPCAction == "Delete")
        {
            GetRPCManager().SendRPC("InventoryManager", "DeleteItemRPC", new Param2<ItemBase, PlayerBase>(item, target_player), true);
            menu.ClearAllChildWidgets();
            if (m_LastPlayerSelected != -1)
                GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(menu.OnSelectPlayer, 100, false, m_LastPlayerSelected);
        }
    }
}