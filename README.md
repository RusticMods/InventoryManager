# InventoryManager

InventoryManager is an inventory administration mod for DayZ. It includes the admin UI, client interaction layer, server authority, RPC handling, inventory manipulation, permissions integration, notifications, configuration, and logging needed to inspect and manage player inventories from a single mod package.

When an admin needs to fix a broken loadout, clean up a bad spawn, inspect a player, or handle inventory issues, InventoryManager gives them the tools to do it in-game.

This project is open source under **GPLv3**.

## What it does

InventoryManager gives authorized administrators an in-game interface for managing online player inventories.

The mod currently supports:

* Opening an admin inventory management UI with a keybind
* Viewing a synchronized list of online players
* Loading a target player's inventory into a navigable menu
* Searching item classnames
* Toggling player preview in the UI
* Spawning items directly into a player's inventory
* Spawning items at a player's position
* Optional spawning of compatible attachments when creating items
* Clearing a player's inventory
* Repairing all items in a player's inventory
* Adjusting item quantity for stackable items
* Raising or lowering item health state
* Deleting items from a target player's inventory
* Taking items from a target player
* Teleporting to the selected player
* Sending notifications back to the admin
* Logging admin actions through RusticModsCore
* Localized UI text through `stringtable.csv`

## Permissions

InventoryManager is restricted to authorized administrators.

Permissions are supplied by **RusticModsCore** through:

```text
profiles/RusticMods/Permissions/users.json
```

Players must be listed in `SuperAdmins`.

Example:

```json
{
  "SuperAdmins": [
    "YourPlainIdHere"
  ]
}
```

## Configuration

On first start, the mod creates:

```text
profiles/RusticMods/InventoryManager.json
```

Current config fields:

* `EnableDebugMode`
* `WebhookLogs`

### Default generated values

```json
{
  "EnableDebugMode": "false",
  "WebhookLogs": "url (EVERYTHING AFTER https://discord.com/api/webhooks/ (Ex: https://discord.com/api/webhooks/12312541 would be 12312541))"
}
```

Note: `WebhookLogs` was a pre-RusticModsCore implementation. It was reserved for possible future use, but RusticModsCore now handles logging more completely. Use RusticModsCore logging unless you intentionally build something around this config entry.

## Required mods

Install these alongside InventoryManager:

* `RusticModsCore`
* `CF`

`RusticModsCore` is required for permissions, logging, and shared Rustic support functionality.

## Installation

1. Click `Code`, then `Download ZIP`.
2. Extract the folder inside the ZIP file into your P drive.
3. Remove `-main` from the folder name.

Example:

```text
InventoryManager-main
```

should become:

```text
InventoryManager
```

4. Pack the folder into a PBO using AddonBuilder or another packing tool.
5. Sign the PBO using DS Utils.
6. Upload the mod to your server pack or Steam Workshop.
7. Load the mod on both the server and clients.
8. Make sure `RusticModsCore` and `CF` are also installed.
9. Add authorized admin IDs to:

```text
profiles/RusticMods/Permissions/users.json
```

No separate `InventoryManager_Server` PBO is required for current versions.

## Admin setup

After installing the mod, add each authorized admin's plain ID to the `SuperAdmins` list in RusticModsCore permissions.

Example:

```json
{
  "SuperAdmins": [
    "76561198000000000"
  ]
}
```

Only players recognized as admins by RusticModsCore should be able to open and use the InventoryManager UI.

## License

This project is licensed under the **GNU General Public License v3.0**.

See `LICENSE-GPL-3.0.txt` for the full license text.

## Steam Workshop redistribution

This project is licensed under the GNU General Public License v3.0.

You may upload, repost, fork, modify, or redistribute this project on Steam Workshop only if you follow the GPLv3 license terms.

Any Steam Workshop upload or derivative version must:

* Credit the original author: Rustic
* Link to the original repository: [InventoryManager](https://github.com/RusticMods/InventoryManager/)
* Include a clear GPLv3 license notice
* Provide access to the complete corresponding source code for the uploaded version
* Clearly state whether changes were made
* Preserve copyright and license notices
* Not imply endorsement by the original author unless permission was explicitly granted

Suggested attribution:

> Based on InventoryManager by Rustic, licensed under GPLv3. Original source: [https://github.com/RusticMods/InventoryManager/](https://github.com/RusticMods/InventoryManager/). Modified version source: [Your Modified Source URL].