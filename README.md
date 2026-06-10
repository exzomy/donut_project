# donut-console-win

3D ASCII donut animation rendered directly in the Windows console.

Real-time rotation, depth buffering and console-based lighting with no external dependencies.

<br>

> **Features**:
> - Rotating torus with dynamic lighting
> - ASCII shading from dark to bright characters
> - Frame-rate independent animation using delta time
> - Depth buffer (Z-buffer) for correct surface visibility
> - Smooth rendering with console double buffering
> - Standalone executable — download and run

<br>

> **Technical Details**:
> - Pure C++ / Win32 API
> - No .NET, SDL, OpenGL or DirectX
> - CreateConsoleScreenBuffer-based rendering
> - High-precision QueryPerformanceCounter timing
> - Consolas 8×16 font configuration
> - Optimized memory usage and minimal allocations

<br>

**Target Platform**:
- Windows x64
- Visual Studio 2022
- Console Application

<br>

**Executable:** `proj.exe`
