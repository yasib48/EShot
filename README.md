# EShot

EShot is a modern, fast, and lightweight screenshot tool for Windows, inspired by Lightshot and Flameshot. It is developed using C++, Qt 6, and Windows Native API.

## Features

*   **Fast and Lightweight:** Minimal system resource usage.
*   **Advanced Annotation Tools:** Pen, Arrow, Rectangle, **Circle**, Text, Highlighter, Blur, and **Counter (#)**.
*   **Modern Interface:** Sleek design, animations, and **Dark Mode** support.
*   **Pin to Desktop:** Pin the selected area to the screen (Always-on-top window).
*   **Easy Selection:** Move the selection area using `Ctrl` + Drag.
*   **Automatic Saving:** Customizable filename templates (`%Y-%m-%d`, etc.) and save path.
*   **Clipboard Integration:** Options to automatically copy to clipboard after capture.
*   **Multi-Monitor DPI Support:** Pixel-perfect capture using Windows Native API (`BitBlt`).

## Installation and Build

To develop or build this project, you need the following tools:

### Requirements
*   **CMake** (3.16 or later)
*   **Qt 6.x** (Core, Gui, Widgets modules)
*   **Visual Studio Build Tools 2022** (or compatible MSVC compiler)

### Build Steps

1.  Clone the repository:
    ```bash
    git clone https://github.com/Benoks/EShot.git
    cd EShot
    ```

2.  Create a build directory and run CMake:
    ```bash
    mkdir build
    cd build
    cmake ..
    ```

3.  Build the project (in Release mode):
    ```bash
    cmake --build . --config Release
    ```

4.  The executable will be located at `build/bin/Release/EShot.exe`.

## Usage

*   **PrtSc (Print Screen):** Starts screenshot mode.
*   **Left Click + Drag:** Selects an area.
*   **Toolbar:** Appears after selection (Pen, Arrow, etc.).
*   **Settings:** Accessible by right-clicking the System Tray icon.
*   **Pinning:** Click the "Pin" icon in the toolbar to pin the selection to the screen.
*   **Resizing:** Drag the white handles at the corners of the selection to resize the capture area.

### Shortcuts
| Key | Function |
| :--- | :--- |
| `PrtSc` | Start Capture |
| `Enter` / `Ctrl+C` | Copy Selection |
| `Ctrl+S` | Save |
| `Esc` | Cancel / Close |
| `Ctrl+Z` | Undo |
| `Ctrl+Y` | Redo |
| `Shift` (While Drawing) | Perfect Square / Circle |
| `Ctrl` + Drag / Drag Selection | Move Selection Area |

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.

## License

This project is licensed under the [MIT](LICENSE) license.
