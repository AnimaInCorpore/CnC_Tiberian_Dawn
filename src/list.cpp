/*
**  Command & Conquer(tm)
**  Copyright 2025 Electronic Arts Inc.
**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/* $Header:   F:\\projects\\c&c\\vcs\\code\\list.cpv   2.17   16 Oct 1995 16:51:36   JOE_BOSTIC  $ */

#include "list.h"

#include "function.h"

#include <cstdlib>
#include <cstring>

char* ListClass::Duplicate_(char const* text)
{
    if (!text) return 0;
    const size_t len = std::strlen(text);
    char* copy = static_cast<char*>(std::malloc(len + 1));
    if (!copy) return 0;
    std::memcpy(copy, text, len + 1);
    return copy;
}

void ListClass::Free_(char* text)
{
    if (!text) return;
    std::free(text);
}

ListClass::ListClass(int id, int x, int y, int w, int h, TextPrintType flags, void const* up, void const* down)
    : ControlClass(id, x, y, w, h, LEFTPRESS | LEFTRELEASE | KEYBOARD, false),
      UpGadget(0, up, x + w, y),
      DownGadget(0, down, x + w, y + h),
      ScrollGadget(0, x + w, y, 0, h, true)
{
    UpGadget.X -= UpGadget.Width;
    DownGadget.X -= DownGadget.Width;
    DownGadget.Y -= DownGadget.Height;
    ScrollGadget.X -= MAX(UpGadget.Width, DownGadget.Width);
    ScrollGadget.Y = Y + UpGadget.Height;
    ScrollGadget.Height -= UpGadget.Height + DownGadget.Height;
    ScrollGadget.Width = MAX(UpGadget.Width, DownGadget.Width);

    TextFlags = flags;
    IsScrollActive = false;
    Tabs = 0;
    SelectedIndex = 0;
    CurrentTopIndex = 0;
    Fancy_Text_Print(TXT_NONE, 0, 0, TBLACK, TBLACK, TextFlags);
    LineHeight = FontHeight + FontYSpacing - 1;
    LineCount = (h - 1) / LineHeight;
}

ListClass::~ListClass(void)
{
    Remove_Scroll_Bar();

    for (int i = 0; i < List.Count(); ++i) {
        Free_(List[i]);
    }
    List.Clear();
}

int ListClass::Add_Item(char const* text)
{
    if (text) {
        char* copy = Duplicate_(text);
        if (copy) {
            List.Add(copy);
        }

        Flag_To_Redraw();

        if (List.Count() > LineCount) {
            Add_Scroll_Bar();
        }

        if (IsScrollActive) {
            ScrollGadget.Set_Maximum(List.Count());
        }
    }
    return List.Count() - 1;
}

int ListClass::Add_Item(int text)
{
    if (text != TXT_NONE) {
        Add_Item(Text_String(text));
    }
    return List.Count() - 1;
}

void ListClass::Remove_Item(char const* text)
{
    if (!text) return;

    int index = -1;
    for (int i = 0; i < List.Count(); ++i) {
        if (!List[i]) continue;
        if (std::strcmp(List[i], text) == 0) {
            index = i;
            break;
        }
    }
    if (index < 0) return;

    Free_(List[index]);
    List.Delete(index);

    if (List.Count() <= LineCount) {
        Remove_Scroll_Bar();
    }

    if (IsScrollActive) {
        ScrollGadget.Set_Maximum(List.Count());
    }

    if (SelectedIndex >= List.Count()) {
        SelectedIndex--;
        if (SelectedIndex < 0) SelectedIndex = 0;
    }

    if (CurrentTopIndex >= List.Count()) {
        CurrentTopIndex--;
        if (CurrentTopIndex < 0) CurrentTopIndex = 0;
        if (IsScrollActive) {
            ScrollGadget.Step(1);
        }
    }
}

int ListClass::Current_Index(void) { return SelectedIndex; }

char const* ListClass::Current_Item(void)
{
    if (SelectedIndex < 0 || SelectedIndex >= List.Count()) return 0;
    return List[SelectedIndex];
}

char const* ListClass::Get_Item(int index) const
{
    if (index < 0 || index >= List.Count()) return 0;
    return List[index];
}

int ListClass::Action(unsigned flags, KeyNumType& key)
{
    if (flags & LEFTRELEASE) {
        key = KN_NONE;
        flags &= (~LEFTRELEASE);
        ControlClass::Action(flags, key);
        return true;
    } else {
        if (flags & KEYBOARD) {
            if (key == KN_UP) {
                Step_Selected_Index(-1);
                key = KN_NONE;
            } else if (key == KN_DOWN) {
                Step_Selected_Index(1);
                key = KN_NONE;
            } else {
                flags &= ~KEYBOARD;
            }
        } else {
            int index = Get_Mouse_Y() - (Y + 1);
            index = index / LineHeight;
            SelectedIndex = CurrentTopIndex + index;
            SelectedIndex = MIN(SelectedIndex, List.Count() - 1);
        }
    }
    return ControlClass::Action(flags, key);
}

