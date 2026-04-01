# CrokyScopy 🚀 (Atomic Vision)

A high-performance, minimalist, and transparent oscilloscope VST3 built with **JUCE 8** and **Direct2D**.

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![JUCE 8](https://img.shields.io/badge/JUCE-8.0.0-blue.svg)
![C++20](https://img.shields.io/badge/Language-C%2B%2B20-orange.svg)

## ✨ Features
- **Atomic Vision Engine**: Lock-free, binned oscilloscope buffer for CPU-friendly visualization.
- **Floating HUD**: Transparent, borderless, and always-on-top waveform display.
- **Direct2D Rendering**: GPU-accelerated drawing on Windows for buttery smooth animations.
- **DAW Integration**: No custom main editor—control everything from your DAW's native parameter panel.
- **Persistence**: Remembers window position, size, and appearance across sessions.

## 🛠️ Build Requirements
- **Windows 10/11**
- **Visual Studio 2022/2026** (with C++ development workload)
- **CMake 3.22+**

## 🚀 Building from Source
```bash
git clone https://github.com/your-username/CrokyScope.git
cd CrokyScope
cmake -B build -S . -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```

## 📂 Project Structure
- `Source/Core`: High-performance data structures (`ScopeBuffer`).
- `Source/DSP`: Audio processing and parameter management.
- `Source/UI`: HUD window and Direct2D rendering components.
- `Source/Common.h`: Shared types and global state.
- `.gemini`: Project-specific discoveries and workflow logs.

## ⚖️ License
MIT License. Created by Ennes.
