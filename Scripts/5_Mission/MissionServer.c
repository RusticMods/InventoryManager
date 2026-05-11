modded class MissionServer 
{
#ifdef SERVER
    ref InventoryManagerConfig IMConfig;
    string configFolder;
#endif

    void MissionServer() 
    {
#ifdef SERVER
        GetRusticModsCoreLogger().Log("InventoryManager", "SERVER", "Server Mod Loaded Correctly");

//CONFIG
        configFolder = CONSTRusticModsCore.RM_MOD_FOLDER + CONSTInventoryManagerConfig.IMConfigName;

        if (!FileExist(configFolder))
        {
            if (!IMConfig)
            {
                IMConfig = new InventoryManagerConfig();
            }

            GetDayZGame().SetInventoryManagerConfig(IMConfig);
            MakeDirectory(CONSTRusticModsCore.RM_MOD_FOLDER);
            SaveIMConfig();
        } else {
            LoadIMConfig();
            GetDayZGame().SetInventoryManagerConfig(IMConfig);
            SaveIMConfig();
        }
#endif
    }

    array<Man> GetPlayers()
    {
        array <Man> players = new array < Man >; 
        GetGame().GetPlayers(players);
        return players;
    }

#ifdef SERVER
//CONFIG
    private void SaveIMConfig()
    {
        JsonFileLoader<InventoryManagerConfig>.JsonSaveFile(configFolder, IMConfig);
    }

    private void LoadIMConfig()
    {
        JsonFileLoader<InventoryManagerConfig>.JsonLoadFile(configFolder, IMConfig);
        IMConfig.Validate();
    }
#endif
};
