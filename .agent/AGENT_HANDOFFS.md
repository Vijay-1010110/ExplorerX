# AI Agent Handoffs and Strategy

This document outlines the AI agent architecture and the rules for handoffs between different agents to ensure safety, reliability, and clear responsibility.

## AI Agent Strategy
AI should assist, not control raw file operations directly. Separate agents handle separate jobs.

### Workflow
**Intent Agent**
Understands the user request.
↓
**Planning Agent**
Converts intent into steps.
↓
**Tool Agent**
Uses allowed system APIs or internal services.
↓
**Validation Agent**
Checks if the result is safe and coherent before execution.

## Guardrails (Non-Negotiable)
- AI cannot silently delete files.
- AI cannot move files without confirmation.
- AI cannot access files outside permission scope.
- AI cannot bypass Windows security.
- AI cannot rewrite core metadata without validation.

## Natural Language Command Layer
Agents process commands like:
- “Show all videos from last week”
- “Find duplicate photos”
- “Open the project folder from yesterday”
- “Sort by file size and show only PDFs”

## Smart Search & Classification
Agents provide smart features:
- Infer intent, expand queries, map vague search terms to metadata filters.
- File classification (detect screenshots, invoices, source code, images, documents).
- Suggestions (suggest folders, cleanup, duplicate files, recent workspaces).
- Automation assistant (create saved workflows, batch rename rules, backup rules).
