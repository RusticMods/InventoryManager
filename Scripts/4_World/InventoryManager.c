typedef array<ref InventoryManagerPlayerData> InventoryManagerPlayerDataArray;

class InventoryManagerPlayerData 
{
    int m_playerId;
    string m_playerName;

    void InventoryManagerPlayerData(int playerId, string playerName) 
    {
        m_playerId = playerId;
        m_playerName = playerName;
    }
}