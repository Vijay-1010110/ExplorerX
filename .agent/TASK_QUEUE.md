# Task Queue

This queue is derived from the Roadmap phases and represents actionable items.

- [x] **Phase 0:** Draft feature specification & non-functional requirements.
- [x] **Phase 0:** Decide on supported Windows versions, file system matrix.
- [x] **Phase 0:** Draft Plugin, AI, Privacy, and Update policies.
- [x] **Phase 1.1:** Implement Path and Item model (`Path`, `FileId`, `FileItem`, etc.).
- [x] **Phase 1.2:** Define Error model (`access denied`, `path not found`, etc.).
- [x] **Phase 1.3:** Implement Result model (`Expected<T>`).
- [x] **Phase 1.4:** Setup logging framework (`spdlog`) and diagnostics.
- [x] **Phase 1.5:** Build config system (JSON-based settings, cache policies).
- [x] **Phase 2.1:** Implement asynchronous directory enumeration.
- [x] **Phase 2.2:** Implement metadata extraction (timestamps, attributes).
- [x] **Phase 2.3:** Build file operations (copy, move, delete) with progress events.
- [x] **Phase 2.4:** Implement path normalization (UNC, reparse points).
- [x] **Phase 2.5:** Setup file system watcher.
- [x] **Phase 3.1:** Setup Qt 6 main window layout.
- [x] **Phase 3.2:** Implement Core Views (tree view, file grid).
- [x] **Phase 3.3:** Implement UI Virtualization for file lists.
- [x] **Phase 3.4:** Add interaction models (drag/drop, shortcuts).
- [x] **Milestone 3:** SQLite index integration.
- [x] **Milestone 6:** Thumbnail and Preview engine.
- [x] **Milestone 10:** AI Command and Intent agent integration.

### Future Polish
- [x] Fix Windows default icon fallback in grid view (Scaffolded SVG Resource system)
- [x] Custom SVG UI implementation
- [x] Build Core Theme Manager service and QSS architecture
- Scope search query to current directory
