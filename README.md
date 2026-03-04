# hint of blue

A small JUCE-based audio plugin (and standalone app) implementing a simple drive/distortion effect.

## Overview

`hint of blue` applies a soft-clipping/tanh distortion to incoming audio. It was created with the JUCE framework and includes both plugin and standalone targets.

## Features

- Per-channel processing using a tanh soft-clipper
- Three user-adjustable parameters:
  - `inputGainDb` — Input gain in decibels (range: -24.0 to 24.0, default: 0.0)
  - `drive` — Drive multiplier (range: 1.0 to 24.0, default: 0.0 in code)
  - `bias` — DC bias applied before the tanh (range: -2.5 to 2.5, default: 0.0)

Note: the `drive` parameter's default value in the code is `0.0`, which is outside the declared range of `1.0` to `24.0`. This should be reviewed in source if unexpected behavior occurs.

## Building

Requirements:
- Visual Studio 2019/2022 (Windows)
- JUCE modules are included in `JuceLibraryCode` (no separate JUCE download required for this workspace)

To build:
1. Open the provided Visual Studio project/solution in the repository (or open the project in Projucer if you have the `.jucer` file).
2. Select the desired configuration (`Debug` or `Release`) and target (Standalone or plugin format if multiple projects are present).
3. Build the solution.

If you want VST/AU formats, make sure the corresponding plugin formats are enabled in your Projucer project and that you have the appropriate SDKs installed (e.g., macOS SDK for AU, Steinberg VST3 SDK for VST3 builds).

## Running

- Standalone: run the built standalone executable.
- Plugin: load the built plugin (VST3/AU/etc.) inside a DAW or plugin host. Presets/parameter state are saved via the host or the plugin's state serialization.

## Project Structure

- `Source/` — main plugin/standalone source files (`PluginProcessor.*`, `PluginEditor.*`)
- `JuceLibraryCode/` — generated JUCE glue (modules and headers)
- Visual Studio project files for building on Windows

## Usage

- Adjust `Input` to set the input gain in dB.
- Increase `Drive` to add distortion (non-linear soft clipping).
- Tweak `Bias` to offset the waveform before clipping (useful for asymmetrical distortion).

