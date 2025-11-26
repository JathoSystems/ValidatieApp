```shell
cd ../../
git clone --depth 1 https://github.com/libsdl-org/SDL.git external/SDL3
git clone --depth 1 https://github.com/libsdl-org/SDL_image.git external/SDL3_image
git clone --depth 1 https://github.com/libsdl-org/SDL_ttf external/SDL3_TTF
cd external/GameEngine
git clone https://github.com/JathoSystems/GameEngine.git external/GameEngine
```
Ja gek andersom clion doet gek idk why

windows:
```shell
.\external\GameEngine\external\SDL3_TTF\external\Get-GitModules.ps1
```
unix:
```shell
./external/GameEngine/external/SDL3_TTF/download.sh
```