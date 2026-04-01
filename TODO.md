# CrokyScopy TODO 📝

Next steps for the Wave Candy Style Plugin:

## 🔊 Audio & DSP
- [ ] **Fix Meter Levels**: Audio data is being processed but not yet visually represented or correctly mapped to the HUD.
- [ ] **Refine Envelope Follower**: Implement proper RMS and Peak detection with the smoothing factor ($\tau$ from APVTS).

## 🛠️ Debug & UI
- [ ] **Debug Menu**: Create a dedicated section in the main plugin window to monitor internal values (e.g., current RMS level, HUD window handle status).
- [ ] **Easier Window Movement**: Temporarily improve the window dragging logic so it's less fiddly to reposition while we are testing.
- [ ] **HUD Visuals**: Implement the vertical VU-meter bar or glow effect in the `HUDWindow`.

## ⚙️ Features
- [ ] **Resize Handles**: Add the mouse-over resize handles in the bottom-right corner of the HUD.
- [ ] **Color Customization**: Bind the accent color parameter to the meter rendering.
- [ ] **Persistence**: Ensure the HUD window position (X, Y) is correctly saved and restored from the APVTS.

