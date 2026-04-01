# CrokyScopy 📺

[![Build Status](https://github.com/ennes-ns/CrokyScopy/actions/workflows/build.yml/badge.svg)](https://github.com/ennes-ns/CrokyScopy/actions)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

**A High-Performance, Split-Window Oscilloscope for VST3/JUCE.**

CrokyScopy goes beyond the traditional VST plugin design. It utilizes a **Split-Window Architecture**, meaning the main GUI acts only as a control panel, while the actual oscilloscope is rendered in a separate, borderless, semi-transparent "HUD" window that hovers over your DAW (inspired by FL Studio's *Wave Candy*).

## ✨ Features
- **Cycling Rewrite Scope:** Horizontally fixed scope that pages every 4, 8, or 16 beats, writing new data over a visible gap ("playhead").
- **Win32 Layered Transparency:** The HUD window uses native `WS_EX_LAYERED` to provide true per-pixel transparency and a click-through interface.
- **Edit Mode:** Toggle a graphical handle to easily drag, scale, and reposition your HUD around your screen.
- **Atomic & Lock-Free:** Built strictly around lock-free C++ atomic operations to ensure zero-allocation real-time safety on the audio thread.

## 💾 Installation (For Producers)
1. Navigate to the [Releases page](https://github.com/ennes-ns/CrokyScopy/releases) and download the latest `CrokyScopy_Windows.zip`.
2. Extract the `.vst3` file.
3. Move `CrokyScopy.vst3` into your VST3 plugins folder:
   - `C:\Program Files\Common Files\VST3`
4. Rescan your plugins in your DAW (Ableton, Bitwig, FL Studio, etc.).

> **Note**: Because CrokyScopy uses heavy Win32 window layer hacking to become fully transparent and click-through, it is currently a **Windows-only** plugin.

## 🚀 Building from Source (For Developers)
This project uses CMake and JUCE 8 (downloaded automatically via FetchContent).

1. Clone the repository:
   ```bash
   git clone https://github.com/ennes-ns/CrokyScopy.git
   cd CrokyScopy
   ```
2. Configure and build using CMake (Visual Studio 2022 recommended):
   ```bash
   cmake -B build -G "Visual Studio 17 2022"
   cmake --build build --config Release
   ```
3. Load the resulting `.vst3` file into your DAW.

## 📜 License
This project is licensed under the [MIT License](LICENSE).

---
_Developed by Ennes_
