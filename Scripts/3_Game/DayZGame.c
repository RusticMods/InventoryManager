modded class DayZGame 
{
    protected ref InventoryManagerConfig m_InventoryManagerConfig;

    ref InventoryManagerConfig GetInventoryManagerConfig()
    {
        return m_InventoryManagerConfig;
    }

    void SetInventoryManagerConfig(ref InventoryManagerConfig inventoryManagerConfig)
    {
        m_InventoryManagerConfig = inventoryManagerConfig;
    }
}