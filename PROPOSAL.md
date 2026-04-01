# Project CrokyScopy: Schone Herstart (Atomic Vision)

Een high-performance, JUCE 8 VST3 Oscilloscope met een 'Always-on-top' transparante HUD.

## Doel
Een visueel vloeibare oscilloscope bouwen die:
1. Geen allocations doet in de audio-thread (**Lock-free**).
2. Een vloeiend, bewegend spoor tekent onafhankelijk van de DAW-buffergrootte (**Binned Buffer**).
3. Een borderless Windows HUD heeft die volledig transparant en eventueel 'click-through' is (**Win32 Interop**).

## Voorgestelde Structuur (Skelet)
```text
/Source
    /Core
        ScopeBuffer.h      <- De 'Atomic' opslag (min/max bins)
    /DSP
        PluginProcessor.h  <- Audio afhandeling & Parameter management (APVTS)
        PluginProcessor.cpp
    /UI
        HUDWindow.h        <- De Win32 Layered Window wrapper
        HUDWindow.cpp
        ScopeComponent.h   <- De feitelijke rendering (Direct2D/juce::Graphics)
        ScopeComponent.cpp
    Common.h               <- Gedeelde types en defines
/CMakeLists.txt            <- JUCE 8 configuratie
```

## UI Strategie
We bouwen **geen custom VST editor**. In plaats daarvan gebruiken we de **Generic Editor** van de DAW (bitwig parameters). De enige custom UI is de **HUDWindow**, die los van de plugin-slot zweeft.

## Plan van Aanpak
1.  **Phase 1: Infrastructuur**
    - Opzetten van de nieuwe `CMakeLists.txt` voor JUCE 8.
    - Implementeren van de `ScopeBuffer` (thread-safe, binned opslag).
2.  **Phase 2: Audio/Data Link**
    - `PluginProcessor` opzetten met de juiste parameters voor Opacity, Hue, Sync.
    - Audio-blocks naar de `ScopeBuffer` pushen.
3.  **Phase 3: HUD UI Foundation**
    - `HUDWindow` bouwen met Win32 API calls (`WS_EX_LAYERED`, `SetLayeredWindowAttributes`).
    - Positie en grootte-opslag implementeren in APVTS.
4.  **Phase 4: Rendering & Sync**
    - `ScopeComponent` bouwen die de buffer op het scherm tekent.
    - Paging vs Scrolling mechanismen toevoegen.
5.  **Phase 5: Polish**
    - HSV-gekleurde lijnen en rendering optimalisaties.
