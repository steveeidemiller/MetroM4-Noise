# Metro M4 Noise Machine
CD quality noise in full stereo

## Required hardware:
* Adafruit Metro M4: https://www.adafruit.com/product/3382
* Adafruit I2S Stereo Decoder UDA1334A: https://www.adafruit.com/product/3678
* Adafruit 10K potentiometer for volume control (or any similar pot should work just fine): https://www.adafruit.com/product/562

## Noise Algorithms
Algorithms used in this sketch have been adapted from the Audacity source code:
https://github.com/audacity/audacity/blob/master/src/effects/Noise.cpp  

## Description
This is a high-quality noise generation project featuring CD quality audio in full stereo. The decoder board has a built-in headphone jack and amplifier capable of driving things like the Bose QC35 noise-cancelling headphones (using their supplied audio cable). The noise algorithms are computed twice during each loop, once for the left channel, and again for the right channel, thus providing true stereo sound.

The volume pot is ONLY read during startup!! Changing the pot during noise playback will have no effect. This was necesary because the noise generation algorithms seem to use most of the CPU bandwidth, and the analogRead() function takes quite a few compute cycles as well. Also, any attempt to use analogRead() at various intervals causes noticeable audio artifacts. Therefore, the current design decision is to simply read volume one time during startup. If you need to change volume, simply touch the reset button. While less than ideal, this ensures continuous high-quality audio without any artifacts. The noise type is also hard-coded for similar reasons.

To use demo mode, simply uncomment the `#define DEMO;` line. This will continuously loop through all available noise types, printing the name of each one in the serial monitor. Demo mode is useful for selecting the type of noise you prefer as well as selecting an appropriate listening volume.