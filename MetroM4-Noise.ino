/*
  Metro M4 Noise Machine - CD quality noise in full stereo

  Required hardware:
  * Adafruit Metro M4: https://www.adafruit.com/product/3382
  * Adafruit I2S Stereo Decoder UDA1334A: https://www.adafruit.com/product/3678
  * Adafruit 10K potentiometer for volume control (or any similar pot should work just fine): https://www.adafruit.com/product/562

  Noise algorithms used in this sketch have been adapted from the Audacity source code:
  https://github.com/audacity/audacity/blob/master/src/effects/Noise.cpp  

  This is a high-quality noise generation project featuring CD quality audio in full stereo. The decoder board has
  a built-in headphone jack and amplifier capable of driving things like the Bose QC35 noise-cancelling headphones
  (using their supplied audio cable). The noise algorithms are computed twice during each loop, once for the left
  channel, and again for the right channel, thus providing true stereo sound.

  The volume pot is ONLY read during startup!! Changing the pot during noise playback will have no effect. This was
  necesary because the noise generation algorithms seem to use most of the CPU bandwidth, and the analogRead()
  function takes quite a few compute cycles as well. Also, any attempt to use analogRead() at various intervals
  causes noticeable audio artifacts. Therefore, the current design decision is to simply read volume one time
  during startup. If you need to change volume, simply touch the reset button. While less than ideal, this ensures
  continuous high-quality audio without any artifacts. The noise type is also hard-coded for similar reasons.

  To use demo mode, simply uncomment the "#define DEMO;" line. This will continuously loop through all available
  noise types, printing the name of each one in the serial monitor. Demo mode is useful for selecting the type of
  noise you prefer as well as selecting an appropriate listening volume.
*/

// Adafruit I2S support library for the Metro M4 Express
#include "Adafruit_ZeroI2S.h"
#define SAMPLERATE_HZ 44100 // Audio sample rate. 44.1khz is standard CD quality sound.
Adafruit_ZeroI2S i2s; // Create the I2S audio driver object

// Demo mode: Uncomment this #define to continuously loop through all available noise types
//#define DEMO;

// When not in DEMO mode, the selected noise type is hard-coded here. Choose from white, pink or brown.
enum NOISE { WHITE, PINK, BROWN };
NOISE noise = BROWN;

// Pink and brown noise buffers
float leftBuf0, leftBuf1, leftBuf2, leftBuf3, leftBuf4, leftBuf5, leftBuf6;
float rightBuf0, rightBuf1, rightBuf2, rightBuf3, rightBuf4, rightBuf5, rightBuf6;

// Parameters used by the brown noise algorithm
const float BROWN_LEAKAGE = 0.9967347f; // At 44100Hz
const float BROWN_SCALING = 0.0428571f; // At 44100Hz

// General storage used for noise calculations
float amplitude, white, brown, left, right;

// Random float number betwen -1.0 and +1.0
inline float randomFloat()
{
  return (float)((double)random(INT32_MAX) / (double)INT32_MAX * 2.0 - 1.0);
}

// Read the 10K pot on pin A5 and set the audio volume
void setVolume()
{
  int pot = 1023 - analogRead(A5); // Invert the pot so turning clockwise increases volume
  amplitude = (float)pot / 1024.0f; // Scale from 0.0 - 1.0 (inclusive to exclusive)
  if (amplitude != 0) amplitude = pow(amplitude, 4.0f); // Volume should be logarithmic, but the pot is probably linear
  amplitude = amplitude * (float)INT32_MAX;
#ifdef DEMO
  Serial.print("Volume="); Serial.println(amplitude);
#endif
}

// Configuration
void setup()
{
  // Configure serial port
  Serial.begin(115200);

  // Initialize the I2S driver
  if (!i2s.begin(I2S_32_BIT, SAMPLERATE_HZ)) {
    Serial.println("Failed to initialize I2S driver!");
    while (1);
  }
  i2s.enableTx();

  // Initialize the random number generator (assumes pin A0 is floating)
  randomSeed(analogRead(A0));

  // Volume control
  //amplitude = (float)INT32_MAX; // Max volume
  setVolume();
}

// Main loop
void loop()
{
#ifdef DEMO
  delay(100);
  setVolume();
  Serial.println("White");
  for (int i = 0; i < 100000; i++)
#else
  if (noise == WHITE)
#endif
  {
    // White noise
    left = amplitude * randomFloat();
    right = amplitude * randomFloat();
    i2s.write((long)left, (long)right);
  }
#ifdef DEMO
  delay(100);
  setVolume();
  Serial.println("Pink");
  for (int i = 0; i < 100000; i++)
#else
  else if (noise == PINK)
#endif
  {
    // Pink noise
    white = randomFloat();
    leftBuf0 = 0.99886f * leftBuf0 + 0.0555179f * white;
    leftBuf1 = 0.99332f * leftBuf1 + 0.0750759f * white;
    leftBuf2 = 0.96900f * leftBuf2 + 0.1538520f * white;
    leftBuf3 = 0.86650f * leftBuf3 + 0.3104856f * white;
    leftBuf4 = 0.55000f * leftBuf4 + 0.5329522f * white;
    leftBuf5 = -0.7616f * leftBuf5 - 0.0168980f * white;
    left = amplitude * 0.129f * (leftBuf0 + leftBuf1 + leftBuf2 + leftBuf3 + leftBuf4 + leftBuf5 + leftBuf6 + white * 0.5362f);
    if (left < INT32_MIN) left = INT32_MIN;
    if (left > INT32_MAX) left = INT32_MAX;
    white = randomFloat();
    rightBuf0 = 0.99886f * rightBuf0 + 0.0555179f * white;
    rightBuf1 = 0.99332f * rightBuf1 + 0.0750759f * white;
    rightBuf2 = 0.96900f * rightBuf2 + 0.1538520f * white;
    rightBuf3 = 0.86650f * rightBuf3 + 0.3104856f * white;
    rightBuf4 = 0.55000f * rightBuf4 + 0.5329522f * white;
    rightBuf5 = -0.7616f * rightBuf5 - 0.0168980f * white;
    right = amplitude * 0.129f * (rightBuf0 + rightBuf1 + rightBuf2 + rightBuf3 + rightBuf4 + rightBuf5 + rightBuf6 + white * 0.5362f);
    if (right < INT32_MIN) right = INT32_MIN;
    if (right > INT32_MAX) right = INT32_MAX;
    i2s.write((long)left, (long)right);
  }
#ifdef DEMO
  delay(100);
  setVolume();
  Serial.println("Brown");
  for (int i = 0; i < 100000; i++)
#else
  else if (noise == BROWN)
#endif
  {
    // Brown noise
    white = randomFloat();
    brown = BROWN_LEAKAGE * leftBuf0 + white * BROWN_SCALING;
    leftBuf0 = (abs(brown) > 1.0f) ? (BROWN_LEAKAGE * leftBuf0 - white * BROWN_SCALING) : brown;
    left = amplitude * leftBuf0;
    if (left < INT32_MIN) left = INT32_MIN;
    if (left > INT32_MAX) left = INT32_MAX;
    white = randomFloat();
    brown = BROWN_LEAKAGE * rightBuf0 + white * BROWN_SCALING;
    rightBuf0 = (abs(brown) > 1.0f) ? (BROWN_LEAKAGE * rightBuf0 - white * BROWN_SCALING) : brown;
    right = amplitude * rightBuf0;
    if (right < INT32_MIN) right = INT32_MIN;
    if (right > INT32_MAX) right = INT32_MAX;
    i2s.write((long)left, (long)right);
  }
}
