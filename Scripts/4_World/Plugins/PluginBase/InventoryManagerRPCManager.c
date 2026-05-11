#ifdef SERVER
class InventoryManagerRPCManager : PluginBase 
{
    void InventoryManagerRPCManager() 
    {
		GetRPCManager().AddRPC("InventoryManager", "SyncPlayersRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "DeleteItemRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "TakeItemRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "LowerHealthRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "RaiseHealthRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "ClearInventoryRPC", this, SingleplayerExecutionType.Both);
        GetRPCManager().AddRPC("InventoryManager", "RepairAllRPC", this, SingleplayerExecutionType.Both);
        GetRPCManager().AddRPC("InventoryManager", "RequestPlayerRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "TeleportPlayerRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "SetItemRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "SendMessageRPC", this, SingleplayerExecutionType.Both); 
        GetRPCManager().AddRPC("InventoryManager", "SpawnItemRPC", this, SingleplayerExecutionType.Both);
    };

//SYNC PLAYERS
    void SyncPlayersRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        ref InventoryManagerPlayerDataArray players = new InventoryManagerPlayerDataArray;
        array<Man> playersArray = new array<Man>;
        ref InventoryManagerPlayerData playerData;

        PlayerBase player;
        int playerId;
        string playerName;

        if ( type == CallType.Server && GetGame().IsServer() )
        {
            GetGame().GetPlayers(playersArray);

            for (int i = 0; i < playersArray.Count(); i++)
            {
                player = PlayerBase.Cast(playersArray.Get(i));
                playerId = player.GetIdentity().GetPlayerId();
                playerName = player.GetIdentity().GetName();

                playerData = new InventoryManagerPlayerData(playerId, playerName);

                players.Insert(playerData);
                GetRPCManager().SendRPC("InventoryManager", "SyncPlayers", new Param1<ref InventoryManagerPlayerDataArray>(players), true, sender);
            }
        }
    }
	


//SEND MESSAGE
    void SendMessage(PlayerIdentity NotificationPlayer, int time, string message, string title = "#str_inventorymanager_im") 
    {
        NotificationSystem.SendNotificationToPlayerIdentityExtended(NotificationPlayer, time, title, message);
    }

    void SendMessageRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param4<PlayerIdentity, int, string, string> param;
        if(!ctx.Read(param)) return;

        PlayerIdentity player = param.param1;
        int time = param.param2;
        string title = param.param3;
        string message = param.param4;

        if (title == string.Empty)
        {
            title = "#str_inventorymanager_im"; // Default title
        }

        SendMessage(player, time, message, title);
    }

//CLEAR INVENTORY
    void ClearInventoryRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<PlayerBase> param;
        if(!ctx.Read(param)) return;

        PlayerBase player = param.param1;

        if (!player)
        {
            SendMessage(sender, 5, "#str_inventorymanager_clearinventoryfailed");
            return;
        } else {
            player.RemoveAllItems();
            GetRPCManager().SendRPC("InventoryManager", "SendTranslatedReplacedStringIM", new Param4<PlayerIdentity, string, string, string>(sender, "str_inventorymanager_clearinventorysuccess", player.GetIdentity().GetName(), string.Empty), true, sender);
            GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " cleared inventory of " + player.GetIdentity().GetName());
        }

        GetRPCManager().SendRPC("InventoryManager", "OnFinishRPC", null, true, sender);
    }

//REPAIR ALL
    void RepairAllRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param1<PlayerBase> param;
        if(!ctx.Read(param)) return;

        PlayerBase player = param.param1;

        if (!player)
        {
            SendMessage(sender, 5, "#str_inventorymanager_repairallfailed");
            return;
        }

        array<EntityAI> itemsArray = new array<EntityAI>;
        player.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray);

        for (int i = 0; i < itemsArray.Count(); i++)
        {
            ItemBase itemBase = ItemBase.Cast(itemsArray.Get(i));
            if (itemBase)
            {
                itemBase.SetHealthLevel(0); // Set item health to Pristine
            }
        }

        GetRPCManager().SendRPC("InventoryManager", "SendTranslatedReplacedStringIM", new Param4<PlayerIdentity, string, string, string>(sender, "str_inventorymanager_repairallsuccess", player.GetIdentity().GetName(), string.Empty), true, sender);
        GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " repaired all items in " + player.GetIdentity().GetName() + "'s inventory");
        GetRPCManager().SendRPC("InventoryManager", "OnFinishRPC", null, true, sender);
    }

