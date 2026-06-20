# ExplorerX AI Agent Workflow

This document outlines the roles and responsibilities of the specialized AI agents in the ExplorerX project, as well as the general workflow for implementing new features or fixing bugs.

## Agent Taxonomy

1. **ArchitectAgent**
   - **Scope:** Architecture governance, API design, roadmap, module management.
   - **Responsibilities:** Define system boundaries, document design decisions, enforce project standards.

2. **CoreAgent**
   - **Scope:** `src/Core/` and `src/Domain/`
   - **Responsibilities:** Platform-independent business layer, domain models, use cases, services, result patterns.

3. **PlatformAgent**
   - **Scope:** `src/Platform/` and `src/Infrastructure/WinFS.*`
   - **Responsibilities:** Windows-specific filesystem functionality (Win32, COM, Shell APIs, file operations).

4. **IndexAgent**
   - **Scope:** `src/Infrastructure/Index.*`
   - **Responsibilities:** Search and indexing engine using SQLite and FTS5, database schemas, migrations, query planning.

5. **UIAgent**
   - **Scope:** `src/UI/`
   - **Responsibilities:** Qt views, viewmodels, navigation, commands, themes, layouts, accessibility.

6. **TestAgent**
   - **Scope:** `tests/`
   - **Responsibilities:** Unit, integration, stress, recovery, and performance testing.

7. **BuildAgent**
   - **Scope:** Build & Release Infrastructure
   - **Responsibilities:** CMake configuration, vcpkg dependency management, CI/CD pipelines, release automation, Git workflow enforcement, and version tracking.

8. **ReviewAgent**
   - **Scope:** Code Review & QA
   - **Responsibilities:** Architectural compliance, security auditing, and performance review for all produced code.

## General Workflow

1. **Prompt Engineering (My Role):** You provide a feature request. I break it down into sub-tasks and write precise prompts for each specialized agent.
2. **Version Control:** Create a new Git branch for the task (e.g., `feature/task-name` or `fix/issue-name`) to track all changes.
3. **Architecture & Design:** `@ArchitectAgent` defines the API contracts and updates the design docs.
4. **Domain Implementation:** `@CoreAgent` implements the core logic and interfaces.
5. **Infrastructure Implementation:** `@PlatformAgent` and/or `@IndexAgent` implement the specific infrastructure details (Win32, SQLite).
6. **UI Implementation:** `@UIAgent` connects the views to the core models/viewmodels.
7. **Testing:** `@TestAgent` writes and executes tests for the new code.
8. **Build & Deploy:** `@BuildAgent` updates CMake/vcpkg if new dependencies or targets are added.
9. **Review:** `@ReviewAgent` signs off on the code.
10. **Commit:** Commit changes locally only when a significant unit of work or a phase is fully completed and working. Avoid committing minor, incomplete changes.
11. **Push:** Do not push to the remote repository until a major milestone or phase is finished and stable.
