# CrokyScope 🚀 (Wave Candy Style HUD)

A high-performance, minimalist, and transparent oscilloscope VST3 built with **JUCE 8** for Windows. Inspired by Image-Line's Wave Candy.

![JUCE 8](https://img.shields.io/badge/JUCE-8.0.1-blue.svg)
![C++20](https://img.shields.io/badge/Language-C%2B%2B20-orange.svg)
![OS](https://img.shields.io/badge/OS-Windows-brightgreen.svg)

## ✨ Features
- **Floating Transparent HUD**: Always-on-top, borderless waveform display that floats on your desktop.
- **Sweep Oscilloscope (Page Scroll)**: High-performance, sweep-style visualization that syncs with your DAW's BPM.
- **Lock-Free DSP**: 100% thread-safe data transfer between the audio engine and the GUI using atomic buffers.
- **Interactive Edit Mode**: 
  - **Drag**: Move the HUD anywhere on your screens.
  - **Resize**: Drag the bottom-right corner to change the size dynamically.
- **Customizable Appearance**: Adjust opacity, line width, and color hue via the VST editor.
- **Click-Through**: In normal operation, the HUD is invisible to the mouse, allowing you to work in your DAW underneath it.
- **Persistence**: Remembers window position, size, and settings across sessions.

## 🛠️ Build Requirements
- **Windows 10/11**
- **Visual Studio 2026** (or 2022)
- **CMake 3.22+**

## 🚀 Building from Source
The project uses CMake to automatically fetch JUCE 8.
```powershell
git clone https://github.com/ennes-ns/CrokyScopy.git
cd CrokyScope
cmake -B build -S .
cmake --build build --config Release
```

## 📂 Project Structure
- `src/PluginProcessor.cpp / .h`: Audio processing, APVTS management, and lock-free data streaming.
- `src/PluginEditor.cpp / .h`: VST3 main menu and HUD lifecycle management.
- `src/ScopeHUDWindow.cpp / .h`: The transparent OS-level window and sweep rendering engine.
- `CMakeLists.txt`: Project build configuration and JUCE dependency fetching.

## ⚖️ License
MIT License. Created by Ennes.
