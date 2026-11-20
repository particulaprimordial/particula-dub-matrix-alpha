# PARTÍCULA DUB MATRIX

**Dub Techno Computational Audio Laboratory — Product 01**

An independent stereo delay and filtered-feedback VST3 processor built with C++ and JUCE.

## Alpha 0.1 scope

- Stereo delay line with interpolated reads
- Feedback path with one-pole low-pass and high-pass filtering
- Soft saturation in the feedback path
- Delay time, feedback, wet, LPF, HPF and drive parameters
- Host automation and parameter state serialization
- Initial JUCE VST3 interface

This is an engineering alpha, not a finished commercial release. It does not yet include tempo sync, multi-tap routing, tape wow/flutter, freeze, diffusion or a reverb network.

## Requirements

- Windows 10/11, 64-bit
- Visual Studio 2022 with Desktop development with C++
- CMake 3.22+
- Git
- Internet connection on first configure (JUCE is fetched by CMake)

## Build

Open a Developer PowerShell for VS 2022 in this folder:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release
```

The VST3 bundle will be generated under:

```text
build/ParticulaDubMatrix_artefacts/Release/VST3/
```

Copy the resulting `Particula Dub Matrix.vst3` bundle to:

```text
C:\Program Files\Common Files\VST3\
```

Then rescan plugins in Ableton Live.

## Important audio-engine note

The current alpha uses a bounded feedback coefficient (0.0–0.92), but this is not a proof of unconditional stability for every future filter, modulation or routing configuration. Future versions require automated stress tests, denormal tests, parameter smoothing, channel-layout tests and host validation.

## Architecture

- `Source/PluginProcessor.*`: audio engine and parameter/state model
- `Source/PluginEditor.*`: initial control interface
- `CMakeLists.txt`: JUCE VST3 build configuration

## Roadmap

1. Validate this baseline build and audio path.
2. Add smoothed parameters and host tempo synchronization.
3. Implement dual feedback filters and stereo routing modes.
4. Add tape modulation, diffusion and reverb.
5. Add Python reference models and Streamlit analysis.
6. Publish validated Windows VST3 releases.

## License

No license has been selected yet. Do not redistribute this source or binary as an open-source project until a license is chosen.


## Python research laboratory

Install and run the offline Streamlit model:

```powershell
cd laboratory
py -m pip install -r requirements.txt
py -m streamlit run app.py
```

Run the DSP reference tests from the project root:

```powershell
py -m pip install pytest numpy
py -m pytest tests
```
