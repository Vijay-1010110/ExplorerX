# Theme Engine Walkthrough

## Summary
The Theme Engine is now fully operational and integrated with the ExplorerX UI!

### Key Features
1. **Windows 11 Mica/Acrylic Glassmorphism**:
   We activated `EnableWindowBlur`, injecting the native OS glass backdrop directly onto the `winId()` of the application.
2. **Dynamic QSS Loading**:
   The `ThemeManager` now safely reads and provisions QSS themes (like `dark.qss`) natively from the Qt Resource System (`themes.qrc`) and pushes them to `qApp->setStyleSheet()`.
3. **Custom Background Rendering**:
   When users pick custom background images, the `MainWindow::paintEvent` override bypasses traditional styles and dynamically paints scaled images directly to the canvas behind all frosted widgets!

---

# AI Command Bar Walkthrough

## Summary
The UI has now successfully wired the central AI Orchestrator into the new AI Command box!

### How to Use the AI Command Bar
1. **Focus the Bar**: Click the "Ask the AI to do something..." text box in the main application toolbar.
2. **Type Natural Language**: Instruct ExplorerX on what you want (e.g., *"Find all C++ files and move them to the Backup folder"* or *"Search for 'Invoice' across my documents"*).
3. **Execute**: Press `Enter`. The UI will temporarily disable the box to prevent spam, run the command asynchronously without freezing the app, and re-enable itself when done.
4. **View Results**: If your intent was a "Search", the `SearchResultsReady` signal fires, dynamically injecting the resulting files right into your grid view!
