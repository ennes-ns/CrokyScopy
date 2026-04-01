# CrokyScopy 📺

**A High-Performance, Split-Window Oscilloscope for VST3/JUCE.**

CrokyScopy goes beyond the traditional VST plugin design. It uses a **Split-Window Architecture**, meaning the main GUI acts only as a control panel, while the actual oscilloscope is rendered in a separate, borderless, semi-transparent "HUD" window (similar to FL Studio's *Wave Candy*).

## ✨ Features
- **Cycling Rewrite Scope:** Horizontally fixed scope that pages every 4, 8, or 16 beats, writing new data over a visible gap ("playhead").
- **Win32 Layered Transparency:** The HUD window uses native `WS_EX_LAYERED` to provide true per-pixel transparency and a click-through interface.
- **Edit Mode:** Toggle a graphical handle to easily drag, scale, and reposition your HUD around your screen.
- **Atomic & Lock-Free:** Built strictly around lock-free C++ atomic operations to ensure zero-allocation real-time safety on the audio thread.

## 🚀 Building the Project

This project uses CMake and JUCE 8 (downloaded automatically via FetchContent).

1. Clone the repository:
   ```bash
   git clone https://github.com/ennes-ns/CrokyScopy.git
   cd CrokyScopy
   ```
2. Configure and build (using your preferred CMake generator, e.g., Visual Studio or Ninja):
   ```bash
   cmake -B build
   cmake --build build --config Release
   ```
3. Load the resulting `.vst3` file into your DAW.

## 🧪 Testing & Documentation
CrokyScopy separates visual elements from audio calculations. The `ScopeBuffer` handles the lock-free mapping of audio samples to visual "bins" based on the DAW's PPQ (Pulse Position Quarter-note) clock.
- **Docs:** Inline code documentation is written in Doxygen style.
- **Unit Tests:** Run the included `ScopeBufferTest` to verify that PPQ-to-Bin mapping handles tempo shifts and varying sample rates correctly.

---
_Developed by Ennes_
