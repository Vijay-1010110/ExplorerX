# Application Policies

This document outlines the strict operational policies governing extensibility, artificial intelligence, telemetry, and software updates for ExplorerX.

---

## 1. Plugin Policy

Extensibility is a core feature, but stability and security are paramount.
*   **Isolation:** All plugins (previewers, context menu handlers) must run in isolated processes or secure sandboxes. A plugin crash **must never** crash the main ExplorerX process.
*   **Permissions:** Plugins must declare their capabilities (e.g., "requires network access", "requires filesystem write"). The user must explicitly approve elevated permissions.
*   **Performance:** Plugins have strict execution timeouts. If a preview plugin fails to generate an image within 500ms, the request is aborted and a fallback icon is displayed.

## 2. AI Integration Policy

AI features are strictly designed to assist the user, never to execute destructive commands autonomously.
*   **Safety Guardrails:** The AI layer cannot perform `Delete`, `Move`, or `Format` operations without explicit user confirmation.
*   **Operation Scoping:** AI agents cannot bypass Windows security ACLs and can only view/modify files the current user has access to.
*   **Transparency:** Any action proposed by the AI (e.g., "Sort these invoices into folders by year") must be previewed as a dry-run before execution.

## 3. Privacy & Telemetry Policy

User data belongs to the user.
*   **No Silent Network Access:** The core file browser requires zero internet access to function.
*   **Local-First AI:** All file indexing and semantic search must be performed entirely on the local device via an embedded vector database or local LLM (e.g., Ollama).
*   **Telemetry Opt-In:** Telemetry (crash dumps, performance metrics) is strictly **opt-in**. No file names, file paths, or file contents are ever transmitted.

## 4. Update Policy

ExplorerX relies on a seamless, atomic update mechanism.
*   **Signed Releases:** All binaries and updates must be cryptographically signed.
*   **Atomic Updates:** Updates are downloaded in the background and applied atomically. A failed update must automatically rollback to the previous stable version.
*   **Data Migration:** Schema changes to the local SQLite databases (index, thumbnails, settings) must be governed by forward/backward-compatible migration scripts.
