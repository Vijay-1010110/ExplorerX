# Technical Decisions

This document captures the rationale for the technology stack, security model, and reliability requirements.

## Technology Stack
- **Core:** C++20
- **UI:** Qt 6
- **Rendering:** OpenGL or DirectX
- **DB:** SQLite
- **Windows APIs:** Win32 + COM + Shell APIs
- **Build:** CMake
- **Tests:** GoogleTest
- **Logging:** spdlog
- **Serialization:** nlohmann/json
- **AI:** local model via Ollama or custom inference service

## Security and Trust Model
- least privilege
- no silent elevation
- no hidden network access
- safe handling of paths
- safe path traversal checks
- safe deserialization
- signed updates
- plugin isolation
- protected secrets storage

**Sensitive Operations requiring confirmation:**
- delete, recursive delete, move outside scope, batch rename, network sync, admin-only operations

**Privacy (AI Integration):**
- make data use transparent, give opt-out, support local-only mode, separate cloud and local inference clearly.

## Reliability, Recovery, and Crash Safety
- autosave UI state
- session restore
- crash recovery
- operation journaling
- retry logic
- partial failure recovery
- corruption checks
- safe cancellation

*Example:* If a copy operation crashes at 72%, the app should know what was copied, what failed, what remains, and whether to resume or restart.

## What "Future-Proof" Means
Modular boundaries, versioned schemas, interface-based design, backward-compatible behavior, isolated AI, replaceable storage/index backends, replaceable UI skins, replaceable shell providers, telemetry-driven optimization, and migration scripts for every data change.
