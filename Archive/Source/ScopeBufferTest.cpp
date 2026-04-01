#include <JuceHeader.h>
#include "ScopeBuffer.h"

namespace CrokyScopy
{

class ScopeBufferTest : public juce::UnitTest
{
public:
    ScopeBufferTest() : juce::UnitTest("ScopeBuffer Tests", "DSP") {}

    void runTest() override
    {
        beginTest("Buffer initialization");
        ScopeBuffer buffer;
        buffer.prepare(44100.0);
        expectEquals(buffer.getWriteIndex(), 0);

        beginTest("Data pushing and binning");
        juce::AudioBuffer<float> testBuffer(1, 100);
        testBuffer.clear();
        testBuffer.setSample(0, 50, 1.0f); // Peak at 1.0

        buffer.pushBlock(testBuffer, 1, 0.0, 120.0, 4.0);

        bool foundPeak = false;
        for (int i = 0; i < ScopeBuffer::NumBins; ++i)
        {
            auto range = buffer.getBinRange(i);
            if (range.getEnd() >= 1.0f) { foundPeak = true; break; }
        }
        expect(foundPeak, "Peak of 1.0 should be found in the buffer");

        beginTest("Reset behavior");
        buffer.reset();
        expectEquals(buffer.getWriteIndex(), 0);
        // Range is [0, 0] or [min, max]
    }
};

static ScopeBufferTest scopeBufferTest;

} // namespace CrokyScopy

int main()
{
    juce::UnitTestRunner runner;
    runner.setPassesAreQuiet(false);
    runner.runAllTests();

    bool anyFailures = false;
    for (int i = 0; i < runner.getNumResults(); ++i)
    {
        if (auto* result = runner.getResult(i))
        {
            if (result->failures > 0) { anyFailures = true; break; }
        }
    }

    return anyFailures ? 1 : 0;
}