//DELETE ITEM
    void DeleteItemRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param2<ItemBase, PlayerBase> param;
        if(!ctx.Read(param)) return;

        ItemBase item = param.param1;
        PlayerBase target_player = param.param2;

        GetRPCManager().SendRPC("InventoryManager", "SendTranslatedReplacedStringIM", new Param4<PlayerIdentity, string, string, string>(sender, "str_inventorymanager_deleteditem", item.GetDisplayName(), string.Empty), true, sender);
        GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " deleted item " + item.GetDisplayName() + " from " + target_player.GetIdentity().GetName() + "'s inventory");

        item.Delete();
    }

//TAKE ITEM
    void TakeItemRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param3<ItemBase, PlayerBase, PlayerBase> param;
        if(!ctx.Read(param)) return;

        ItemBase item = param.param1;
        PlayerBase player = param.param2;
        PlayerBase selectedPlayer = param.param3;
        

        if (!item || !player) {SendMessage(sender, 5, "#str_inventorymanager_takeitemfailed"); return;}

        GameInventory playerInventory = player.GetInventory();
        GameInventory currentInventory = selectedPlayer.GetInventory();

        if (currentInventory == playerInventory)
        {
            SendMessage(sender, 5, "#str_inventorymanager_takeitemfailedself");
            return;
        }

        InventoryLocation il = new InventoryLocation();
        item.GetInventory().GetCurrentInventoryLocation(il);

        InventoryLocation dst_il = new InventoryLocation();
        bool HFS = playerInventory.FindFreeLocationFor(item, FindInventoryLocationType.CARGO, dst_il);

        if (!HFS)
        {
            SendMessage(sender, 5, "#str_inventorymanager_takeitemfailedspace");
            return;
        }

        if (currentInventory.LocationCanRemoveEntity(il))
        {
            GetGame().ClearJuncture(player, item);
            playerInventory.ClearInventoryReservation(item, dst_il);
            bool result = currentInventory.LocationSyncMoveEntity(il, dst_il);
            if (result && dst_il.IsValid())
                InventoryInputUserData.SendServerMove(null, InventoryCommandType.SYNC_MOVE, il, dst_il);

            GetRPCManager().SendRPC("InventoryManager", "OnFinishRPC", null, true, sender);
            GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", player.GetIdentity().GetName() + " took " + item.GetDisplayName() + " from " + selectedPlayer.GetIdentity().GetName());
        } else {
            SendMessage(sender, 5, "#str_inventorymanager_takeitemfailedmove");
            GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", "Move item failed.");
            
            return;
        }
        
    }

//Health ITEM
    void LowerHealthRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param2<ItemBase, PlayerBase> param;
        if(!ctx.Read(param)) return;

        ItemBase item = param.param1;
        PlayerBase target_player = param.param2;
        
        int currentHealth = item.GetHealthLevel();
        int newHealth = currentHealth + 1;

        if (currentHealth == 4) return;
        item.SetHealthLevel(newHealth);
        LogHealthLevel(sender, target_player, item, newHealth);
    }

    void RaiseHealthRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param2<ItemBase, PlayerBase> param;
        if(!ctx.Read(param)) return;

        ItemBase item = param.param1;
        PlayerBase target_player = param.param2;
        
        int currentHealth = item.GetHealthLevel();
        int newHealth = currentHealth - 1;

        if (currentHealth == 0) return;
        item.SetHealthLevel(newHealth);
        LogHealthLevel(sender, target_player, item, newHealth);
    }

    void LogHealthLevel(PlayerIdentity admin, PlayerBase target_player, ItemBase item, int newHealthLevel)
    {
        string healthLevel = "";
        switch (newHealthLevel)
        {
            case 0:
                healthLevel = "Pristine";
                break;

            case 1:
                healthLevel = "Worn";
                break;

            case 2:
                healthLevel = "Damaged";
                break;

            case 3:
                healthLevel = "Badly Damaged";
                break;

            case 4:
                healthLevel = "Ruined";
                break;
        }

        GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", admin.GetName() + " Set " + item.GetDisplayName() + " to " + healthLevel + " in " + target_player.GetIdentity().GetName() + "'s inventory");
    }

