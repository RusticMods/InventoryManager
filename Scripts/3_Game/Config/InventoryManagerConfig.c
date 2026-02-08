class InventoryManagerConfig {
    private string EnableDebugMode;
    private string WebhookLogs;

    void InventoryManagerConfig() 
    {
        EnableDebugMode = "false";
        WebhookLogs = "url (EVERYTHING AFTER https://discord.com/api/webhooks/ (Ex: https://discord.com/api/webhooks/12312541 would be 12312541))";
    }

    bool DebugMode() 
    {
        string lowerString = EnableDebugMode;
        lowerString.ToLower();

        return lowerString == "true";
    }

    string GetWebookURL() 
    {
        return WebhookLogs;
    }

    void Validate() 
    {
        if (EnableDebugMode == string.Empty) {EnableDebugMode = "false";}
        if (WebhookLogs == string.Empty) {WebhookLogs = "url (EVERYTHING AFTER https://discord.com/api/webhooks/ (Ex: https://discord.com/api/webhooks/12312541 would be 12312541))";}
    }
}