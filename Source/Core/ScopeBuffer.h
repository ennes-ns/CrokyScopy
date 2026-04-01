#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>

namespace CrokyScopy
{

/**
 * @class ScopeBuffer
 * @brief Lock-free binnned buffer for the Oscilloscope HUD.
 * 
 * Maps samples into a fixed number of bins (e.g. 4096). 
 * Captures min/max peaks per bin for pixel-perfect visualization 
 * regardless of zoom or sample rate.
 */
class ScopeBuffer
{
public:
    static constexpr int NumBins = 4096;

    ScopeBuffer() = default;

    /**
     * @brief Prepares the buffer with the current sample rate.
     */
    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate;
        reset();
    }

    /**
     * @brief Clears the buffer data.
     */
    void reset()
    {
        for (auto& bin : maxBins) bin.store(0.0f, std::memory_order_relaxed);
        for (auto& bin : minBins) bin.store(0.0f, std::memory_order_relaxed);
        writeIndex.store(0, std::memory_order_release);
    }

    /**
     * @brief Processes an incoming audio block. O(Samples).
     * @param buffer The input audio buffer.
     * @param numChannels Number of channels to mix down (usually 1 or 2).
     * @param ppqStart The PPQ (Pulse Position Quarter-note) at the start of the block.
     * @param bpm Current tempo in Beats Per Minute.
     * @param beatsPerPass How many beats represent a full cycle (e.g., 4.0).
     */
    void pushBlock(const juce::AudioBuffer<float>& buffer, 
                   int numChannels, 
                   double ppqStart, 
                   double bpm, 
                   double beatsPerPass)
    {
        if (sampleRate <= 0.0 || beatsPerPass <= 0.0) return;

        int numSamples = buffer.getNumSamples();
        double ppqPerSample = (bpm / 60.0) / sampleRate;

        for (int i = 0; i < numSamples; ++i)
        {
            double currentPpq = ppqStart + (i * ppqPerSample);
            
            // Map the continuous PPQ into a repeating 0-1 range based on beatsPerPass
            double fractionalPosition = std::fmod(currentPpq, beatsPerPass) / beatsPerPass;
            if (fractionalPosition < 0.0) fractionalPosition += 1.0; 

            int binIdx = static_cast<int>(fractionalPosition * NumBins);
            binIdx = juce::jlimit(0, NumBins - 1, binIdx);

            // Mixdown channels 
            float sampleVal = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
            {
                sampleVal += buffer.getReadPointer(ch)[i];
            }
            sampleVal /= static_cast<float>(juce::jmax(1, numChannels));

            // Logic: If we moved to a new bin, reset it. Otherwise combine (capture peak).
            int previousWriteIdx = writeIndex.load(std::memory_order_relaxed);
            if (binIdx != previousWriteIdx)
            {
                maxBins[binIdx].store(sampleVal, std::memory_order_relaxed);
                minBins[binIdx].store(sampleVal, std::memory_order_relaxed);
                writeIndex.store(binIdx, std::memory_order_release);
            }
            else 
            {
                float currentMax = maxBins[binIdx].load(std::memory_order_relaxed);
                float currentMin = minBins[binIdx].load(std::memory_order_relaxed);
                
                if (sampleVal > currentMax) maxBins[binIdx].store(sampleVal, std::memory_order_relaxed);
                if (sampleVal < currentMin) minBins[binIdx].store(sampleVal, std::memory_order_relaxed);
            }
        }
    }

    /**
     * @brief Gets the current write index.
     */
    int getWriteIndex() const
    {
        return writeIndex.load(std::memory_order_acquire);
    }

    /**
     * @brief Gets a snapshot of the current min/max bounds for a given bin.
     */
    juce::Range<float> getBinRange(int index) const
    {
        if (index < 0 || index >= NumBins) return {0.0f, 0.0f};
        return juce::Range<float>(
            minBins[index].load(std::memory_order_relaxed),
            maxBins[index].load(std::memory_order_relaxed)
        );
    }

private:
    double sampleRate { 0.0 };

    std::array<std::atomic<float>, NumBins> maxBins;
    std::array<std::atomic<float>, NumBins> minBins;
    
    std::atomic<int> writeIndex { 0 };
};

} // namespace CrokyScopy
