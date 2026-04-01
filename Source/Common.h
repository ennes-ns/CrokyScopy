#pragma once

#include <JuceHeader.h>

namespace CrokyScopy
{

/**
 * @struct HUDParameters
 * @brief Simple container for HUD state updates.
 */
struct HUDState
{
    float opacity { 1.0f };
    float hue { 0.5f };
    float lineWidth { 2.0f };
    bool isEditMode { true };
    int syncMode { 0 }; // 0 = Synced, 1 = Free
    int drawMode { 0 }; // 0 = Paging, 1 = Scrolling
};

} // namespace CrokyScopy
