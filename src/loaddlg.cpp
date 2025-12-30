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

/* $Header:   F:\projects\c&c\vcs\code\loaddlg.cpv   2.18   16 Oct 1995 16:51:18   JOE_BOSTIC  $ */

#include "legacy/function.h"
#include "legacy/loaddlg.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <filesystem>

LoadOptionsClass::LoadOptionsClass(LoadStyleType style) {
	Style = style;
	Files.Clear();
}

LoadOptionsClass::~LoadOptionsClass() {
	for (int i = 0; i < Files.Count(); i++) {
		delete Files[i];
	}
	Files.Clear();
}

int LoadOptionsClass::Process(void) {
	/*
	**	Dialog & button dimensions
	*/
	int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;
	int d_dialog_w = 250 * factor;
	int d_dialog_h = 156 * factor;
	int d_dialog_x = (SeenBuff.Get_Width() - d_dialog_w) >> 1;
	int d_dialog_y = (SeenBuff.Get_Height() - d_dialog_h) >> 1;
	int d_dialog_cx = d_dialog_x + (d_dialog_w >> 1);
	int d_txt8_h = 11 * factor;
	int d_margin = 7 * factor;

	int d_list_w = d_dialog_w - (d_margin * 2);
	int d_list_h = 104 * factor;
	int d_list_x = d_dialog_x + d_margin;
	int d_list_y = d_dialog_y + d_margin + d_txt8_h + d_margin;

	int d_edit_w = d_dialog_w - (d_margin * 2);
	int d_edit_h = 13 * factor;
	int d_edit_x = d_dialog_x + d_margin;
	int d_edit_y = d_list_y + d_list_h - (30 * factor) + d_margin + d_txt8_h;

	int d_button_w = 40 * factor;
	int d_button_h = 13 * factor;
	int d_button_x = d_dialog_cx - d_button_w - d_margin;
	int d_button_y = d_dialog_y + d_dialog_h - d_button_h - d_margin;

	int d_cancel_w = 40 * factor;
	int d_cancel_h = 13 * factor;
	int d_cancel_x = d_dialog_cx + d_margin;
	int d_cancel_y = d_dialog_y + d_dialog_h - d_cancel_h - d_margin;

	enum {
		BUTTON_LOAD = 100,
		BUTTON_SAVE,
		BUTTON_DELETE,
		BUTTON_CANCEL,
		BUTTON_LIST,
		BUTTON_EDIT,
	};

	typedef enum {
		REDRAW_NONE = 0,
		REDRAW_BUTTONS,
		REDRAW_BACKGROUND,
		REDRAW_ALL = REDRAW_BACKGROUND
	} RedrawType;

	bool cancel = false;
	int list_ht = d_list_h;
	int btn_txt;
	int btn_id;
	int caption;
	int game_idx = 0;
	int game_num = 0;
	char game_descr[40] = {0};
	char fname[32];

	void const* up_button;
	void const* down_button;
	if (InMainLoop) {
		up_button = Hires_Retrieve("BTN-UP.SHP");
		down_button = Hires_Retrieve("BTN-DN.SHP");
	} else {
		up_button = Hires_Retrieve("BTN-UP2.SHP");
		down_button = Hires_Retrieve("BTN-DN2.SHP");
	}

	ControlClass* commands = nullptr;

	if (Style == LOAD) {
		btn_txt = TXT_LOAD_BUTTON;
		btn_id = BUTTON_LOAD;
		caption = TXT_LOAD_MISSION;
	} else if (Style == SAVE) {
		btn_txt = TXT_SAVE_BUTTON;
		btn_id = BUTTON_SAVE;
		caption = TXT_SAVE_MISSION;
		list_ht -= 30;
	} else {
		btn_txt = TXT_DELETE_BUTTON;
		btn_id = BUTTON_DELETE;
		caption = TXT_DELETE_MISSION;
	}

	GadgetClass* buttons = nullptr;
	TextButtonClass okbtn(btn_id, btn_txt, TPF_6PT_GRAD | TPF_NOSHADOW, d_button_x, d_button_y, d_button_w,
	                      d_button_h);
	TextButtonClass cancelbtn(BUTTON_CANCEL, TXT_CANCEL, TPF_6PT_GRAD | TPF_NOSHADOW, d_cancel_x, d_cancel_y,
	                          d_cancel_w, d_cancel_h);

	ListClass list(BUTTON_LIST, d_list_x, d_list_y, d_list_w, list_ht, TPF_6PT_GRAD, up_button, down_button);
	EditClass edit(BUTTON_EDIT, game_descr, sizeof(game_descr), TPF_6PT_GRAD, d_edit_x, d_edit_y, d_edit_w,
	               d_edit_h);

	commands = &okbtn;
	okbtn.Add_Tail(cancelbtn);
	cancelbtn.Add_Tail(list);
	list.Add_Tail(edit);
	buttons = commands;

	if (Style != SAVE) {
		edit.Flag_To_Redraw();
		edit.Disable();
	}

	Clear_List(&list);
	Fill_List(&list);
	list.Set_Selected_Index(game_idx);
	okbtn.Disable();
	okbtn.Flag_To_Redraw();

	HiddenPage.Clear();
	commands->Flag_To_Redraw();

	RedrawType redraw = REDRAW_ALL;
	while (!cancel) {
		if (redraw) {
			if (redraw >= REDRAW_BACKGROUND) {
				HiddenPage.Clear();
				Draw_Box(d_dialog_x, d_dialog_y, d_dialog_w, d_dialog_h, BOXSTYLE_GREEN_BORDER, true);
				Draw_Caption(caption, d_dialog_x, d_dialog_y, d_dialog_w);
				HidPage.Blit(SeenBuff);
			}
			if (redraw >= REDRAW_BUTTONS) {
				buttons->Draw_All(true);
			}
			redraw = REDRAW_NONE;
		}

		Call_Back();
		KeyNumType gadget_input = buttons->Input();

		KeyNumType input = KN_NONE;
		if (Keyboard::Check()) {
			input = Keyboard::Get();
		}

		if ((gadget_input & KN_BUTTON) && (gadget_input & ~KN_BUTTON) == BUTTON_CANCEL) {
			cancel = true;
			break;
		}
		if (input == KN_ESC) {
			cancel = true;
			break;
		}

		if ((gadget_input & KN_BUTTON) && (gadget_input & ~KN_BUTTON) == BUTTON_LIST) {
			game_idx = list.Current_Index();
			if (game_idx >= 0 && game_idx < Files.Count() && Files[game_idx]->Valid) {
				std::strncpy(game_descr, Files[game_idx]->Descr, sizeof(game_descr) - 1);
				game_descr[sizeof(game_descr) - 1] = '\0';
				if (Style == SAVE) {
					edit.Enable();
					edit.Flag_To_Redraw();
				}
				okbtn.Enable();
			} else {
				okbtn.Disable();
			}
			okbtn.Flag_To_Redraw();
			continue;
		}

		if ((gadget_input & KN_BUTTON) && (gadget_input & ~KN_BUTTON) == BUTTON_EDIT) {
			okbtn.Enable();
			okbtn.Flag_To_Redraw();
			continue;
		}

		if ((gadget_input & KN_BUTTON) && (gadget_input & ~KN_BUTTON) == btn_id) {
			game_idx = list.Current_Index();
			if (game_idx < 0 || game_idx >= Files.Count()) {
				continue;
			}
			game_num = Files[game_idx]->Num;
			if (Style == LOAD) {
				if (Load_Game(game_num)) {
					return true;
				}
				redraw = REDRAW_ALL;
				continue;
			}
			if (Style == SAVE) {
				if (Save_Game(game_num, game_descr)) {
					Clear_List(&list);
					Fill_List(&list);
				}
				redraw = REDRAW_ALL;
				continue;
			}

			std::snprintf(fname, sizeof(fname), "SAVEGAME.%03d", game_num);
			std::remove(fname);
			Clear_List(&list);
			Fill_List(&list);
			okbtn.Disable();
			okbtn.Flag_To_Redraw();
			redraw = REDRAW_ALL;
		}
	}

	return false;
}

