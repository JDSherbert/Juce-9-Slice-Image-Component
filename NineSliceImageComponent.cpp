// Copyright (c) 2025 JDSherbert. All rights reserved.

#include "NineSliceImageComponent.h"

// ======================================================================= //

Sherbert::SliceLayout::SliceLayout
(
    const juce::Image& image,
    juce::Rectangle<int> destinationBounds,
    const Margin& margin
)
{
    const int imageWidth  = image.getWidth();
    const int imageHeight = image.getHeight();

    const int destinationWidth  = destinationBounds.getWidth();
    const int destinationHeight = destinationBounds.getHeight();

    const int rightEdgeX  = destinationWidth  - margin.right;
    const int bottomEdgeY = destinationHeight - margin.bottom;

    const int sourceCenterWidth  = imageWidth        - margin.left - margin.right;
    const int sourceCenterHeight = imageHeight       - margin.top  - margin.bottom;
    const int destCenterWidth    = destinationWidth  - margin.left - margin.right;
    const int destCenterHeight   = destinationHeight - margin.top  - margin.bottom;

    // Source rects
    const juce::Rectangle<int> sourceRects[3][3] =
    {
        {
            { 0,                               0,  margin.left, margin.top },
            { margin.left,                     0,  sourceCenterWidth, margin.top },
            { margin.left + sourceCenterWidth, 0,  margin.right,margin.top }
        },
        {
            { 0,                               margin.top, margin.left, sourceCenterHeight },
            { margin.left,                     margin.top, sourceCenterWidth, sourceCenterHeight },
            { margin.left + sourceCenterWidth, margin.top, margin.right,sourceCenterHeight }
        },
        {
            { 0,                               margin.top + sourceCenterHeight, margin.left, margin.bottom },
            { margin.left,                     margin.top + sourceCenterHeight, sourceCenterWidth, margin.bottom },
            { margin.left + sourceCenterWidth, margin.top + sourceCenterHeight, margin.right,margin.bottom }
        }
    };

    // Destination rects
    const juce::Rectangle<int> destinationRects[3][3] =
    {
        {
            { 0,           0, margin.left,  margin.top },
            { margin.left, 0, destCenterWidth,    margin.top },
            { rightEdgeX,         0, margin.right, margin.top }
        },
        {
            { 0,           margin.top, margin.left, destCenterHeight },
            { margin.left, margin.top, destCenterWidth,   destCenterHeight },
            { rightEdgeX,         margin.top, margin.right,destCenterHeight }
        },
        {
            { 0,           bottomEdgeY,  margin.left,    margin.bottom },
            { margin.left, bottomEdgeY,  destCenterWidth,      margin.bottom },
            { rightEdgeX,         bottomEdgeY,  margin.right,   margin.bottom }
        }
    };

    // Flatten into array
    int index = 0;
    for (int row = 0; row < 3; ++row)
    {
        for (int col = 0; col < 3; ++col)
        {
            slices[index++] = { destinationRects[row][col], sourceRects[row][col] };
        }
    }

}

// ======================================================================= //

void Sherbert::NineSliceImageComponent::setImage(const juce::Image& newImage, const Margin& newMargins)
{
    jassert(newMargins.left   >= 0);
    jassert(newMargins.top    >= 0);
    jassert(newMargins.right  >= 0);
    jassert(newMargins.bottom >= 0);

    jassert(newMargins.left + newMargins.right  <= newImage.getWidth());
    jassert(newMargins.top  + newMargins.bottom <= newImage.getHeight());

    image   = newImage;
    margins = newMargins;
    repaint();
}

// ======================================================================= //

void Sherbert::NineSliceImageComponent::paint(juce::Graphics& g)
{
    if (!image.isValid())
        return;

    g.setImageResamplingQuality(resamplingQuality);

    const SliceLayout layout(image, getLocalBounds(), margins);

    for (const auto& slice : layout.slices)
    {
        if (slice.source.isEmpty() || slice.destination.isEmpty()) continue;

        g.drawImage
        (
            image,
            slice.destination.getX(),         slice.destination.getY(),
            slice.destination.getWidth(),  slice.destination.getHeight(),
            slice.source.getX(),               slice.source.getY(),
            slice.source.getWidth(),        slice.source.getHeight()
        );
    }

    // DEBUG OVERLAY
    if (showDebugSlices)
    {
        const auto bounds  = getLocalBounds();
        const int  boundsWidth          = bounds.getWidth();
        const int  boundsHeight         = bounds.getHeight();

        g.setColour(juce::Colours::magenta.withAlpha(0.6f));

        // Vertical slice boundaries
        g.drawVerticalLine(margins.left,                     0.0f, (float)boundsHeight);
        g.drawVerticalLine(boundsWidth - margins.right,   0.0f, (float)boundsHeight);

        // Horizontal slice boundaries
        g.drawHorizontalLine(margins.top,                      0.0f, (float)boundsWidth);
        g.drawHorizontalLine(boundsHeight - margins.bottom, 0.0f, (float)boundsWidth);

        // Outline component
        g.setColour(juce::Colours::cyan.withAlpha(0.6f));
        g.drawRect(bounds, 1);
    }
}

// ======================================================================= //