//SET ITEM

    void SetItemRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param3<ItemBase, int, PlayerBase> param;
        if(!ctx.Read(param)) return;

        ItemBase item = param.param1;
        int quantity = param.param2;
        PlayerBase target_player = param.param3;

        if (!item || !IsItemStackable(item)) 
        {
            SendMessage(sender, 2, "#str_inventorymanager_setitemnotfound");
            return;
        }

        if (quantity > item.GetQuantityMax()) 
        {
            SendMessage(sender, 2, "#str_inventorymanager_setitemfailedgreaterthanmax");
            return;
        }

        if (quantity < 1) 
        {
            SendMessage(sender, 2, "#str_inventorymanager_setitemfailedlessthanone");
            return;
        }

        item.SetQuantity(quantity);

        if (item.GetQuantity() != quantity)
        {
            SendMessage(sender, 2, "#str_inventorymanager_setitemgenericfail");
            return;
        }
        
        GetRPCManager().SendRPC("InventoryManager", "SendTranslatedReplacedStringIM", new Param4<PlayerIdentity, string, string, string>(sender, "str_inventorymanager_setitemsuccess", quantity.ToString(), string.Empty), true, sender);
        GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " set " + item.GetDisplayName() + " quantity to " + quantity + " in " + target_player.GetIdentity().GetName() + "'s inventory");

    }

    bool IsItemStackable(ItemBase item)
    {
        return item.CanBeSplit() || item.HasQuantity();
    }

//SPAWN ITEM

    void SpawnItemRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param6<string, float, int, vector, PlayerBase, bool> param;
        if(!ctx.Read(param)) return;

        string itemName = param.param1;
        float quantity = param.param2;
        int health = param.param3;
        vector position = param.param4; 
        PlayerBase player = param.param5;
        bool SpawnAttachments = param.param6;

        GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", "SpawnItemRPC " + itemName + " " + quantity + " " + health + " " + position + " " + player + " " + SpawnAttachments.ToString());

        if (!itemName) { SendMessage(sender, 2, "#str_inventorymanager_spawnitemfailednotfound"); return; }
        if (health != 0 && health != 1 && health != 2 && health != 3 && health != 4) { SendMessage(sender, 2, "#str_inventorymanager_spawnitemfailedhealthinvalid"); return; }
        if (!player && !position) { SendMessage(sender, 2, "#str_inventorymanager_spawnitemfaileddestination"); return; }

        
        
        ItemBase item;
        if (player)
        {
            item = ItemBase.Cast(player.GetInventory().CreateInInventory(itemName));
            if (item)
            {
                SetItemParams(item, quantity, health, SpawnAttachments, sender);
                GetRPCManager().SendRPC("InventoryManager", "SendTranslatedReplacedStringIM", new Param4<PlayerIdentity, string, string, string>(sender, "str_inventorymanager_spawnitemsuccessgave", itemName, player.GetIdentity().GetName()), true, sender);
                GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " gave " + itemName + " to " + player.GetIdentity().GetName());
                return;
            }
        }

        if (position) 
        { 
            item = GetGame().CreateObject(itemName, position, false, true); 
            if (item) 
            { 
                SetItemParams(item, quantity, health, SpawnAttachments, sender); 
                GetRPCManager().SendRPC("InventoryManager", "SendTranslatedReplacedStringIM", new Param4<PlayerIdentity, string, string, string>(sender, "str_inventorymanager_spawnitemsuccessposition", itemName, string.Empty), true, sender);
                GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " spawned " + itemName + " at players position");

                return; 
            } else {
                GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " tried to spawn " + itemName + " but item does not exist");
            }
        } else {
            GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", sender.GetName() + " tried to spawn " + itemName + " but position is invalid");
        }

        SendMessage(sender, 2, "#str_inventorymanager_spawnitemgenericfail");
    }

    void SetItemParams(ItemBase item, int quantity, int health, bool SpawnAttachments, PlayerIdentity sender)
    {
        if (IsItemStackable(item)) 
        { 
            if (quantity < 1 || quantity > item.GetQuantityMax())
            {
                SendMessage(sender, 2, "#str_inventorymanager_spawnitemfailedquantity"); 
                return; 
            }
            item.SetQuantity(quantity);
        }
        item.SetHealthLevel(health);
        if (SpawnAttachments)
        {
            SpawnCompatibleAttachments(item);
        }
    }

