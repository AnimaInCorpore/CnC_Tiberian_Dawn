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

/* $Header:   F:\projects\c&c\vcs\code\monoc.cpv   2.13   16 Oct 1995 16:50:36   JOE_BOSTIC  $ */
#include "legacy/function.h"
#include "legacy/monoc.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

int MonoClass::Enabled = 0;
MonoClass * MonoClass::PageUsage[MonoClass::MAX_MONO_PAGES] = {nullptr};
void * MonoClass::MonoSegment = nullptr;

/*
**	These are the IBM linedraw characters.
*/
MonoClass::BoxDataType const MonoClass::CharData[MonoClass::COUNT] = {
	{0xDA,0xC4,0xBF,0xB3,0xD9,0xC4,0xC0,0xB3},	// Single line
	{0xD5,0xCD,0xB8,0xB3,0xBE,0xCD,0xD4,0xB3},	// Double horz.
	{0xD6,0xC4,0xB7,0xBA,0xBD,0xC4,0xD3,0xBA},	// Double vert.
	{0xC9,0xCD,0xBB,0xBA,0xBC,0xCD,0xC8,0xBA}		// Double horz and vert.
};

MonoClass::MonoClass(void)
{
	int	index;

	Attrib = DEFAULT_ATTRIBUTE;
	X = Y = 0;
	for (index = 0; index < MAX_MONO_PAGES; index++) {
		if (!PageUsage[index]) {
			PageUsage[index] = this;
			Page = static_cast<char>(index);
			break;
		}
	}
	if (index == MAX_MONO_PAGES) {
		delete this;
		return;
	}

	if (!MonoSegment) {
		MonoSegment = new char[SIZE_OF_PAGE * MAX_MONO_PAGES];
		std::memset(MonoSegment, 0, SIZE_OF_PAGE * MAX_MONO_PAGES);
	}
}

MonoClass::~MonoClass(void)
{
	PageUsage[Page] = nullptr;
}

void MonoClass::Draw_Box(int x, int y, int w, int h, char attrib, BoxStyleType thick)
{
	CellType	cell;
	char	oldattrib = Attrib;

	if (!Enabled || !w || !h) return;

	cell.Attribute = attrib;

	for (int xpos = 0; xpos < w-2; xpos++) {
		cell.Character = CharData[thick].TopEdge;
		Store_Cell(cell, x+xpos+1, y);
		cell.Character = CharData[thick].BottomEdge;
		Store_Cell(cell, x+xpos+1, y+h-1);
	}

	for (int ypos = 0; ypos < h-2; ypos++) {
		cell.Character = CharData[thick].LeftEdge;
		Store_Cell(cell, x, y+ypos+1);
		cell.Character = CharData[thick].RightEdge;
		Store_Cell(cell, x+w-1, y+ypos+1);
	}

	if (w > 1 && h > 1) {
		cell.Character = CharData[thick].UpperLeft;
		Store_Cell(cell, x, y);
		cell.Character = CharData[thick].UpperRight;
		Store_Cell(cell, x+w-1, y);
		cell.Character = CharData[thick].BottomRight;
		Store_Cell(cell, x+w-1, y+h-1);
		cell.Character = CharData[thick].BottomLeft;
		Store_Cell(cell, x, y+h-1);
	}

	Attrib = oldattrib;
}

void MonoClass::Set_Cursor(int x, int y)
{
	if (!Enabled) return;

	int clamped_x = x;
	int clamped_y = y;

	if (clamped_x < 0) clamped_x = 0;
	if (clamped_y < 0) clamped_y = 0;
	if (clamped_x >= COLUMNS) clamped_x = COLUMNS - 1;
	if (clamped_y >= LINES) clamped_y = LINES - 1;

	X = static_cast<char>(clamped_x);
	Y = static_cast<char>(clamped_y);
}

void MonoClass::Clear(void)
{
	CellType	cell;

	if (!Enabled) return;

	Set_Cursor(0, 0);

	cell.Attribute = Attrib;
	cell.Character = ' ';

	for (int y = 0; y < LINES; y++) {
		for (int x = 0; x < COLUMNS; x++) {
			Store_Cell(cell, x, y);
		}
	}
}

void MonoClass::Scroll(int lines)
{
	CellType	cell;

	if (!Enabled || lines <= 0) return;

	std::memmove(static_cast<char*>(MonoSegment) + Offset(0, 0),
				static_cast<char*>(MonoSegment) + Offset(0, lines),
				(LINES-lines)*COLUMNS*sizeof(CellType));

	Y--;
	cell.Attribute = Attrib;
	cell.Character = ' ';

	for (int l = LINES-lines; l < LINES; l++) {
		for (int index = 0; index < COLUMNS; index++) {
			Store_Cell(cell, index, l);
		}
	}
}

