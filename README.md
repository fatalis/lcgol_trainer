Arch Linux compilation instructions:

```
sudo pacman -Sy mingw-w64 mingw-w64-cmake git yasm
sudo ln -s /usr/bin/yasm /usr/i686-w64-mingw32/bin # fix this hack later
git clone --recursive https://github.com/fatalis/lcgol_trainer.git
cd lcgol_trainer
mkdir build && cd build
i686-w64-mingw32-cmake -DCMAKE_BUILD_TYPE=Release ..
make
```