//REQUEST PLAYER

    void RequestPlayerRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param5<int, string, string, ItemBase, PlayerBase> param;
        if(!ctx.Read(param)) return;

        int playerId = param.param1;
        string RPCToCall = param.param2;
        string OnFinishRPCFunction = param.param3;

        ItemBase takeItemItem = param.param4;
        PlayerBase takeItemPlayer = param.param5;

        if (playerId == -1) return;

        array<Man> players = new array<Man>;
        PlayerBase player = NULL;

        GetGame().GetPlayers(players);

        for (int i = 0; i < players.Count(); i++)
        {
            player = PlayerBase.Cast(players.Get(i));
            if (player.GetIdentity().GetPlayerId() == playerId)
            break;
        }

        if (!player) {GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", "Player returned null."); return;}
        
        

        if (RPCToCall == "OnFoundPlayerRPC")
        {
            array<EntityAI> itemsArray = new array<EntityAI>;

            player.GetInventory().EnumerateInventory(InventoryTraversalType.INORDER, itemsArray);

            GetRusticModsCoreLogger().Log("InventoryManagerRPC", "CLIENT", "Found " + itemsArray.Count() + " items.");

            GetRPCManager().SendRPC("InventoryManager", "OnFoundPlayerRPC", new Param3<PlayerBase, array<EntityAI>, vector>(player, itemsArray, player.GetPosition()), true, sender);

            player = NULL;
        }
        
        
        if (RPCToCall == "OnFinishRPC")
        {
            GetRPCManager().SendRPC("InventoryManager", "OnFinishRPC", new Param4<PlayerBase, string, ItemBase, PlayerBase>(player, OnFinishRPCFunction, takeItemItem, takeItemPlayer), true, sender);
        }
        player = NULL;
    }

//TELEPORT PLAYER
    void TeleportPlayerRPC(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param2<PlayerBase, vector> param;
        if(!ctx.Read(param)) return;
        PlayerBase player = param.param1;
        vector pos = param.param2;

        if (player && pos)
        {
            player.SetPosition(pos);
            GetRusticModsCoreLogger().Log("InventoryManagerRPC", "SERVER", player.GetIdentity().GetName() + " teleported to " + pos);
        }
    }

