#pragma once

#include <JuceHeader.h>
#include <array>
#include <atomic>

namespace CrokyScopy
{

/**
 * @class ScopeBuffer
 * @brief Lock-free binnned buffer for the Oscillogram HUD.
 * 
 * Maps sample-accurate ppq positions into a fixed number of bins. 
 * This guarantees real-time safety (no allocations) and steady 
 * paint() performance (O(N) where N is NumBins), regardless of SampleRate or BPM.
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
            if (fractionalPosition < 0.0) fractionalPosition += 1.0; // Safety against negative PPQ

            // Calculate which bin this sample belongs to
            int binIdx = static_cast<int>(fractionalPosition * NumBins);
            binIdx = juce::jlimit(0, NumBins - 1, binIdx);

            // Mixdown channels (mono representation)
            float sampleVal = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
            {
                sampleVal += buffer.getReadPointer(ch)[i];
            }
            sampleVal /= static_cast<float>(numChannels); // Average

            // If we moved to a new bin, reset the NEW bin to the current sample
            int previousWriteIdx = writeIndex.load(std::memory_order_relaxed);
            if (binIdx != previousWriteIdx)
            {
                maxBins[binIdx].store(sampleVal, std::memory_order_relaxed);
                minBins[binIdx].store(sampleVal, std::memory_order_relaxed);
                writeIndex.store(binIdx, std::memory_order_release);
            }
            else // Append to current bin
            {
                float currentMax = maxBins[binIdx].load(std::memory_order_relaxed);
                float currentMin = minBins[binIdx].load(std::memory_order_relaxed);
                
                if (sampleVal > currentMax) maxBins[binIdx].store(sampleVal, std::memory_order_relaxed);
                if (sampleVal < currentMin) minBins[binIdx].store(sampleVal, std::memory_order_relaxed);
            }
        }
    }

    /**
     * @brief Read the current write index. Thread-safe.
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

    // Separate arrays for Min and Max values for memory contiguity / cache friendliness
    std::array<std::atomic<float>, NumBins> maxBins;
    std::array<std::atomic<float>, NumBins> minBins;
    
    std::atomic<int> writeIndex { 0 };
};

} // namespace CrokyScopy

