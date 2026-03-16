# CrokyScope Architecture & Logic

This document explains the structure and internal workings of the CrokyScope VST3 plugin.

## 📁 Project Structure

```text
CrokyScope/
├── Source/
│   ├── PluginProcessor.h/cpp  # Audio logic & HUD management
│   ├── PluginEditor.h/cpp     # Main DAW UI (Parameters)
│   ├── HUDWindow.h/cpp        # Floating "Wave Candy" display
│   └── VUMeterComponent.h/cpp # Visual rendering (Upcoming)
├── CMakeLists.txt             # Build configuration (JUCE 8)
└── GEMINI.md                  # Project requirements (Prompt)
```

## 🏗️ Core Architecture: Split-Window Design

Unlike traditional plugins where the UI is tied to the DAW's editor window, CrokyScope uses a **Split-Window Design**:

1.  **AudioProcessor**: Acts as the "Brain". It owns a `std::unique_ptr<HUDWindow>`. Because the Processor lives as long as the plugin is loaded, the HUD window can stay open even if you close the main plugin UI in your DAW.
2.  **HUDWindow**: A custom `juce::DocumentWindow` that uses **Win32 Hacking** (`WS_EX_LAYERED`, `WS_EX_TOOLWINDOW`) to:
    - Become truly borderless and semi-transparent.
    - Hide its existence from the Windows Taskbar to stay "minimal".
    - Stay "Always on Top" using JUCE's native window management.

## 🔊 Audio Logic & Data Transfer

- **Envelope Follower**: Inside `processBlock`, we calculate the Peak/RMS levels of the incoming audio buffer.
- **Thread Safety**: We use `juce::Atomic<float>` to pass the volume level from the high-priority Audio Thread to the Low-priority UI Thread (the HUD). This prevents "glitches" or crashes.
- **Smoothing**: A release factor ($\tau$) is applied to ensure the meter falls back down smoothly instead of flickering instantly to zero.

## 🛠️ How it works (The "Win32 Hack")

In `HUDWindow.cpp`, we intercept the window handle (`HWND`) immediately after creation:
```cpp
HWND hwnd = (HWND)getWindowHandle();
LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
exStyle |= WS_EX_LAYERED | WS_EX_TOOLWINDOW; // The magic sauce
SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);
```
- `WS_EX_LAYERED`: Enables per-pixel alpha transparency (Glass effect).
- `WS_EX_TOOLWINDOW`: Prevents the window from appearing in the `Alt+Tab` menu or Taskbar, making it feel like a part of the desktop background.

## 🎛️ Parameters

All settings (Opacity, Scale, Color) are managed via `juce::AudioProcessorValueTreeState` (APVTS). This ensures that your HUD settings are saved within your DAW project and can be automated.
