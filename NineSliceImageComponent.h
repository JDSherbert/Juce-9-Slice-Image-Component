// Copyright (c) 2025 JDSherbert. All rights reserved.

#pragma once

#include <JuceHeader.h>

namespace Sherbert
{
    // ======================================================================= //

    struct Margin
    {
        int left   = 0;
        int top    = 0;
        int right  = 0;
        int bottom = 0;
    };

    // ======================================================================= //

    struct Slice
    {
        juce::Rectangle<int> destination;
        juce::Rectangle<int> source;
    };

    // ======================================================================= //

    struct SliceLayout
    {
        std::array<Slice, 9> slices;

        SliceLayout
        (
            const juce::Image& image,
            juce::Rectangle<int> dstBounds,
            const Margin& margin
        );
    };

    // ======================================================================= //

    class NineSliceImageComponent : public juce::Component
    {

    public:

        NineSliceImageComponent() = default;

        void setImage(const juce::Image& newImage, const Margin& margins);

        [[nodiscard]] const Margin& getMargins() const noexcept { return margins; }

        void setShowDebugSlices(bool shouldShow)
        {
            showDebugSlices = shouldShow;
            repaint();
        }

        void setResamplingQuality(juce::Graphics::ResamplingQuality q) noexcept
        {
            resamplingQuality = q;
        }

    private:

        void paint(juce::Graphics& g) override;

        void resized() override {} // Layout rebuilt lazily in paint(); extend here if needed

        juce::Image image;
        Margin margins;
        juce::Graphics::ResamplingQuality resamplingQuality = juce::Graphics::highResamplingQuality;
        bool showDebugSlices = false;
    };

    // ======================================================================= //

}
