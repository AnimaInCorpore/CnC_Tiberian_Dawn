#pragma once

#include <cstdint>

struct VQAHandle;

using VQADrawerCallback = long (*)(unsigned char* buffer, long frame);

struct VQAConfig {
  int DrawFlags = 0;
  int FrameRate = 0;
  int DrawRate = 0;
  VQADrawerCallback DrawerCallback = nullptr;
  int ImageWidth = 0;
  int ImageHeight = 0;
  int Vmode = 0;
  unsigned char* ImageBuf = nullptr;
  int OptionFlags = 0;
  unsigned char* AudioBuf = nullptr;
  std::uint32_t AudioBufSize = 0;
  int DigiCard = 0;
  int HMIBufSize = 0;
  int DigiHandle = 0;
  int Volume = 0;
  int AudioRate = 0;
  void* SoundObject = nullptr;
  void* PrimaryBufferPtr = nullptr;
  char* EVAFont = nullptr;
  char* CapFont = nullptr;
};

using _VQAConfig = VQAConfig;
