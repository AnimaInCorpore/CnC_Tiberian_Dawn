#pragma once

#include "legacy_compat.h"

#include <cstdlib>

typedef int KeyNumType;

enum {
    LEFTPRESS = 1u << 0
};

class ListClass {
public:
    ListClass(int id, int x, int y, int w, int h, TextPrintType flags, void const* up, void const* down)
        : Id(id),
          X(x),
          Y(y),
          Width(w),
          Height(h),
          TextFlags(flags),
          Tabs(NULL),
          LineHeight(10),
          UpShape(up),
          DownShape(down),
          SelectedIndex(0) {}

    virtual ~ListClass() { Clear(); }

    int Count() const { return List.Count(); }

    char* Item_At(int index) {
        if (index < 0 || index >= List.Count()) return NULL;
        return List[index];
    }

    char const* Item_At(int index) const {
        if (index < 0 || index >= List.Count()) return NULL;
        return List[index];
    }

    int Add_Item(char const* text) {
        if (!text) return -1;
        char* copy = Duplicate_(text);
        List.Add(copy);
        return List.Count() - 1;
    }

    int Add_Item(int text) {
        char buffer[32];
        ::snprintf(buffer, sizeof(buffer), "%d", text);
        buffer[sizeof(buffer) - 1] = '\0';
        return Add_Item(buffer);
    }

    virtual void Remove_Item(char const* text) {
        int index = ID(text);
        if (index < 0) return;
        std::free(List[index]);
        List.Delete(index);
        if (SelectedIndex >= List.Count()) SelectedIndex = 0;
    }

    void Clear() {
        for (int i = 0; i < List.Count(); ++i) {
            std::free(List[i]);
        }
        List.Clear();
        SelectedIndex = 0;
    }

protected:
    int ID(char const* text) const {
        if (!text) return -1;
        for (int i = 0; i < List.Count(); ++i) {
            if (List[i] && std::strcmp(List[i], text) == 0) return i;
        }
        return -1;
    }

    virtual int Action(unsigned, KeyNumType&) { return 0; }

protected:
    DynamicVectorClass<char*> List;
    int SelectedIndex;
    TextPrintType TextFlags;
    int const* Tabs;
    int LineHeight;

private:
    static char* Duplicate_(char const* text) {
        size_t len = std::strlen(text);
        char* copy = static_cast<char*>(std::malloc(len + 1));
        if (!copy) return NULL;
        std::memcpy(copy, text, len + 1);
        return copy;
    }

private:
    int Id;
    int X;
    int Y;
    int Width;
    int Height;
    void const* UpShape;
    void const* DownShape;
};
