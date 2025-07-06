# 🪟 JP Window Module

Native window creation and management for the JP programming language.

## Features

- ✅ **Cross-platform** - Windows (Win32), Linux (X11), macOS (Cocoa)
- ✅ **Native performance** - Direct OS API calls
- ✅ **Simple API** - Easy to use functions
- ✅ **Input handling** - Keyboard input detection
- ✅ **Event-driven** - Proper message loop handling

## Installation

```bash
jp install window
```

## Quick Start

```jp
import window

// Create and show window
win = createWindow("My App", 800, 600)
showWindow(win)

// Main loop
while pollEvents(win) {
    if isKeyPressed(VK_ESCAPE) {
        break
    }
}

// Cleanup
destroyWindow(win)
```

## API Reference

### Window Management

#### `createWindow(title, width, height)`
Creates a new window.

**Parameters:**
- `title` (string): Window title
- `width` (int): Window width in pixels  
- `height` (int): Window height in pixels

**Returns:** Window handle (void*)

**Example:**
```jp
win = createWindow("My Game", 1024, 768)
```

#### `showWindow(window)`
Shows the window on screen.

**Parameters:**
- `window` (void*): Window handle

**Example:**
```jp
showWindow(win)
```

#### `pollEvents(window)`
Processes window events. Must be called in main loop.

**Parameters:**
- `window` (void*): Window handle

**Returns:** `false` if window should close, `true` to continue

**Example:**
```jp
while pollEvents(win) {
    // Game loop
}
```

#### `destroyWindow(window)`
Destroys window and frees resources.

**Parameters:**
- `window` (void*): Window handle

**Example:**
```jp
destroyWindow(win)
```

#### `setWindowTitle(window, title)`
Changes the window title.

**Parameters:**
- `window` (void*): Window handle
- `title` (string): New title

**Example:**
```jp
setWindowTitle(win, "New Title")
```

### Input Handling

#### `isKeyPressed(keyCode)`
Checks if a key is currently pressed.

**Parameters:**
- `keyCode` (int): Virtual key code

**Returns:** `true` if key is pressed

**Example:**
```jp
if isKeyPressed(VK_SPACE) {
    jump()
}
```

### Virtual Key Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `VK_SPACE` | 32 | Space bar |
| `VK_ENTER` | 13 | Enter key |
| `VK_ESCAPE` | 27 | Escape key |
| `VK_LEFT` | 37 | Left arrow |
| `VK_UP` | 38 | Up arrow |
| `VK_RIGHT` | 39 | Right arrow |
| `VK_DOWN` | 40 | Down arrow |

## Examples

### Basic Window
```jp
import window

win = createWindow("Basic Window", 640, 480)
showWindow(win)

while pollEvents(win) {
    // Window is running
}

destroyWindow(win)
```

### Game Loop with Input
```jp
import window

win = createWindow("Game", 800, 600)
showWindow(win)

player_x = 400
player_y = 300

while pollEvents(win) {
    // Input handling
    if isKeyPressed(VK_LEFT) {
        player_x = player_x - 5
    }
    if isKeyPressed(VK_RIGHT) {
        player_x = player_x + 5
    }
    if isKeyPressed(VK_UP) {
        player_y = player_y - 5
    }
    if isKeyPressed(VK_DOWN) {
        player_y = player_y + 5
    }
    
    // Exit on ESC
    if isKeyPressed(VK_ESCAPE) {
        break
    }
    
    // Update window title with player position
    title = "Player: (" + player_x + ", " + player_y + ")"
    setWindowTitle(win, title)
}

destroyWindow(win)
```

### Multiple Windows
```jp
import window

// Create multiple windows
win1 = createWindow("Window 1", 400, 300)
win2 = createWindow("Window 2", 400, 300)

showWindow(win1)
showWindow(win2)

// Both windows running
while pollEvents(win1) and pollEvents(win2) {
    if isKeyPressed(VK_ESCAPE) {
        break
    }
}

destroyWindow(win1)
destroyWindow(win2)
```

## Platform Support

### Windows
- **Requirements:** Windows 7 or later
- **Dependencies:** Built-in Win32 APIs (gdi32, user32, kernel32)
- **Compiler:** MinGW-w64 or Visual Studio

### Linux
- **Requirements:** X11 display server
- **Dependencies:** `sudo apt install libx11-dev`
- **Compiler:** GCC or Clang

### macOS
- **Requirements:** macOS 10.10 or later
- **Dependencies:** Cocoa framework (built-in)
- **Compiler:** Xcode command line tools

## Building from Source

1. Clone the repository
2. Ensure dependencies are installed
3. Use JP package manager:
```bash
jp install window
```

## License

MIT License - see LICENSE file for details.

## Contributing

1. Fork the repository
2. Create feature branch
3. Add tests for new functionality
4. Submit pull request

## Changelog

### v1.0.0
- Initial release
- Windows Win32 support
- Linux X11 support  
- Basic input handling
- Window management functions