void LoadOptionsClass::Clear_List(ListClass* list) {
	if (list) {
		while (list->Count() > 0) {
			const char* entry = list->Get_Item(0);
			if (!entry) break;
			list->Remove_Item(entry);
		}
		list->Flag_To_Redraw();
	}
	for (int i = 0; i < Files.Count(); i++) {
		delete Files[i];
	}
	Files.Clear();
}

static bool Starts_With_Case_Insensitive(std::string const& value, std::string const& prefix) {
	if (value.size() < prefix.size()) return false;
	for (std::size_t i = 0; i < prefix.size(); ++i) {
		const unsigned char a = static_cast<unsigned char>(value[i]);
		const unsigned char b = static_cast<unsigned char>(prefix[i]);
		if (std::tolower(a) != std::tolower(b)) return false;
	}
	return true;
}

void LoadOptionsClass::Fill_List(ListClass* list) {
	if (!list) return;

	namespace fs = std::filesystem;

	constexpr char kPrefix[] = "SAVEGAME.";
	for (const auto& entry : fs::directory_iterator(fs::current_path())) {
		if (!entry.is_regular_file()) continue;
		const std::string name = entry.path().filename().string();
		if (!Starts_With_Case_Insensitive(name, kPrefix)) continue;

		FileEntryClass* file = new FileEntryClass();
		std::memset(file, 0, sizeof(*file));

		file->Num = Num_From_Ext(const_cast<char*>(name.c_str()));
		if (file->Num < 0) {
			delete file;
			continue;
		}

		unsigned scen = 0;
		HousesType house = HOUSE_NONE;
		file->Valid = Get_Savefile_Info(file->Num, file->Descr, &scen, &house);
		file->Scenario = scen;
		file->House = house;

		std::error_code ec;
		const auto ts = entry.last_write_time(ec);
		if (!ec) {
			file->DateTime = static_cast<unsigned long>(ts.time_since_epoch().count());
		} else {
			file->DateTime = 0;
		}

		Files.Add(file);
	}

	if (Files.Count() > 1) {
		std::qsort(&Files[0], Files.Count(), sizeof(FileEntryClass*), Compare);
	}

	for (int i = 0; i < Files.Count(); ++i) {
		char buffer[96];
		if (!Files[i]->Valid) continue;
		std::snprintf(buffer, sizeof(buffer), "%s", Files[i]->Descr);
		list->Add_Item(buffer);
	}

	list->Flag_To_Redraw();
}

int LoadOptionsClass::Num_From_Ext(char* fname) {
	if (!fname) return -1;
	const char* dot = std::strrchr(fname, '.');
	if (!dot || !dot[1]) return -1;
	int value = 0;
	for (const char* c = dot + 1; *c; ++c) {
		if (!std::isdigit(static_cast<unsigned char>(*c))) return -1;
		value = value * 10 + (*c - '0');
	}
	return value;
}

int LoadOptionsClass::Compare(const void* p1, const void* p2) {
	const auto* a = *reinterpret_cast<FileEntryClass* const*>(p1);
	const auto* b = *reinterpret_cast<FileEntryClass* const*>(p2);
	if (!a && !b) return 0;
	if (!a) return 1;
	if (!b) return -1;

	if (a->DateTime < b->DateTime) return 1;
	if (a->DateTime > b->DateTime) return -1;
	return a->Num - b->Num;
}
