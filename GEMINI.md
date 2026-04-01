Prompt voor Antigravity: Wave Candy Style Plugin Architectuur
Doel: Ontwikkel een Windows-only VST3 plugin in C++ met het JUCE-framework, die een extern, zwevend, randloos en semi-transparant displayvenster (HUD) opent voor een VU-meter, los van de standaard plugin-editor.

1. Architectuur Vereisten
Split-Window Design: De AudioProcessor beheert een std::unique_ptr naar een extern venster (HUDWindow). Dit venster moet blijven bestaan, ook als de standaard AudioProcessorEditor (de GUI van de DAW) wordt gesloten.
HUDWindow Klasse (De "Wave Candy" Display):
- Overerf van juce::DocumentWindow.
- Verwijder alle native decoraties: setUsingNativeTitleBar(false).
- Maak het venster "Always on Top": setAlwaysOnTop(true).
- Stel de achtergrond in op juce::Colours::transparentBlack.
- Win32 Hack: Implementeer WS_EX_LAYERED en WS_EX_TOOLWINDOW via de HWND om de toolbar in de Windows-taakbalk te verbergen en per-pixel transparantie mogelijk te maken.
- Interactie: Implementeer een juce::ComponentDragger zodat het venster versleept kan worden door op de GUI te klikken (aangezien de titelbalk ontbreekt).

2. Audio Processing (DSP)
- Envelope Follower: Implementeer in processBlock een efficiënte algoritme voor RMS- en Peak-detectie.
- Smoothing: Gebruik een lineaire of exponentiële smoothing factor (τ) voor de release-fase van de meter, instelbaar via de editor.
- Data Transfer: Gebruik een juce::Atomic<float> om de huidige meterwaarde van de Processor naar de HUDWindow te sturen zonder audio-glitches.

3. Visuals & UI
- Clean Look: Het HUD-venster moet alleen een minimalistische VU-meter tekenen (bijv. een verticale balk of een glow-effect).
- Toggle Handles: Teken een kleine resize-handle in de rechterbenedenhoek die alleen zichtbaar wordt bij mouseMove.
- Customization: De hoofd-editor (VST window) moet parameters bevatten voor:
  - Opacity (0-100%).
  - Scale (Grootte van het HUD venster).
  - Color (Accentkleur van de meter).

4. Code Structuur Instructies
- Gebruik JUCE 8 standaarden.
- Scheid de Win32-specifieke code met #if JUCE_WINDOWS.
- Zorg dat het venster de laatst bekende positie (X, Y) opslaat in de AudioProcessorValueTreeState.

