# Product and Technical Requirements

## 1. Feature Specification

ExplorerX aims to be a complete replacement for Windows Explorer, providing a modern, high-performance, and extensible file management experience.

### Must-Have Features (MVP)
*   **Navigation:** Multi-tabbed browsing, address bar, breadcrumb navigation, and an intuitive tree view.
*   **File Operations:** Robust, asynchronous copy, move, rename, and delete operations with comprehensive conflict resolution and rollback capabilities.
*   **Views:** Support for grid/details, large icons, thumbnails, and preview panes.
*   **Search:** Instant local folder search and progressive indexed search utilizing SQLite.
*   **Integration:** Full support for Windows Shell extensions (context menus, properties, "Open With").

### Advanced Features (Post-MVP)
*   Semantic AI-powered natural language search.
*   Plugin architecture for custom previewers and right-click actions.
*   Advanced archiving support natively within the navigation tree.

---

## 2. Environment Matrix

### Supported Operating Systems
ExplorerX is designed to run natively on modern Windows architectures.
*   **Windows 11** (All versions)
*   **Windows 10** (Version 21H2 and newer)
*   *Note: Windows 7, 8, and early builds of Windows 10 are strictly unsupported.*

### Supported File Systems
The core engine guarantees robust enumeration, path normalization, and operation handling on the following file systems:
*   **NTFS:** Full support, including alternate data streams (ADS), reparse points (symlinks/junctions), and ACLs.
*   **ReFS:** Full support for large data volumes.
*   **FAT32 / exFAT:** Full support, primarily targeting removable USB storage and external drives.

---

## 3. Non-Functional Requirements (NFRs)

### Performance Targets
*   **Directory Open Latency:** Opening a directory with up to 100,000 files must take `< 200ms`.
*   **UI Rendering:** The UI must maintain 60 FPS. Rendering must utilize UI virtualization, ensuring that only visible items are drawn and kept in memory.
*   **Search Response:** Local prefix search must yield initial results within `< 50ms`.

### Resilience and Crash Safety
*   **Massive Directories:** The application must never freeze or trigger an "Application Not Responding" state, regardless of the folder size or network latency.
*   **Crash Recovery:** In-flight operations (like a massive file copy) must be journaled. If the application crashes or power is lost, ExplorerX must be able to resume or rollback the operation upon restarting.
*   **Resource Bounds:** Worker threads for asynchronous tasks (e.g., thumbnail extraction) must be strictly bounded to avoid starving the host OS.
