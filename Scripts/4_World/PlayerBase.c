modded class PlayerBase
{
	override void Init()
	{
		super.Init();

		GetRPCManager().AddRPC("InventoryManager", "SendTranslatedReplacedStringIM", this, SingleplayerExecutionType.Client);
	}

	string TranslateStringIM(string key)
	{
		return (new CF_Localiser(key)).Format();
	}

	void SendTranslatedReplacedStringIM(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        Param4<PlayerIdentity, string, string, string> param;
        if(!ctx.Read(param)) return;

        PlayerIdentity playerIdentity = param.param1;
        string key = param.param2;
        string replaceString = param.param3;
        string replaceString2 = param.param4;

		string translatedString = TranslateStringIM(key);
		translatedString.Replace("{0}", replaceString);
        translatedString.Replace("{1}", replaceString2);

		GetRPCManager().SendRPC("InventoryManager", "SendMessageRPC", new Param4<PlayerIdentity, int, string, string>(playerIdentity, 5, "#str_inventorymanager_im", translatedString), true);
	}
};