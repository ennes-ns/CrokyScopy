#pragma once

#include <JuceHeader.h>
#include "ScopeBuffer.h"

namespace CrokyScopy
{

class ScopeBufferTest : public juce::UnitTest
{
public:
    ScopeBufferTest() : juce::UnitTest("ScopeBuffer Logic Test") {}

    void runTest() override
    {
        beginTest("Basic Buffer Bin Assignment");
        
        ScopeBuffer buffer;
        buffer.prepare(44100.0);
        
        // Let's simulate 1 sample at exactly the start (0.0 PPQ)
        // Beats per pass = 4.0
        juce::AudioBuffer<float> dummy(1, 1);
        dummy.setSample(0, 0, 1.0f); // Max Amplitude out of 1.0
        
        buffer.pushBlock(dummy, 1, 0.0, 120.0, 4.0);
        
        expectEquals(buffer.getWriteIndex(), 0);
        
        // Bin 0 should capture our peak of 1.0
        juce::Range<float> r = buffer.getBinRange(0);
        expect(r.getEnd() == 1.0f);


        beginTest("Cycling Behavior (Modulo Wrap)");

        buffer.reset();
        
        // Simulating exactly 2 beats in (Halfway through a 4 beat pass)
        // With 4096 bins, halfway is bin 2048.
        buffer.pushBlock(dummy, 1, 2.0, 120.0, 4.0);
        expectEquals(buffer.getWriteIndex(), 2048);

        // Next, simulating exactly 4 beats in (Which should wrap back to bin 0)
        buffer.pushBlock(dummy, 1, 4.0, 120.0, 4.0);
        expectEquals(buffer.getWriteIndex(), 0);
    }
};

// Create a static instance so the JUCE UnitTestRunner can find it.
static ScopeBufferTest scopeBufferTestInstance;

} // namespace CrokyScopy
