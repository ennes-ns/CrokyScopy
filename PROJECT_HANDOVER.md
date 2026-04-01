# CrokyScopy: Project Handover Document

Dit document beschrijft de exacte, oorspronkelijke bedoeling van het **CrokyScopy** project. Alle eerdere (falende) broncode is verwijderd, zodat een volgende agent of programmeur met een 100% schone lei aan de architectuur kan bouwen zonder beïnvloed te worden door oude bugs.

---

## 1. Doel van de Plugin
**CrokyScopy** is een professionele VST3 oscilloscope (audio effect) gebouwd met JUCE 8 voor **Windows**.  
Het unieke aan deze plugin is dat de visualisatie **niet** in het standaard DAW-venster (de plugin-editor) gebeurt. In plaats daarvan spawnt er een **losstaand, transparant, always-on-top HUD-venster** rechtstreeks op de Windows desktop.

*Ideaal gebruikscenario:* De gebruiker zet de HUD ergens op het scherm, schakelt 'Click-through' in, en heeft tijdens het mixen in Bitwig of FL Studio altijd visuele feedback van de pieken zonder dat er een lomp plugin-venster in de weg staat.

---

## 2. Architectuur & Componenten

Er zijn drie hoofdonderdelen nodig. De volgende agent moet deze **exact** implementeren, maar wél met de juiste thread-safety en Window-lifecycle logica om freezes te voorkomen.

### A. De Audio Processor (`processBlock`)
- Leest inkomende audio.
- Berekent de absolute piek per frame/bin op basis van DAW Transport (BPM / PPQ).
- Verzendt deze data **100% lock-free** (bijv. via een `std::atomic<float>` array) naar de HUD. De audio-thread mag **nooit** wachten op de GUI of UI commando's afvuren (`callAsync` is verboden).

### B. De VST3 Editor (DAW Window)
- Heeft zelf geen grafische scope nodig. 
- Mag simpelweg JUCE's `GenericAudioProcessorEditor` of een kale parameterlijst zijn.
- Stelt de gebruiker in staat om de onderstaande APVTS parameters aan te passen.

### C. De Zwevende HUD (Het Knolpunt)
- Een `juce::TopLevelWindow`, `DocumentWindow` of naakte `Component` die een eigen OS-venster krijgt (meestal via `addToDesktop()`).
- **Must-haves:**
  - `AlwaysOnTop = true`
  - Geen titelbalk (borderless).
  - Volledig transparante achtergrond (`juce::Colours::transparentBlack`).
- Opgeslagen positie: De X, Y, Breedte en Hoogte moeten opgeslagen worden in de plugin-state zodat de HUD op dezelfde plek terugkomt bij het inladen van het project.

---

## 3. APVTS Parameters (Features)
De plugin moet de volgende parameters bevatten (in een `AudioProcessorValueTreeState`):

| Parameter | Type | Beschrijving |
| :--- | :--- | :--- |
| `show_hud` | Bool | Toggelt de zichtbaarheid (en levenscyclus) van het zwevende venster. |
| `opacity` | Float | De doorschijnendheid van de HUD (0.0 tot 1.0). |
| `line_width`| Float | Dikte van de oscilloscope lijn. |
| `hue` | Float | Kleur van de lijn (HSV hue: 0.0 - 1.0). |
| `beats` | Choice | Aantal inkomende beats (2, 4, 8, 16) voordat het scherm vol is. |
| `paused` | Bool | Bevriest de huidige oscilloscope weergave. |
| `transport_sync`| Bool | Pauzeert het tekenen automatisch als de DAW op stop/pauze staat. |
| *(Hidden)* | Float | `hud_x`, `hud_y`, `hud_width`, `hud_height` voor het onthouden van de venstergrootte en locatie. |

---

## 4. De Fatale Bug (Voor de nieuwe Agent)

De vorige iteraties van dit project faalden herhaaldelijk op **één specifiek pijnpunt** in JUCE 8 op Windows 10/11:

> 🚨 **SYSTEM FREEZE & GHOST WINDOWS** 🚨
> 
> Zodra het secundaire HUD-venster (`DocumentWindow` of `Component`) werd aangemaakt en via `addToDesktop()` of de parameter-timer zichtbaar werd gemaakt, liep de *gehele PC* (en de DAW) totaal vast in een extreme lag-/freeze-lus. Als het al opende, werd het vaak geregistreerd als een "onzichtbaar spook-venster" zonder grafische inhoud, terwijl de background CPU naar 100% schoot.
>
> **Specifieke oorzaken in het verleden bleken:**
> - Een timer op 60Hz met `setBufferedToImage(true)` op een transparant Component.
> - Het aanroepen van Win32 commando's (`SetWindowPos`, `SetWindowLong`) met 30 frames per seconde vanuit een timer.
> - De levenscyclus (`new Window / delete Window`) verkeerd koppelen aan APVTS timers, wat leidde tot memory leaks, message-loop deadlocks of JUCE internal assertion crashes.

### Instructie voor de nieuwe agent:
Jouw (nieuwe) taak is om deze multi-window VST3 voor Windows te bouwen zónder dat het OS vastloopt. 
**Prioriteit 1:** Zoek in de JUCE fora naar de exacte, 100% stabiele "best practice" om een tweede, onafhankelijk, always-on-top venster te koppelen aan een VST3 AudioProcessor *zonder* Win32-spikes of OpenGL/Direct2D deadlocks te veroorzaken. Zodra dat venster stabiel op- en wegpopt in Bitwig, dán pas begin je met het tekenen van de geluidsgolf. 