int ListClass::Draw_Me(int forced)
{
    if (GadgetClass::Draw_Me(forced)) {
        if (LogicPage == &SeenBuff) {
            Conditional_Hide_Mouse(X, Y, X + Width, Y + Height);
        }

        Draw_Box(X, Y, Width, Height, BOXSTYLE_GREEN_BOX, true);

        if (List.Count()) {
            for (int index = 0; index < LineCount; index++) {
                int line = CurrentTopIndex + index;
                if (List.Count() > line) {
                    Draw_Entry(line, X + 1, Y + (LineHeight * index) + 1, Width - 2, (line == SelectedIndex));
                }
            }
        }

        if (LogicPage == &SeenBuff) {
            Conditional_Show_Mouse();
        }
        return true;
    }
    return false;
}

void ListClass::Bump(int up)
{
    if (IsScrollActive) {
        if (ScrollGadget.Step(up)) {
            CurrentTopIndex = ScrollGadget.Get_Value();
            Flag_To_Redraw();
        }
    }
}

void ListClass::Step(int up)
{
    if (IsScrollActive) {
        if (ScrollGadget.Step(up)) {
            CurrentTopIndex = ScrollGadget.Get_Value();
            Flag_To_Redraw();
        }
    }
}

void ListClass::Peer_To_Peer(unsigned flags, KeyNumType&, ControlClass& whom)
{
    if (flags & LEFTRELEASE) {
        if (&whom == &UpGadget) {
            Step(true);
        }
        if (&whom == &DownGadget) {
            Step(false);
        }
    }

    if (&whom == &ScrollGadget) {
        Set_View_Index(ScrollGadget.Get_Value());
    }
}

int ListClass::Set_View_Index(int index)
{
    int max_top = List.Count() - LineCount;
    if (max_top < 0) max_top = 0;

    index = Bound(index, 0, max_top);
    if (index != CurrentTopIndex) {
        CurrentTopIndex = index;
        Flag_To_Redraw();
        if (IsScrollActive) {
            ScrollGadget.Set_Value(CurrentTopIndex);
        }
        return true;
    }
    return false;
}

int ListClass::Add_Scroll_Bar(void)
{
    if (!IsScrollActive) {
        IsScrollActive = true;

        Flag_To_Redraw();
        Width -= ScrollGadget.Width;

        UpGadget.Make_Peer(*this);
        DownGadget.Make_Peer(*this);
        ScrollGadget.Make_Peer(*this);

        Add_Tail(UpGadget);
        Add_Tail(DownGadget);
        Add_Tail(ScrollGadget);

        UpGadget.Flag_To_Redraw();
        DownGadget.Flag_To_Redraw();
        ScrollGadget.Flag_To_Redraw();

        ScrollGadget.Set_Maximum(List.Count());
        ScrollGadget.Set_Thumb_Size(LineCount);
        ScrollGadget.Set_Value(CurrentTopIndex);

        return true;
    }
    return false;
}

int ListClass::Remove_Scroll_Bar(void)
{
    if (IsScrollActive) {
        IsScrollActive = false;
        Width += ScrollGadget.Width;
        ScrollGadget.Remove();
        UpGadget.Remove();
        DownGadget.Remove();
        Flag_To_Redraw();
        return true;
    }
    return false;
}

void ListClass::Set_Tabs(int const* tabs) { Tabs = tabs; }

void ListClass::Draw_Entry(int index, int x, int y, int width, int selected)
{
    if (!List[index]) return;

    if (TextFlags & TPF_6PT_GRAD) {
        TextPrintType flags = TextFlags;

        if (selected) {
            flags = (TextPrintType)(flags | TPF_BRIGHT_COLOR);
            LogicPage->Fill_Rect(x, y, x + width - 1, y + LineHeight - 1, CC_GREEN_SHADOW);
        } else {
            if (!(flags & TPF_USE_GRAD_PAL)) {
                flags = (TextPrintType)(flags | TPF_MEDIUM_COLOR);
            }
        }

        Conquer_Clip_Text_Print(List[index], x, y, CC_GREEN, TBLACK, flags, width, Tabs);
    } else {
        Conquer_Clip_Text_Print(List[index], x, y, (selected ? BLUE : WHITE), TBLACK, TextFlags, width, Tabs);
    }
}

GadgetClass* ListClass::Remove(void)
{
    if (IsScrollActive) {
        ScrollGadget.Remove();
        DownGadget.Remove();
        UpGadget.Remove();
    }

    return ControlClass::Remove();
}

void ListClass::Set_Selected_Index(int index)
{
    if ((unsigned)index < (unsigned)List.Count()) {
        SelectedIndex = index;
        Flag_To_Redraw();
        if (SelectedIndex < CurrentTopIndex) {
            Set_View_Index(SelectedIndex);
        }
        if (SelectedIndex >= CurrentTopIndex + LineCount) {
            Set_View_Index(SelectedIndex - (LineCount - 1));
        }
    }
}

int ListClass::Step_Selected_Index(int step)
{
    int old = SelectedIndex;
    Set_Selected_Index(old + step);
    return old;
}

