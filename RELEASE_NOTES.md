# EShot v2.1.0

## What's New

- 7-language support (Turkish, English, German, French, Spanish, Japanese, Chinese)
- OCR text extraction from captured screenshots (Windows.Media.Ocr API)
- First-run wizard for guided setup (language, hotkey, save path)
- Pinned window corner-handle resize (aspect-ratio locked)
- Pinned window opacity control (scroll wheel, Shift+Scroll for zoom)
- Pinned window "Save As" from right-click context menu
- Settings export/import as JSON file
- Live theme switching (dark/light/high-contrast) without restart
- High contrast accessibility mode
- Customizable tray icon (dark/light)
- Settings tooltips on all options
- Command-line arguments (--capture, --save)
- Auto-update check from GitHub releases
- OCR button with dedicated icon
- About dialog now translated for all languages

## Fixes

- Language setting always reverting to Turkish after restart (string vs int mismatch)
- copyAfterCapture not working (was copying at wrong time)
- closeAfterCopy setting loaded but never applied
- %T filename variable always empty (overlay was foreground window)
- Undo/redo stacks cleared on new annotation (now preserved)
- Erased annotations not undoable (now pushed to redo stack)
- Opacity label showing "39%" at startup instead of "0%"
- Close All Pins tray action not working (signal connection missing)
- Version mismatch between CMakeLists.txt and application
- About dialog description hardcoded to Turkish/English only
- checkForUpdates() never called on startup

## Removed

- Imgur cloud upload (unreliable)
- Window capture mode (overlay blocked detection)
- "Restart required" text from dark mode checkbox

## Supported Languages

| Language | Code |
|----------|------|
| Türkçe | tr |
| English | en |
| Deutsch | de |
| Français | fr |
| Español | es |
| 日本語 | ja |
| 中文 | zh |

---

**Full Changelog:** https://github.com/Benoks/EShot/compare/v2.0.0...v2.1.0
