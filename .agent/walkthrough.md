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
