#pragma once

/*
 * WW graphics compatibility layer (software surfaces + viewports).
 *
 * The original C&C95 code uses GraphicBufferClass / GraphicViewPortClass on top
 * of DirectDraw surfaces. For the portable SDL 1.2 build we emulate the same
 * interfaces using 8-bit software buffers that are presented via SDL.
 */

#include <cstddef>

class GraphicViewPortClass;

class GraphicBufferClass {
public:
    GraphicBufferClass();
    GraphicBufferClass(int width, int height);
    GraphicBufferClass(int width, int height, void* buffer);
    GraphicBufferClass(int width, int height, void* buffer, long buffer_size);
    ~GraphicBufferClass();

    bool Init(int width, int height, void* buffer, long buffer_size, int flags = 0);
    void Un_Init();

    int Get_Width() const;
    int Get_Height() const;
    int Get_Pitch() const;   // bytes per row
    int Get_XAdd() const;    // bytes to add after a row (pitch - width)
    int Get_LockCount() const;

    bool Get_IsDirectDraw() const;

    void* Get_Buffer();
    void const* Get_Buffer() const;
    void* Get_Offset();
    void const* Get_Offset() const;

    unsigned char* Data();
    unsigned char const* Data() const;
    int Pitch() const;

    bool Lock();
    void Unlock();

    void Clear(int color = 0);

    int Get_Pixel(int x, int y) const;
    void Put_Pixel(int x, int y, int color);
    void Fill_Rect(int x1, int y1, int x2, int y2, int color);
    void Draw_Rect(int x1, int y1, int x2, int y2, int color);
    void Draw_Line(int x1, int y1, int x2, int y2, int color);
    void Print(char const* text, int x, int y, int fore, int back);

    void Blit(GraphicBufferClass& dest);
    void Blit(GraphicBufferClass& dest, int dest_x, int dest_y);
    void Blit(GraphicBufferClass& dest,
              int src_x,
              int src_y,
              int dest_x,
              int dest_y,
              int width,
              int height,
              bool use_key);

    void Blit(GraphicViewPortClass& dest);
    void Blit(GraphicViewPortClass& dest, int dest_x, int dest_y);
    void Blit(GraphicViewPortClass& dest,
              int src_x,
              int src_y,
              int dest_x,
              int dest_y,
              int width,
              int height,
              bool use_key);

    void Scale(GraphicViewPortClass& dest,
               int src_x,
               int src_y,
               int dest_x,
               int dest_y,
               int src_w,
               int src_h,
               int dest_w,
               int dest_h);

    void Scale(GraphicViewPortClass& dest,
               int src_x,
               int src_y,
               int dest_x,
               int dest_y,
               int src_w,
               int src_h,
               int dest_w,
               int dest_h,
               bool use_key);

    void Scale(GraphicViewPortClass& dest,
               int src_x,
               int src_y,
               int dest_x,
               int dest_y,
               int src_w,
               int src_h,
               int dest_w,
               int dest_h,
               bool use_key,
               char const* remap_table);

    int To_Buffer(int x, int y, int width, int height, void* out, int out_bytes) const;

    // DirectDraw-specific helpers (stubbed in the SDL build).
    void Attach_DD_Surface(GraphicBufferClass* back_buffer);
    void* Get_DD_Surface() const;

private:
    GraphicBufferClass(GraphicBufferClass const&);
    GraphicBufferClass& operator=(GraphicBufferClass const&);

    void Ensure_Owned();

    int Width;
    int Height;
    int PitchBytes;
    int LockCount;

    unsigned char* Pixels;
    bool OwnsPixels;
    size_t OwnedSize;
};

class GraphicViewPortClass {
public:
    GraphicViewPortClass();
    GraphicViewPortClass(GraphicBufferClass* buffer, int x, int y, int width, int height);

    void Attach(GraphicBufferClass* buffer, int x, int y, int width, int height);
    GraphicBufferClass* Get_Graphic_Buffer();
    GraphicBufferClass const* Get_Graphic_Buffer() const;

    int Get_XPos() const;
    int Get_YPos() const;
    int Get_Width() const;
    int Get_Height() const;
    int Get_Pitch() const;
    int Get_XAdd() const;
    int Get_LockCount() const;

    bool Get_IsDirectDraw() const;

    void* Get_Offset();
    void const* Get_Offset() const;

    unsigned char* Data();
    unsigned char const* Data() const;
    int Pitch() const;

    bool Lock();
    void Unlock();

    void Clear(int color = 0);

    int Get_Pixel(int x, int y) const;
    void Put_Pixel(int x, int y, int color);
    void Fill_Rect(int x1, int y1, int x2, int y2, int color);
    void Draw_Rect(int x1, int y1, int x2, int y2, int color);
    void Draw_Line(int x1, int y1, int x2, int y2, int color);
    void Print(char const* text, int x, int y, int fore, int back);

    void Blit(GraphicBufferClass& dest);
    void Blit(GraphicBufferClass& dest, int dest_x, int dest_y);
    void Blit(GraphicBufferClass& dest,
              int src_x,
              int src_y,
              int dest_x,
              int dest_y,
              int width,
              int height,
              bool use_key = false);

    void Blit(GraphicViewPortClass& dest);
    void Blit(GraphicViewPortClass& dest, int dest_x, int dest_y);
    void Blit(GraphicViewPortClass& dest,
              int src_x,
              int src_y,
              int dest_x,
              int dest_y,
              int width,
              int height,
              bool use_key = false);

    void Scale(GraphicViewPortClass& dest,
               int src_x,
               int src_y,
               int dest_x,
               int dest_y,
               int src_w,
               int src_h,
               int dest_w,
               int dest_h,
               bool use_key = false,
               char const* remap_table = 0);

    int To_Buffer(int x, int y, int width, int height, void* out, int out_bytes) const;

private:
    GraphicBufferClass* Buffer;
    int X;
    int Y;
    int Width;
    int Height;
};

extern GraphicViewPortClass* LogicPage;

GraphicViewPortClass* Set_Logic_Page(GraphicViewPortClass& page);