void MonoClass::Printf(char const *text, ...)
{
	va_list	va;
	char buffer[256];

	if (!Enabled) return;

	va_start(va, text);
	std::vsnprintf(buffer, sizeof(buffer), text, va);
	buffer[sizeof(buffer)-1] = '\0';

	Print(buffer);
	va_end(va);
}

void MonoClass::Print(char const *ptr)
{
	char startcol = X;
	char const * text;
	CellType	cell;

	if (!ptr || !Enabled) return;

	text = ptr;
	cell.Attribute = Attrib;
	while (*text) {
		switch (*text) {
			case '\r':
				X = startcol;
				Y++;
				Scroll(Y-(LINES-1));
				break;
			case '\n':
				X = 0;
				Y++;
				Scroll(Y-(LINES-1));
				break;
			default:
				cell.Character = *text;
				Store_Cell(cell, X, Y);

				X++;
				if (X >= COLUMNS) {
					X = 0;
					Y++;

					if (Y > (LINES-1)) {
						Scroll(Y-(LINES-1));
					}
				}
				break;

		}
		text++;
	}

	Set_Cursor(X, Y);
}

void MonoClass::Text_Print(char const *text, int x, int y, char attrib)
{
	char	oldx = X;
	char	oldy = Y;
	char	oldattrib = Attrib;

	X = static_cast<char>(x);
	Y = static_cast<char>(y);
	Attrib = attrib;
	Print(text);
	Attrib = oldattrib;
	Set_Cursor(oldx, oldy);
}

MonoClass & MonoClass::operator = (MonoClass const & src)
{
	std::memcpy(static_cast<char*>(MonoSegment) + src.Offset(0, 0),
				static_cast<char*>(MonoSegment) + Offset(0, 0),
				SIZE_OF_PAGE);
	Set_Cursor(src.X, src.Y);
	return(*this);
}

void MonoClass::View(void)
{
	MonoClass *displace;

	if (Get_Current() == this) return;

	displace = Get_Current();
	if (displace) {
		char temp[SIZE_OF_PAGE];

		std::memcpy(&temp[0], MonoSegment, SIZE_OF_PAGE);
		std::memcpy(MonoSegment, static_cast<char*>(MonoSegment) + Offset(0, 0), SIZE_OF_PAGE);
		std::memcpy(static_cast<char*>(MonoSegment) + Offset(0, 0), &temp[0], SIZE_OF_PAGE);

		displace->Page = Page;

	} else {
		std::memcpy(MonoSegment, static_cast<char*>(MonoSegment) + Offset(0, 0), SIZE_OF_PAGE);
	}
	PageUsage[Page] = displace;
	PageUsage[0] = this;
	Page = 0;

	Set_Cursor(X, Y);
}

void Mono_Set_Cursor(int x, int y)
{
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}
		mono->Set_Cursor(x, y);
	}
}

int Mono_Printf(char const *string, ...)
{
	va_list	va;
	char buffer[256];

	buffer[0] = '\0';
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}

		va_start(va, string);
		std::vsnprintf(buffer, sizeof(buffer), string, va);
		buffer[sizeof(buffer)-1] = '\0';

		mono->Print(buffer);

		va_end(va);
	}
	return(static_cast<short>(std::strlen(buffer)));
}


void Mono_Clear_Screen(void)
{
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}
		mono->Clear();
	}
}

void Mono_Text_Print(void const *text, int x, int y, int attrib)
{
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}
		mono->Text_Print((const char*)text, x, y, (char)attrib);
	}
}

void Mono_Draw_Rect(int x, int y, int w, int h, int attrib, int thick)
{
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}
		mono->Draw_Box(x, y, w, h, (char)attrib, (MonoClass::BoxStyleType)thick);
	}
}

void Mono_Print(void const *text)
{
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}
		mono->Print((const char*)text);
	}
}

int Mono_X(void)
{
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}
		return(short)mono->Get_X();
	}
	return(0);
}

int Mono_Y(void)
{
	if (MonoClass::Is_Enabled()) {
		MonoClass *mono = MonoClass::Get_Current();
		if (!mono) {
			mono = new MonoClass();
			mono->View();
		}
		return(short)mono->Get_X();
	}
	return(0);
}


void Mono_Put_Char(char , int )
{
}

void Mono_Scroll(int )
{
}

void Mono_View_Page(int )
{
}
