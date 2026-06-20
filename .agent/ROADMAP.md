# Project Roadmap

## Phase 0: Product and Technical Specification
Deliverables: feature specification, non-functional requirements, supported Windows versions, file system support matrix, shell integration scope, plugin policy, AI policy, privacy policy, crash recovery policy, telemetry policy, update policy.

## Phase 1: Core System Foundation
- 1.1 Path and Item Model (Path, FileId, DirectoryId, VolumeId, FileItem, FolderItem, DriveItem)
- 1.2 Error Model (structured errors)
- 1.3 Result Model (explicit success/failure)
- 1.4 Logging and Diagnostics (structured logs, tracing, dumps)
- 1.5 Config System (local settings, profile storage, theme config, cache policies, feature flags)

## Phase 2: File System Engine
- 2.1 Directory Enumeration (huge folders, pagination, cancellation, sorting)
- 2.2 Metadata Extraction (size, timestamps, attributes, file type, permissions)
- 2.3 File Operations (copy, move, delete, rename, progress events, conflict resolution, rollback)
- 2.4 Path Normalization (UNC, junctions, long paths)
- 2.5 File System Watching (change notifications)

## Phase 3: UI Framework and Navigation Shell
- 3.1 Main Window (tabs, address bar, split panes)
- 3.2 Views (tree, grid/details, preview, properties)
- 3.3 Virtualization (render only visible rows)
- 3.4 Interaction Model (drag/drop, multi-select, keyboard navigation)

## Performance Roadmap
- **Targets:** folder open < 200ms, UI responsive for huge directories, async thumbnails.
- **Techniques:** asynchronous enumeration, background indexing, cached trees, virtualization, bounded worker pools, prefetching.

## Testing Strategy
- **Unit Tests:** path normalization, migrations, name validation.
- **Integration Tests:** file operations, watcher behavior, index updates.
- **UI Tests:** navigation, shortcuts, drag/drop.
- **Stress Tests:** 100k files, long paths, low disk space, removable drive unplug.
- **Recovery Tests:** crash during copy, crash during index rebuild.
- **Compatibility Tests:** Windows versions, NTFS/FAT32, mixed locale filenames.

## Suggested Development Order (Milestones)
- **M1:** Core file engine in console app
- **M2:** Windows adapter layer with directory enumeration and file ops
- **M3:** SQLite index and search prototype
- **M4:** Qt shell with navigation and file list
- **M5:** Async copy/move/delete UI
- **M6:** Thumbnail and preview pipelines
- **M7:** Tabs, history, bookmarks, quick access
- **M8:** Themes and glass/crystal effects
- **M9:** Shell integration and context menus
- **M10:** AI assistant layer for commands and search
- **M11:** Plugin system and extensibility
- **M12:** Packaging, updates, crash recovery, telemetry, release readiness
