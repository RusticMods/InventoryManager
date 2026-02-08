modded class MissionGameplay 
{
    private ref InventoryManagerMenu im_menu;

    override void OnUpdate(float timeslice) {
        super.OnUpdate(timeslice);
        
        Input input = GetGame().GetInput();
        if (input.LocalPress("UAUIBack", false)) 
        {
            if (im_menu != NULL && GetGame().GetUIManager().GetMenu() == im_menu) {
                im_menu.Close();
            }
        }

        if ( input.LocalPress("UAInventoryManager", false) ) 
        {
            if (im_menu.IsMouseOnSearch()) return;
            PlayerBase player = PlayerBase.Cast(GetGame().GetPlayer());

            bool checkedPerms = player.IsRusticModsAdmin();

            if (!checkedPerms) {
                return;
            }

            if ( im_menu && checkedPerms) {
                if (im_menu.IsMenuOpen()) {
                    im_menu.Close();
                } else if (GetGame().GetUIManager().GetMenu() == NULL) {
                    GetGame().GetUIManager().ShowScriptedMenu(im_menu, NULL);
                    im_menu.SetMenuOpen(true);
                    LockControls();
                    im_menu.SetPlayer(player);
                }
            } else if (GetGame().GetUIManager().GetMenu() == NULL && im_menu == null && checkedPerms) {
                LockControls();
                im_menu = InventoryManagerMenu.Cast(GetUIManager().EnterScriptedMenu(IM_MENU, null));
                im_menu.SetMenuOpen(true);
                im_menu.SetPlayer(player);
            }
        }
    }

    private void LockControls()
    {
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        GetGame().GetUIManager().ShowUICursor( true );
        GetGame().GetMission().GetHud().Show( false );
    }

    private void UnlockControls()
    {
        GetGame().GetMission().PlayerControlEnable(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetUIManager().ShowUICursor( false );
        GetGame().GetMission().GetHud().Show( true );
    }
};

modded class MissionBase {
    override UIScriptedMenu CreateScriptedMenu(int id) {
        UIScriptedMenu im_menu = NULL;
        im_menu = super.CreateScriptedMenu(id);
        if (!im_menu) {
            switch (id) {
            case IM_MENU:
                im_menu = new InventoryManagerMenu;
                break;
            }
            if (im_menu) {
                im_menu.SetID(id);
            }
        }
        return im_menu;
    }
};