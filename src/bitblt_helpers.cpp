#include "function.h"

#include <vector>

namespace {

inline bool Buffer_Valid_For_Pixels(const GraphicBufferClass* buffer) {
  if (!buffer) return false;
  if (!buffer->Is_Valid()) return false;
  if (!buffer->Get_Buffer()) return false;
  if (buffer->Get_Width() <= 0 || buffer->Get_Height() <= 0) return false;
  return true;
}

inline unsigned char Buffer_Get_Pixel(const GraphicBufferClass* buffer, int x, int y) {
  if (!Buffer_Valid_For_Pixels(buffer)) return 0;
  if (x < 0 || y < 0) return 0;
  if (x >= buffer->Get_Width() || y >= buffer->Get_Height()) return 0;
  return buffer->Get_Buffer()[(y * buffer->Get_Width()) + x];
}

inline void Buffer_Put_Pixel(GraphicBufferClass* buffer, int x, int y, unsigned char value) {
  if (!Buffer_Valid_For_Pixels(buffer)) return;
  if (x < 0 || y < 0) return;
  if (x >= buffer->Get_Width() || y >= buffer->Get_Height()) return;
  buffer->Get_Buffer()[(y * buffer->Get_Width()) + x] = value;
}

}  // namespace

void Bit_It_In_Scale(int x, int y, int w, int h, GraphicBufferClass* src, GraphicBufferClass* dest,
                     GraphicViewPortClass*, int delay, int dagger) {
  if (!src || !dest) return;
  if (w <= 0 || h <= 0) return;

  std::vector<short> xindex(static_cast<std::size_t>(w));
  std::vector<short> yindex(static_cast<std::size_t>(h));

  for (int i = 0; i < w; ++i) xindex[static_cast<std::size_t>(i)] = static_cast<short>(i);
  for (int i = 0; i < h; ++i) yindex[static_cast<std::size_t>(i)] = static_cast<short>(i);

  for (int i = 0; i < w; ++i) {
    const unsigned int k = IRandom(0, w - 1);
    const unsigned int m = static_cast<unsigned int>(i);
    const short n = xindex[k];
    xindex[k] = xindex[m];
    xindex[m] = n;
  }

  for (int i = 0; i < h; ++i) {
    const unsigned int k = IRandom(0, h - 1);
    const unsigned int m = static_cast<unsigned int>(i);
    const short n = yindex[k];
    yindex[k] = yindex[m];
    yindex[m] = n;
  }

  for (int j = 0; j < h; ++j) {
    if (j & 1) {
      int frames = delay;
      do {
        Call_Back_Delay(frames ? 1 : 0);
      } while (frames--);
    } else {
      Call_Back();
    }

    if (src->Lock() && dest->Lock()) {
      int j1 = j;
      for (int i = 0; i < w; ++i) {
        const int pixel_x = x + xindex[static_cast<std::size_t>(i)];
        const int pixel_y = y + yindex[static_cast<std::size_t>(j1)];
        ++j1;
        if (j1 >= h) j1 = 0;
        Buffer_Put_Pixel(dest, pixel_x, pixel_y, Buffer_Get_Pixel(src, pixel_x, pixel_y));
      }

      if (dagger) {
        for (int q = j; q >= 0; --q) {
          const int offset = j - q;
          Buffer_Put_Pixel(dest, 160 - offset, q, Buffer_Get_Pixel(src, 160 - offset, q));
          Buffer_Put_Pixel(dest, 160 + offset, q, Buffer_Get_Pixel(src, 160 + offset, q));
        }
      }
    }
    src->Unlock();
    dest->Unlock();
  }
}
