# Architecture Rules

These non-negotiable engineering principles keep the app future-proof.

## Principle A: Separate Layers Hard
Do not mix UI, domain logic, and Windows API calls.

**Architecture Split:**
- UI (Presentation Layer)
  ↓
- Application Layer
  ↓
- Domain/Core Layer
  ↓
- Platform Abstraction Layer
  ↓
- Windows API (Win32 / COM / Shell)

## Principle B: Everything is Async By Default
Any operation that can block must be asynchronous:
- enumeration
- thumbnails
- search
- copy
- metadata extraction
- network access
- preview generation
- indexing

## Principle C: Cache Everywhere, but Invalidate Correctly
Required caches:
- directory cache
- icon cache
- thumbnail cache
- metadata cache
- search index cache
- shell item cache
- ACL/security cache where appropriate

## Principle D: Stable Contracts Only
Every module talks through interfaces, not concrete classes (see `API_CONTRACTS.md`).

## Principle E: Every Expensive Feature Gets Instrumentation
Must measure:
- enumeration latency
- render latency
- search latency
- thumbnail generation time
- copy throughput
- cache hit rate
- crash rate

## Suggested Layered Design

**Presentation Layer**
- Views, ViewModels / Presenters, Commands, Theme Engine

**Application Layer**
- Navigation Service, File Operation Orchestrator, Search Orchestrator, Thumbnail Orchestrator, Selection / Clipboard Service, Settings Service

**Domain Layer**
- File Item Model, Folder Model, Operation Models, Query Models, Error Models

**Infrastructure Layer**
- Windows File System Adapter, Shell Adapter, Thumbnail Provider, Index Database, Logging, Telemetry, Update Service

**Platform Layer**
- Win32, COM / Shell, NTFS / reparse handling, network resources, removable media
