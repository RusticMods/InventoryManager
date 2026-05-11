## Repository structure

InventoryManager now contains both sides of the mod:

* Client UI, menu logic, keybind handling, item browsing, player preview, and stringtable data
* Server authority, RPC handlers, config loading, permissions checks, inventory operations, notifications, and logging

A separate `InventoryManager_Server` repository or PBO is no longer required for current versions.

## Server-side behavior

The server side is responsible for validating requests and executing admin actions with authority.

Server logic handles:

* Creating and loading `InventoryManager.json`
* Registering InventoryManager RPC handlers
* Syncing the online player list to admins
* Looking up target players by player ID
* Returning inventory contents and position data to the client UI
* Clearing a player's inventory
* Repairing all items in a player's inventory
* Deleting a selected item
* Taking an item from one player and moving it to another
* Raising or lowering item health state
* Setting quantity on stackable items
* Spawning items in inventory or at a world position
* Optional spawning of compatible attachments
* Teleporting the requesting admin to a selected player
* Sending notifications back to the admin client
* Writing action logs through RusticModsCore

## Server-side actions

InventoryManager implements server-side logic for actions such as:

* `ClearInventoryRPC`
* `RepairAllRPC`
* `DeleteItemRPC`
* `TakeItemRPC`
* `LowerHealthRPC`
* `RaiseHealthRPC`
* `SetItemRPC`
* `SpawnItemRPC`
* `TeleportPlayerRPC`

## Spawn behavior

The item spawn flow supports:

* Validation of classname, quantity, health state, and destination
* Spawning directly into a player's inventory
* Spawning in the world at a position
* Optional attachment spawning for compatible items
* Exclusion handling for restricted or unfinished classes in the attachment logic