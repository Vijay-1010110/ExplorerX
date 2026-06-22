# System Modules

## Filesystem Features
- **Implement Early:** NTFS basics, FAT32/exFAT volume detection, symlink detection, junction detection, reparse point classification, removable drive handling, network drive handling, UNC path handling.
- **Implement Later:** Alternate data streams, ACLs and ownership, encryption flags, compression flags, hard links, mount points, sparse files, file attributes, known folders, shell namespace items.

## Search and Indexing Architecture
- **Level 1:** Immediate local folder search.
- **Level 2:** Incremental indexed search.
- **Level 3:** Full content index for supported file types.
- **Level 4:** Semantic AI search over file names, metadata, and optionally content.
- **Index Design:** SQLite with path, parent, name, size, timestamps, volume, reparse classification, etc.
- **Search Features:** Prefix, fuzzy, full text, filters, date ranges, size ranges, tags, file type, path scope, saved searches.
- **Indexer Rules:** Incremental updates, watch-based invalidation, backfill scanning, throttling on battery, pause when busy, resume after crash.

## Thumbnail, Preview, and Shell Integration
- **Thumbnail Engine:** Images, videos, documents, archives. Cache with expiry, size tiers, device scaling.
- **Preview Engine:** Text, image, metadata, PDF, video, audio metadata, code, 3D models.
- **Shell Integration:** Standard context menus, file properties, open with, send to, pinned locations, recycle bin, known folders, default handlers, shell item identifiers.

## Data Model and Schema Planning
- **Storage Layers:** user settings DB, file index DB, thumbnail cache DB, recent locations DB, session state DB, plugin registry, action history DB, crash recovery state.
- **Schema Rules:** Version every schema, support migration scripts, build forward-compatible readers, write migration tests.

## UI Assets and Resources
- **Qt Resource System:** `icons.qrc` bundling modular SVG assets (Material Design/Fluent UI) under `:/icons/`.
- **SVG Standardization:** Modern SVGs replace legacy Win32 default icons for standard file types (folder, document, image, etc.).
