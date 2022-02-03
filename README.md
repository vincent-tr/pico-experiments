# pico-experiments

## Build deps
- build ~/workspace/pico_i2c_slave

## Build

- cd <dir>
- mkdir build
- cd build
- PICO_SDK_PATH=~/workspace/pico-sdk cmake ..
- make -j4
- pick .uf2 file and move it to rpi dir