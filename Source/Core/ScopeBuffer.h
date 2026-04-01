#pragma once

#include <JuceHeader.h>
#include <atomic>
#include <array>

namespace CrokyScopy
{

class ScopeBuffer
{
public:
    static constexpr int NumBins = 1000;

    ScopeBuffer()
    {
        reset();
    }

    void prepare(double newSampleRate)
    {
        sampleRate = newSampleRate > 0.0 ? newSampleRate : 44100.0;
        reset();
    }

    void reset()
    {
        for (auto& b : bins)
            b.store(0.0f, std::memory_order_relaxed);
        
        writeIndex.store(0, std::memory_order_relaxed);
    }

    // Called from the AUDIO THREAD
    void pushBlock(const juce::AudioBuffer<float>& buffer, int numChannels, 
                   double ppq, double bpm, double beatsPerPass)
    {
        if (numChannels == 0 || buffer.getNumSamples() == 0) return;

        double samplesPerPass = (beatsPerPass / bpm) * 60.0 * sampleRate;
        if (samplesPerPass <= 0.0) return;

        // Simple rolling max implementation for absolute zero-lag visual feedback
        const float* channelData = buffer.getReadPointer(0);
        int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i)
        {
            float env = std::abs(channelData[i]);
            
            // Advance writing bin conceptually
            int currentBin = writeIndex.load(std::memory_order_relaxed);
            
            float currentVal = bins[currentBin].load(std::memory_order_relaxed);
            if (env > currentVal)
            {
                bins[currentBin].store(env, std::memory_order_relaxed);
            }

            samplesInCurrentBin++;
            if (samplesInCurrentBin >= (int)(samplesPerPass / NumBins))
            {
                samplesInCurrentBin = 0;
                int nextBin = (currentBin + 1) % NumBins;
                bins[nextBin].store(0.0f, std::memory_order_relaxed); // Clear ahead
                writeIndex.store(nextBin, std::memory_order_release);
            }
        }
    }

    // Called from the GUI THREAD
    float getBinValue(int index) const
    {
        if (index < 0 || index >= NumBins) return 0.0f;
        return bins[index].load(std::memory_order_acquire);
    }

    int getWriteIndex() const
    {
        return writeIndex.load(std::memory_order_acquire);
    }

private:
    std::array<std::atomic<float>, NumBins> bins;
    std::atomic<int> writeIndex { 0 };
    int samplesInCurrentBin { 0 };
    double sampleRate { 44100.0 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ScopeBuffer)
};

} // namespace CrokyScopy
