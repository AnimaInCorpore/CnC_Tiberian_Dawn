/*
**	Command & Conquer(tm)
**	Copyright 2025 Electronic Arts Inc.
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "legacy/gscreen.h"

#include "legacy/externs.h"
#include "legacy/function.h"

#include <SDL.h>

#include <algorithm>

#include "runtime_sdl.h"

namespace {

void Ensure_Page_Sizes() {
	if (!VisiblePage.Is_Valid()) {
		VisiblePage.Init(ScreenWidth, ScreenHeight, nullptr, 0, GBC_NONE);
	}
	if (!HiddenPage.Is_Valid()) {
		HiddenPage.Init(ScreenWidth, ScreenHeight, nullptr, 0, GBC_NONE);
	}
	SeenBuff.Configure(&VisiblePage, 0, 0, ScreenWidth, ScreenHeight);
	HidPage.Configure(&HiddenPage, 0, 0, ScreenWidth, ScreenHeight);
}

}  // namespace

GadgetClass* GScreenClass::Buttons = nullptr;
GraphicBufferClass* GScreenClass::ShadowPage = nullptr;

GScreenClass::GScreenClass(void) : IsToRedraw(true), IsToUpdate(true) {}

void GScreenClass::One_Time(void) {
	Buttons = nullptr;
	Ensure_Page_Sizes();
	if (!ShadowPage) {
		ShadowPage = new GraphicBufferClass(ScreenWidth, ScreenHeight);
	}
	if (ShadowPage) {
		ShadowPage->Clear();
	}
	HiddenPage.Clear();
}

void GScreenClass::Init(TheaterType theater) {
	Init_Clear();
	Init_IO();
	Init_Theater(theater);
}

void GScreenClass::Init_Clear(void) {
	if (ShadowPage) {
		ShadowPage->Clear();
	}
	HiddenPage.Clear();
	IsToRedraw = true;
	IsToUpdate = true;
}

void GScreenClass::Init_IO(void) { Buttons = nullptr; }

void GScreenClass::Init_Theater(TheaterType) {}

void GScreenClass::Input(KeyNumType&, int&, int&) {}

void GScreenClass::Add_A_Button(GadgetClass& gadget) {
	gadget.Remove();
	if (Buttons) {
		Buttons = static_cast<GadgetClass*>(&gadget.Add_Head(*Buttons));
	} else {
		Buttons = &gadget;
	}
}

void GScreenClass::Remove_A_Button(GadgetClass& gadget) {
	Buttons = static_cast<GadgetClass*>(gadget.Remove());
}

void GScreenClass::Flag_To_Redraw(bool complete) {
	IsToRedraw |= complete;
	IsToUpdate = true;
}

void GScreenClass::Render(void) {
	if (!IsToRedraw && !IsToUpdate) return;
	Draw_It(IsToRedraw);
	IsToUpdate = false;
	IsToRedraw = false;
	Blit_Display();
}

void GScreenClass::Blit_Display(void) {
  Ensure_Page_Sizes();

  GraphicViewPortClass* view = &HidPage;
  if (!view) {
    return;
  }

  GraphicBufferClass* buffer = view->Get_Graphic_Buffer();
  if (!buffer || !buffer->Is_Valid()) {
    return;
  }

  unsigned char* pixels = buffer->Get_Buffer();
  int width = buffer->Get_Width();
  int height = buffer->Get_Height();

  if (!pixels || width <= 0 || height <= 0) {
    return;
  }

  const unsigned char* palette = GamePalette ? GamePalette : Palette;
  if (!palette) {
    return;
  }

  SDL_Renderer* renderer = Runtime_Get_Sdl_Renderer();
  if (!renderer) {
    return;
  }
  int out_w = 0;
  int out_h = 0;
  if (SDL_GetRendererOutputSize(renderer, &out_w, &out_h) != 0 || out_w <= 0 || out_h <= 0) {
    SDL_Window* window = Runtime_Get_Sdl_Window();
    if (window) {
      SDL_GetWindowSize(window, &out_w, &out_h);
    }
  }

  SDL_Surface* surface =
      SDL_CreateRGBSurfaceFrom(pixels, width, height, 8, width, 0, 0, 0, 0);
  if (!surface) {
    return;
  }

  SDL_Color colors[256];
  for (int i = 0; i < 256; ++i) {
    const int offset = i * 3;
    auto expand = [](int value) { return std::clamp(value * 4, 0, 255); };
    colors[i].r = static_cast<Uint8>(expand(palette[offset + 0]));
    colors[i].g = static_cast<Uint8>(expand(palette[offset + 1]));
    colors[i].b = static_cast<Uint8>(expand(palette[offset + 2]));
    colors[i].a = 255;
  }
  SDL_SetPaletteColors(surface->format->palette, colors, 0, 256);

  SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  if (!texture) {
    return;
  }

  SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  SDL_RenderPresent(renderer);

  SDL_DestroyTexture(texture);
}

void GScreenClass::Code_Pointers(void) {}
void GScreenClass::Decode_Pointers(void) {}
void GScreenClass::Set_Default_Mouse(MouseType, bool) {}
bool GScreenClass::Override_Mouse_Shape(MouseType, bool) { return false; }
void GScreenClass::Revert_Mouse_Shape(void) {}
void GScreenClass::Mouse_Small(bool) {}
