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

#include <cctype>
#include <chrono>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <limits>
#include <system_error>

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

	bool cancel = false; // true = user cancels
	int list_ht = d_list_h;
	int btn_txt;
	int btn_id;
	int caption;
	int game_idx = 0;
	int game_num = 0;
	char game_descr[40] = {0};
	char fname[13];

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

	TextButtonClass okbtn(btn_id, btn_txt, TPF_6PT_GRAD | TPF_CENTER | TPF_NOSHADOW, d_button_x, d_button_y,
	                      d_button_w, d_button_h);
	TextButtonClass cancelbtn(BUTTON_CANCEL, TXT_CANCEL, TPF_6PT_GRAD | TPF_CENTER | TPF_NOSHADOW, d_cancel_x,
	                          d_cancel_y, d_cancel_w, d_cancel_h);

	ListClass list(BUTTON_LIST, d_list_x, d_list_y, d_list_w, list_ht, TPF_6PT_GRAD | TPF_NOSHADOW, up_button,
	               down_button);
	EditClass edit(BUTTON_EDIT, game_descr, sizeof(game_descr), TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
	               d_edit_x, d_edit_y, d_edit_w, d_edit_h, EditClass::ALPHANUMERIC);

	Set_Logic_Page(SeenBuff);

	Fill_List(&list);

	if ((Style == LOAD || Style == WWDELETE) && list.Count() == 0) {
		Clear_List(&list);
		CCMessageBox().Process(TXT_NO_SAVES);
		return false;
	}

	commands = &okbtn;
	okbtn.Add_Tail(cancelbtn);
	cancelbtn.Add_Tail(list);
	if (Style == SAVE) {
		list.Add_Tail(edit);
		edit.Set_Focus();
	}

	bool firsttime = true;
	bool display = true;
	bool process = true;

	while (process) {
		if (GameToPlay == GAME_NORMAL) {
			Call_Back();
		} else {
			if (Main_Loop()) {
				process = false;
				cancel = true;
			}
		}

		if (AllSurfaces.SurfacesRestored) {
			AllSurfaces.SurfacesRestored = false;
			display = true;
		}

		if (display) {
			Hide_Mouse();

			if (InMainLoop) {
				HiddenPage.Clear();
				Map.Flag_To_Redraw(true);
				Map.Render();
			} else {
				HiddenPage.Clear();
				char title_pcx[] = "HTITLE.PCX";
				Load_Title_Screen(title_pcx, &HidPage, Palette);
				HidPage.Blit(SeenBuff);
			}

			Dialog_Box(d_dialog_x, d_dialog_y, d_dialog_w, d_dialog_h);
			Draw_Caption(caption, d_dialog_x, d_dialog_y, d_dialog_w);

			if (Style == SAVE) {
				Fancy_Text_Print(TXT_MISSION_DESCRIPTION, d_dialog_cx, d_edit_y - d_txt8_h, CC_GREEN, TBLACK,
				                 TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_CENTER | TPF_NOSHADOW);
			}

			commands->Flag_List_To_Redraw();
			Show_Mouse();
			display = false;
		}

		KeyNumType input = commands->Input();

		if (firsttime && Style == SAVE) {
			firsttime = false;
			edit.Set_Focus();
			edit.Flag_To_Redraw();
		}

		if (input == KN_RETURN) {
			switch (Style) {
				case SAVE:
					input = static_cast<KeyNumType>(BUTTON_SAVE | KN_BUTTON);
					break;
				case LOAD:
					input = static_cast<KeyNumType>(BUTTON_LOAD | KN_BUTTON);
					break;
				case WWDELETE:
					input = static_cast<KeyNumType>(BUTTON_DELETE | KN_BUTTON);
					break;
				default:
					break;
			}
		}

		switch (input) {
			case (BUTTON_LOAD | KN_BUTTON):
				game_idx = list.Current_Index();
				game_num = Files[game_idx]->Num;
				if (Files[game_idx]->Valid) {
					CCMessageBox().Process(TXT_LOADING, TXT_NONE);
					if (!Load_Game(game_num)) {
						CCMessageBox().Process(TXT_ERROR_LOADING_GAME);
					} else {
						Hide_Mouse();
						VisiblePage.Clear();
						Set_Palette(GamePalette);
						Show_Mouse();
						process = false;
					}
				} else {
					CCMessageBox().Process(TXT_OBSOLETE_SAVEGAME);
				}
				break;

			case (BUTTON_SAVE | KN_BUTTON):
				if (!std::strlen(game_descr)) {
					CCMessageBox().Process(TXT_MUSTENTER_DESCRIPTION);
					firsttime = true;
					display = true;
					break;
				}
				game_idx = list.Current_Index();
				if (Disk_Space_Available() < SAVE_GAME_DISK_SPACE && game_idx == 0) {
					CCMessageBox().Process(TXT_SPACE_CANT_SAVE);
					firsttime = true;
					display = true;
					break;
				}
				game_num = Files[game_idx]->Num;
				if (!Save_Game(game_num, game_descr)) {
					CCMessageBox().Process(TXT_ERROR_SAVING_GAME);
				} else {
					CCMessageBox().Process(TXT_GAME_WAS_SAVED, TXT_NONE, TXT_NONE);
				}
				process = false;
				break;

			case (BUTTON_DELETE | KN_BUTTON):
				game_idx = list.Current_Index();
				game_num = Files[game_idx]->Num;
				if (CCMessageBox().Process(TXT_DELETE_FILE_QUERY, TXT_YES, TXT_NO) == 0) {
					std::snprintf(fname, sizeof(fname), "SAVEGAME.%03d", game_num);
					std::remove(fname);
					Clear_List(&list);
					Fill_List(&list);
					if (list.Count() == 0) {
						process = false;
					}
				}
				display = true;
				break;

			case (BUTTON_LIST | KN_BUTTON):
				if (Style != SAVE) {
					break;
				}
				if (list.Count() && list.Current_Index() != game_idx) {
					game_idx = list.Current_Index();
					if (game_idx != 0) {
						std::strncpy(game_descr, list.Get_Item(game_idx), sizeof(game_descr) - 1);
						game_descr[sizeof(game_descr) - 1] = '\0';
					} else {
						game_descr[0] = '\0';
					}
					edit.Set_Text(game_descr, sizeof(game_descr));
				}
				break;

			case (KN_ESC):
			case (BUTTON_CANCEL | KN_BUTTON):
				cancel = true;
				process = false;
				break;

			default:
				break;
		}
	}

	Clear_List(&list);

	if (cancel) return false;
	return true;
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

