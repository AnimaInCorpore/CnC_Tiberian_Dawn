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

#include <algorithm>

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
	gadget.Next = Buttons;
	Buttons = &gadget;
}

void GScreenClass::Remove_A_Button(GadgetClass& gadget) {
	if (Buttons == &gadget) {
		Buttons = gadget.Next;
		return;
	}

	for (GadgetClass* cursor = Buttons; cursor; cursor = cursor->Next) {
		if (cursor->Next == &gadget) {
			cursor->Next = gadget.Next;
			break;
		}
	}
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
	if (ShadowPage) {
		GraphicViewPortClass shadow_view(ShadowPage, 0, 0, ShadowPage->Get_Width(), ShadowPage->Get_Height());
		shadow_view.Blit(HidPage);
	}
	SeenBuff.Blit(HidPage);
}

void GScreenClass::Code_Pointers(void) {}
void GScreenClass::Decode_Pointers(void) {}
void GScreenClass::Set_Default_Mouse(MouseType, bool) {}
bool GScreenClass::Override_Mouse_Shape(MouseType, bool) { return false; }
void GScreenClass::Revert_Mouse_Shape(void) {}
void GScreenClass::Mouse_Small(bool) {}
