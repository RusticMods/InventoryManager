# InventoryManager

InventoryManager is the **client-side/UI half** of the Rustic inventory administration mod for DayZ. It provides the menu, keybind, list syncing, item browsing, and client-side interaction layer used to inspect and manage player inventories.

This project is now open source under **GPLv3**.

## What it does

InventoryManager gives authorized administrators an in-game interface for managing player inventories.

The client mod supports:

- Opening an admin inventory management UI with a keybind
- Viewing a synchronized list of online players
- Loading a target player's inventory into a navigable menu
- Searching for item classnames
- Toggling player preview in the UI
- Spawning items directly into a player's inventory
- Spawning items at the player's position
- Optional spawning of compatible attachments when creating items
- Clearing a player's inventory
- Repairing all items in a player's inventory
- Adjusting item quantity for stackable items
- Raising or lowering item health state
- Deleting items from a target player's inventory
- Taking items from a target player
- Teleporting to the selected player
- Localized UI text through `stringtable.csv`

## Client vs server

This repository splits InventoryManager into two mods:

- **InventoryManager** — client/UI/content side
- **InventoryManager_Server** — server RPC/config/authority side

You need **both** parts for the full mod to function correctly on a server.

## Permissions

Opening the UI is restricted to players flagged as admins.

That permission comes from **RusticModsCore** via:

```text
profiles/RusticMods/Permissions/users.json
```

Players must be present in the `SuperAdmins` list used by RusticModsCore.

## Installation

- Click `Code`, then `Download ZIP`
- Extract the folder within the ZIP file into your P drive
- Remove the "-main" from the folder name (Eg: InventoryManager instead of InventoryManager-main)
- Using AddonBuilder (or other packer tool), pack the folder into a PBO
- Sign PBO using DS Utils
- Upload to server pack/workshop
- Done

You should also install:

- `RusticModsCore`
- `InventoryManager_Server`
- `CF`

## Features exposed by the UI

### Player management

- Refresh online player list
- Select a player by name
- Preview target player model/position when available
- Teleport to the selected player

### Inventory actions

- Clear inventory
- Repair all inventory items
- Delete a selected item
- Take a selected item
- Change stack quantity
- Raise/lower item condition

### Item spawning

- Search item classnames
- Choose quantity
- Choose health state
- Spawn into inventory
- Spawn on player position
- Optionally spawn compatible attachments

## Configuration

See `InventoryManager_Server` for server-side setup details.

## License

This project is licensed under the **GNU General Public License v3.0**. See `LICENSE-GPL-3.0.txt` for the full license text.
