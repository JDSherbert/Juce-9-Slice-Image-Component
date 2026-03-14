<img width="975" height="865" alt="image" src="https://github.com/user-attachments/assets/77928d4d-99fc-403d-a71e-ba13eae0aed5" />

# Nine-Slice Image Component

<!-- Header Start -->
<img align="right" alt="Stars Badge" src="https://img.shields.io/github/stars/jdsherbert/Juce-9-Slice-Image-Component?label=%E2%AD%90"/>
<img align="right" alt="Forks Badge" src="https://img.shields.io/github/forks/jdsherbert/Juce-9-Slice-Image-Component?label=%F0%9F%8D%B4"/>
<img align="right" alt="Watchers Badge" src="https://img.shields.io/github/watchers/jdsherbert/Juce-9-Slice-Image-Component?label=%F0%9F%91%81%EF%B8%8F"/>
<img align="right" alt="Issues Badge" src="https://img.shields.io/github/issues/jdsherbert/Juce-9-Slice-Image-Component?label=%E2%9A%A0%EF%B8%8F"/>

<a href="https://juce.com/">
  <img height="40" width="40" src="https://cdn.simpleicons.org/juce">
</a>
<a href="https://learn.microsoft.com/en-us/cpp/cpp-language">
  <img height="40" width="40" src="https://cdn.simpleicons.org/cplusplus">
</a>
<a href="https://cmake.org/">
  <img height="40" width="40" src="https://cdn.simpleicons.org/cmake">
</a>
<!-- Header End -->

-----------------------------------------------------------------------

<a href="https://juce.com/">
  <img align="left" alt="JUCE Component" src="https://img.shields.io/badge/JUCE%20Component-black?style=for-the-badge&logo=juce&logoColor=white&color=black&labelColor=black">
</a>

<a href="https://choosealicense.com/licenses/mit/">
  <img align="right" alt="License" src="https://img.shields.io/badge/License%20:%20MIT-black?style=for-the-badge&logo=mit&logoColor=white&color=black&labelColor=black">
</a>

<br></br>

-----------------------------------------------------------------------

## Overview

A JUCE `Component` that renders an image using **nine-slice scaling** (also known as scale-9 or border-image slicing). Nine-slice scaling divides an image into a 3×3 grid of regions (four corners, four edges, and a center) and scales each region independently. Corners are drawn at their original size, edges are stretched along one axis only, and the center fills the remaining space. This preserves crisp, undistorted corners regardless of how the component is resized, making it ideal for UI chrome such as panels, buttons, tooltips, and windows.

-----------------------------------------------------------------------

## Features

- Pixel-accurate nine-slice rendering via JUCE's `Graphics::drawImage`
- Configurable per-side margins (`left`, `top`, `right`, `bottom`) via the `Margin` struct
- Zero-size slice guard - empty source or destination rects are safely skipped
- Configurable resampling quality (defaults to `highResamplingQuality` for modern UI assets). 
Worth noting you can change this to "low" for pixel or low res assets to sharpen them up.
- Optional debug overlay that draws slice boundary lines and a component outline at runtime

-----------------------------------------------------------------------

## Files

| File | Description |
|---|---|
| `NineSliceImageComponent.h` | Component declaration, `Margin`, `Slice`, and `SliceLayout` structs |
| `NineSliceImageComponent.cpp` | Layout construction and paint implementation |

-----------------------------------------------------------------------

## Integration
### CMake (Recommended)
Add the following to your `CMakeLists.txt`. 
CMake will fetch the component directly from GitHub at configure time; no manual copying required!

```cmake
cmakeinclude(FetchContent)

FetchContent_Declare(
    NineSliceImageComponent
    GIT_REPOSITORY https://github.com/JDSherbert/Juce-9-Slice-Image-Component.git
    GIT_TAG        main
    GIT_SHALLOW    TRUE
)

FetchContent_MakeAvailable(NineSliceImageComponent)

target_link_libraries(YourTarget
    PRIVATE
        NineSliceImageComponent
)
```

Then include the header in your source:

```cpp
#include <NineSliceImageComponent.h>
```

Tip: Pin to a specific commit hash rather than main for reproducible builds:
```cmake
GIT_TAG <commit-hash>
```

### Manual
If you're not using CMake, copy `NineSliceImageComponent.h` and `NineSliceImageComponent.cpp` directly into your project and add them to your build system. No dependencies beyond `JuceHeader.h` are required.

-----------------------------------------------------------------------

## Usage

### Basic

```cpp
Sherbert::NineSliceImageComponent nineSlice;

// Load your image however you like — BinaryData, File, ImageCache, etc.
juce::Image panelImage = juce::ImageCache::getFromMemory(BinaryData::panel_png, BinaryData::panel_pngSize);

// Define margins: how many pixels on each side are treated as fixed corners/edges.
// Here: 12px fixed regions on all sides.
Sherbert::Margin margin { 12, 12, 12, 12 };

nineSlice.setImage(panelImage, margin);
addAndMakeVisible(nineSlice);
```

### Resizing

`SliceLayout` is rebuilt automatically on each paint call, so simply call `setBounds()` as normal; no manual layout invalidation required:

```cpp
nineSlice.setBounds(getLocalBounds().reduced(20));
```

### Resampling Quality

The default is `highResamplingQuality`, which produces smooth results for photographic source images. For pixel/low resolution art, you may prefer a lower setting to combat the anti-aliasing and keep it looking sharp:

```cpp
nineSlice.setResamplingQuality(juce::Graphics::lowResamplingQuality);
```

### Debug Overlay

Enable the debug overlay to visualise slice boundaries at runtime:

```cpp
nineSlice.setShowDebugSlices(true);
```

This draws magenta lines at each slice boundary and a cyan outline around the component bounds.

-----------------------------------------------------------------------

## API Reference

### `Margin`

```cpp
struct Margin
{
    int left   = 0;
    int top    = 0;
    int right  = 0;
    int bottom = 0;
};
```

Defines the fixed pixel thickness of each edge region. The corner size is the intersection of the adjacent edge thicknesses. For example, the top-left corner is `left` × `top` pixels.

---

### `NineSliceImageComponent`

| Method | Description |
|---|---|
| `setImage(image, margins)` | Sets the source image and slice margins. Triggers a repaint. |
| `getMargins()` | Returns the current `Margin` by const reference. |
| `setResamplingQuality(quality)` | Sets the `juce::Graphics::ResamplingQuality` used during paint. |
| `setShowDebugSlices(bool)` | Toggles the debug slice boundary overlay. |

-----------------------------------------------------------------------

## How It Works

The `SliceLayout` constructor divides the source image and destination bounds into a 3×3 grid and pre-computes all nine `{ dst, src }` rect pairs upfront. During `paint()`, each pair is passed directly to `Graphics::drawImage`, which handles the stretch. Because `SliceLayout` is rebuilt fresh on each paint call, it always reflects the current component bounds with no stale state.

```
Source image          Destination component
┌───┬───────┬───┐     ┌───┬─────────────┬───┐
│ TL│  Top  │TR │     │ TL│    Top      │TR │  ← corners: fixed size
├───┼───────┼───┤     ├───┼─────────────┼───┤
│   │       │   │     │   │             │   │
│ L │ Center│ R │     │ L │   Center    │ R │  ← edges: stretch on one axis
│   │       │   │     │   │             │   │  ← center: stretches on both axes
├───┼───────┼───┤     ├───┼─────────────┼───┤
│ BL│ Bottom│BR │     │ BL│   Bottom    │BR │  ← corners: fixed size
└───┴───────┴───┘     └───┴─────────────┴───┘
```

-----------------------------------------------------------------------
