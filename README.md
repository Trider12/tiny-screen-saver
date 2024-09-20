# Tiny screen saver
https://github.com/user-attachments/assets/23629fee-a9f5-44fa-85e0-4246ba02ef9e
### About
The goal of this project was to write a fully functional Windows screen saver (including preview and settings) in around 1024 bytes. The current size is **1293** bytes.\
The screen saver implements a 2D [Cyclic cellular automaton](https://en.wikipedia.org/wiki/Cyclic_cellular_automaton) using the first 16 colors of the default Windows color palette.\
\
**WARNING:** the screen saver executable can trigger antiviruses due to compression.
### Building
Open the project in Visual Studio.\
\
Alternatively:
```cmd
cd tiny-screen-saver
cmake -Bbuild -DCMAKE_BUILD_TYPE=Release -GNinja
cmake --build build
```
The `USE_CRINKLER` CMake option allows to set [crinkler](https://github.com/runestubbe/Crinkler) as the linker. The default is `ON`.
### Installing
To install the screen saver the produced `tss.scr` needs to be copied to the Windows `System32` folder.\
The build script will do this automatically if it has admin rights.\
After that select the `Change screen saver` option in the Windows `Control Panel` and select `tss` from the list.
