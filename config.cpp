class CfgMods
{
	class InventoryManager
	{
		dir = "InventoryManager";
		hideName = 1;
		hidePicture = 1;
		name = "InventoryManager";
		credits = "Rustic";
		author = "Rustic";
		authorID = "69";
		version = "Version 1.0";
		inputs="InventoryManager/inputs.xml";
		type = "mod";
		dependencies[] = {"Game","World","Mission"};
		class defs
		{
			class imageSets
			{
				files[] = {"InventoryManager/Data/GUI/images/itemlisting.imageset", "InventoryManager/Data/GUI/images/objectspawner.imageset"};
			};

			class gameScriptModule
			{
				value = "";
				files[] = {"InventoryManager/Scripts/3_Game"};
			};

			class worldScriptModule
			{
				value = "";
				files[] = {"InventoryManager/Scripts/4_World"};
			};

			class missionScriptModule
			{
				value = "";
				files[] = {"InventoryManager/Scripts/5_Mission"};
			};
		};
	};
};
class CfgPatches
{
	class InventoryManager
	{
		units[] = {};
		weapons[] = {};
		requiredVersion = 0.1;
		requiredAddons[] = {"DZ_Data","DZ_Scripts","JM_CF_Scripts","RusticModsCore"};
	};
};