static unsigned long File_Time_Seconds(std::filesystem::file_time_type ft) {
	using namespace std::chrono;
	const auto sctp = time_point_cast<system_clock::duration>(ft - std::filesystem::file_time_type::clock::now()
	                                                        + system_clock::now());
	const auto seconds = duration_cast<std::chrono::seconds>(sctp.time_since_epoch()).count();
	if (seconds <= 0) return 0;
	if (seconds > static_cast<long long>(std::numeric_limits<unsigned long>::max())) {
		return std::numeric_limits<unsigned long>::max() - 1;
	}
	return static_cast<unsigned long>(seconds);
}

void LoadOptionsClass::Fill_List(ListClass* list) {
	if (!list) return;

	namespace fs = std::filesystem;
	constexpr char kPrefix[] = "SAVEGAME.";

	Clear_List(list);

	if (Style == SAVE) {
		FileEntryClass* fdata = new FileEntryClass;
		std::memset(fdata, 0, sizeof(*fdata));
		std::strncpy(fdata->Descr, Text_String(TXT_EMPTY_SLOT), sizeof(fdata->Descr) - 1);
		fdata->Descr[sizeof(fdata->Descr) - 1] = '\0';
		fdata->DateTime = std::numeric_limits<unsigned long>::max();
		fdata->Valid = true;
		fdata->Num = 0;
		Files.Add(fdata);
	}

	std::error_code iter_ec;
	fs::directory_iterator it(fs::current_path(), iter_ec);
	if (iter_ec) {
		list->Flag_To_Redraw();
		return;
	}

	for (const auto& entry : it) {
		if (!entry.is_regular_file()) continue;
		const std::string name = entry.path().filename().string();
		if (!Starts_With_Case_Insensitive(name, kPrefix)) continue;

		const int id = Num_From_Ext(const_cast<char*>(name.c_str()));
		if (id < 0) continue;

		char descr[DESCRIP_MAX] = {0};
		unsigned scenario = 0;
		HousesType house = HOUSE_NONE;
		const bool ok = Get_Savefile_Info(id, descr, &scenario, &house);

		FileEntryClass* fdata = new FileEntryClass;
		std::memset(fdata, 0, sizeof(*fdata));
		fdata->Descr[0] = '\0';
		if (!ok) {
			std::strncpy(fdata->Descr, Text_String(TXT_OLD_GAME), sizeof(fdata->Descr) - 1);
			fdata->Descr[sizeof(fdata->Descr) - 1] = '\0';
		}

		std::strncat(fdata->Descr, descr, (sizeof(fdata->Descr) - std::strlen(fdata->Descr)) - 1);
		fdata->Valid = ok;
		fdata->Scenario = scenario;
		fdata->House = house;
		fdata->Num = id;

		std::error_code ts_ec;
		const auto ts = entry.last_write_time(ts_ec);
		fdata->DateTime = ts_ec ? 0 : File_Time_Seconds(ts);

		Files.Add(fdata);
	}

	if (Style == SAVE) {
		int id = -1;
		int i;
		for (i = 0; i < Files.Count(); i++) {
			id = -1;
			for (int j = 0; j < Files.Count(); j++) {
				if (Files[j]->Num == i) {
					id = j;
					break;
				}
			}
			if (id == -1) break;
		}
		Files[0]->Num = i;
	}

	std::qsort(&Files[0], Files.Count(), sizeof(FileEntryClass*), Compare);

	for (int i = 0; i < Files.Count(); i++) {
		list->Add_Item(Files[i]->Descr);
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
	if (a->DateTime > b->DateTime) return -1;
	if (a->DateTime < b->DateTime) return 1;
	return 0;
}