//SPAWN ATTACHMENTS

    private ref array< string > m_UnfinishedItems = 
	{
		"quickiebow", 
		"recurvebow", 
		"gp25base", 
		"gp25", 
		"gp25_standalone", 
		"m203base", 
		"m203", 
		"m203_standalone", 
		"red9", 
		"pvcbow", 
		"crossbow", 
		"m249", 
		"undersluggrenadem4", 
		"groza", 
		"pm73rak", 
		"trumpet", 
		"lawbase", 
		"law", 
		"rpg7base", 
		"rpg7", 
		"dartgun", 
		"shockpistol", 
		"shockpistol_black", 
		"fnx45_arrow", 
		"makarovpb", 
		"mp133shotgun_pistolgrip", 

		"largetentbackpack", 
		"splint_applied", 
		"leatherbelt_natural", 
		"leatherbelt_beige", 
		"leatherbelt_brown", 
		"leatherbelt_black", 
		"leatherknifesheath" 
	}; 

	private ref array< string > m_RestrictedClassNames =
	{
		"placing",
		"debug",
		"bldr_",
		"land_",
		"staticobj_"
	};

    bool IsExcludedClassName( string className )
	{
		if ( m_UnfinishedItems.Find( className ) > -1 )
		{
			return true;
		}

		foreach ( string restrictedClassName: m_RestrictedClassNames )
		{
			if ( className.Contains( restrictedClassName ) )
			{
				return true;
			}
		}

        foreach (string unfinisheditem : m_UnfinishedItems)
        {
            if (className.Contains(unfinisheditem))
            {
                return true;
            }
        }


		return false;
	}

    void SpawnChildAttachments(EntityAI entity, int depth = 3)
    {
        ItemBase item;

        if (!entity.GetInventory())
			return;

		if (!entity.GetInventory().GetAttachmentSlotsCount())
			return;

		if (entity.GetInventory().AttachmentCount())
			return;

		if (entity.IsInherited(DayZCreature)  || entity.IsInherited(TentBase) || entity.IsInherited(Weapon_Base))
			return;

		SpawnCompatibleAttachments(entity, depth);
    }

    void SpawnCompatibleAttachments(EntityAI entity, int depth = 3) 
	{
		TIntArray slot_ids = {};
		int slot_id;

		TStringArray atts = {};
		entity.ConfigGetTextArray("attachments", atts);

		foreach (string att: atts)
		{
			slot_id = InventorySlots.GetSlotIdFromString(att);
			if (slot_id != InventorySlots.INVALID && InventorySlots.GetShowForSlotId(slot_id))
			{
				slot_ids.Insert(slot_id);
			}
		}

		TStringArray all_paths = new TStringArray;

		all_paths.Insert(CFG_VEHICLESPATH);
		all_paths.Insert(CFG_WEAPONSPATH);

		string child_name;
		int scope;
		string path;
		string model;
		int idx;
		EntityAI child;

		foreach (string config_path: all_paths)
		{
			int children_count = GetGame().ConfigGetChildrenCount(config_path);

			for (int i = 0; i < children_count; i++)
			{
				GetGame().ConfigGetChildName(config_path, i, child_name);
				path = config_path + " " + child_name;
				scope = GetGame().ConfigGetInt(path + " scope");

				if (scope == 2)
				{
					if (!GetGame().ConfigGetText(path + " model", model) || model == string.Empty)
						continue;

					TStringArray inv_slots = {};
					GetGame().ConfigGetTextArray(path + " inventorySlot", inv_slots);

					foreach (string inv_slot: inv_slots)
					{
						slot_id = InventorySlots.GetSlotIdFromString(inv_slot);
						if (slot_id != InventorySlots.INVALID)
						{
							idx = slot_ids.Find(slot_id);
							if (idx > -1)
							{
								//if (!IsInventoryBase(path))
									//break;

								child_name.ToLower();
                                if (IsExcludedClassName(child_name))
								    break;

								child = entity.GetInventory().CreateAttachmentEx(child_name, slot_id);
								if (child)
								{
									slot_ids.Remove(idx);
									if (depth > 0)
										SpawnChildAttachments(child, depth - 1);
									if (slot_ids.Count() == 0)
										return;
								}
							}
						}
					}
				}
			}
		}
	}

}

InventoryManagerRPCManager GetInventoryManagerRPCManager() {
    return InventoryManagerRPCManager.Cast(GetPlugin(InventoryManagerRPCManager));
}
#endif
