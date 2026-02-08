modded class MissionServer 
{
    void MissionServer() 
    {
        
    }

    array<Man> GetPlayers()
    {
        array <Man> players = new array < Man >; 
        GetGame().GetPlayers(players);
        return players;
    }
};