# ShaderCourseStarter

An OpenGL/GLSL project used for 3D and shader programming, focusing on an instanced leaf model with a hover/spin/vibration animations.

## Requirements

- **CMake 3.24+**
- A **C++20** compiler:
  - Windows: MSVC (Visual Studio 2022) or MinGW-w64
  - Linux: GCC or Clang
  - macOS: Apple Clang (Xcode command line tools)
- Git (needed by CMake's `FetchContent` to pull GLFW and Assimp)
- An internet connection for the first configure (GLFW and Assimp are downloaded automatically)

### Third-party dependencies

Most dependencies are fetched or built automatically. No manual installation is required:

| Library | Source |
|---|---|
| GLFW 3.4 | Fetched via `FetchContent` |
| Assimp v5.4.3 | Fetched via `FetchContent` (static build) |
| GLM | Included in `vendors/glm`, added via `add_subdirectory` |
| stb_image | Included in `vendors/stb` |
| GLAD | Included in `external/glad` |

Make sure the project folder contains `external/glad`, `vendors/glm`, `vendors/stb`, `shaders/`, and `res/` — these are referenced directly by the CMake script and are required for the build to succeed and for the app to find its assets at runtime.

## Building

### 1. Configure

From the project root:

```bash
cmake -S . -B build
```

On Windows with Visual Studio, this generates a `.sln` you can open, or you can add a generator explicitly, e.g.:

```bash
cmake -S . -B build -G "Visual Studio 17 2022"
```

The first configure step will download and build GLFW and Assimp, so it can take a few minutes.

### 2. Build

```bash
cmake --build build --config Release
```

(Use `Debug` instead of `Release` for a debug build; the config flag only matters for multi-config generators like Visual Studio.) Due to dynamic model loading via Assimp, the `Release` config is recommended if the only purpose is to view the shader demo, in order to reduce build times.

### 3. Run

The executable, along with the `shaders/` and `res/` directories it needs at runtime, is copied into the binary location:

```
build/bin/shader_course        # Linux/macOS
build/bin/shader_course.exe    # Windows
```

Run it directly from the `bin` folder or via your IDE.

## Controls

| Input | Action |
|---|---|
| `W` / `A` / `S` / `D` | Move camera forward / left / backward / right |
| `Q` / `E` | Move camera down / up |
| Mouse movement | Look around (first-person free-fly camera) |
| Mouse scroll | Zoom in/out (adjusts camera FOV) |
| `Left Shift` (hold) | Speed boost — accelerates camera movement |
| `F` (hold)| Toggle the leaf cluster's hover animation until the `F` key is released|
| `F` (release)| Causes the leaf cluster to return to it's "resting" state|
| `T` (press)| Toggles either draw calls using either instanced or non-instanced element arrays. Relevant debug log is printed to display the time it takes for each render process to finish.|
| `Esc` | Close the application |
