Common application tools, a reasonably slim set of genuinely useful stuff:

- Platform/compiler/architecture defines + related macros.
- Assert/verify macros (with a callback for app-specific behavior).
- Time functions.
- Hash functions.
- Containers.
- File + file system tools.
- Common file format load/parse (image files, JSON, INI).
- Misc useful base classes for common idioms (factory, static initializer, etc.).

Currently only supporting Windows, but the project is designed around being cross-platform.

## Usage ##
The project is intended to be compiled as a static lib. A (premake)[https://premake.github.io/] script is provided in `build/` (requires premake5).

## Dependencies ##
Committed dependencies:

- (GLM)[https://github.com/g-truc/glm]
- (stb)[https://github.com/nothings/stb]
- (RapidJSON)[http://rapidjson.org/]
- (LodePNG)[http://lodev.org/lodepng/]