#include "legacy/function.h"
#include "legacy/conquer.h"
#include "legacy/ipxmgr.h"
#include "legacy/phone.h"
#include "legacy/tcpip.h"

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>

static void Garble_Message(char *buf);

int Choose_Internet_Game(void);
int Get_Internet_Host_Or_Join(void);
int Get_IP_Address(void);
void Show_Internet_Connection_Progress(void);

namespace {
void StrUpr(char* buffer) {
	if (!buffer) return;
	for (; *buffer; ++buffer) {
		*buffer = static_cast<char>(std::toupper(static_cast<unsigned char>(*buffer)));
	}
}
}  // namespace

/***********************************************************************************************
 * Select_MPlayer_Game -- prompts user for NULL-Modem, Modem, or Network game                  *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      GAME_NORMAL, GAME_MODEM, etc.                                                          *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   02/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
GameType Select_MPlayer_Game(void)
{

	int factor = (SeenBuff.Get_Width() == 320) ? 1 : 2;
	bool ipx_avail = FALSE;
	int number_of_buttons;
	/*........................................................................
	Dialog & button dimensions
	........................................................................*/
	int d_dialog_w = 190 * factor;
	int d_dialog_h = 26 * 4 * factor;
	int d_dialog_x = ((320 * factor - d_dialog_w) / 2);
//	d_dialog_y = ((200 - d_dialog_h) / 2),
	int d_dialog_y = ((136 * factor - d_dialog_h) / 2);
	int d_dialog_cx = d_dialog_x + (d_dialog_w / 2);

	int d_txt6_h = 11 * factor;
	int d_margin = 7 * factor;

	int d_modemserial_w = 80 * factor;
	int d_modemserial_h = 9 * factor;
	int d_modemserial_x = d_dialog_cx - d_modemserial_w / 2;
	int d_modemserial_y = d_dialog_y + d_margin + d_txt6_h + d_margin;
#if (0)
	int d_internet_w = 80 * factor;
	int d_internet_h = 9 * factor;
	int d_internet_x = d_dialog_cx - d_internet_w / 2;
	int d_internet_y = d_modemserial_y + d_modemserial_h + 2 * factor;
#endif	//(0)
	int d_ipx_w = 80 * factor;
	int d_ipx_h = 9 * factor;
	int d_ipx_x = d_dialog_cx - d_ipx_w / 2;
	int d_ipx_y = d_modemserial_y + d_modemserial_h + 2 * factor;
//	int 	d_ipx_y = d_internet_y + d_internet_h + 2*factor;

	int d_cancel_w = 60 * factor;
	int d_cancel_h = 9 * factor;
	int d_cancel_x = d_dialog_cx - d_cancel_w / 2;
	int d_cancel_y = d_ipx_y + d_ipx_h + d_margin;

	CountDownTimerClass delay;

	/*........................................................................
	Button enumerations:
	........................................................................*/
	enum {
		BUTTON_MODEMSERIAL = 100,
#if (0)
		BUTTON_INTERNET,
#endif	//(0)
		BUTTON_IPX,
		BUTTON_CANCEL,

		NUM_OF_BUTTONS = 3,
	};
	number_of_buttons = NUM_OF_BUTTONS;
	/*........................................................................
	Redraw values: in order from "top" to "bottom" layer of the dialog
	........................................................................*/
	typedef enum {
		REDRAW_NONE = 0,
		REDRAW_BUTTONS,		// includes map interior & coord values
		REDRAW_BACKGROUND,	// includes box, map bord, key, coord labels, btns
		REDRAW_ALL = REDRAW_BACKGROUND
	} RedrawType;
	/*........................................................................
	Dialog variables:
	........................................................................*/
	KeyNumType input;								// input from user
	bool process;									// loop while true
	RedrawType display;							// true = re-draw everything
	GameType retval;								// return value
	int selection;
	bool pressed;
	int curbutton;
	TextButtonClass *buttons[NUM_OF_BUTTONS];

	/*........................................................................
	Buttons
	........................................................................*/
	ControlClass *commands = NULL;				// the button list

	//
	// If neither IPX or winsock are active then do only the modem serial dialog
	//
	if (Ipx.Is_IPX()) {
		ipx_avail = TRUE;
	}


	TextButtonClass modemserialbtn(BUTTON_MODEMSERIAL, TXT_MODEM_SERIAL,
		TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
		d_modemserial_x, d_modemserial_y, d_modemserial_w, d_modemserial_h);
#if (0)
	TextButtonClass internetbtn (BUTTON_INTERNET, TXT_INTERNET,
		TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
		d_internet_x, d_internet_y, d_internet_w, d_internet_h);
#endif	//(0)
	TextButtonClass ipxbtn(BUTTON_IPX, TXT_NETWORK,
		TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
		d_ipx_x, d_ipx_y, d_ipx_w, d_ipx_h);

	TextButtonClass cancelbtn(BUTTON_CANCEL, TXT_CANCEL,
		TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
		d_cancel_x, d_cancel_y, d_cancel_w, d_cancel_h);

	/*
	------------------------------- Initialize -------------------------------
	*/
	Set_Logic_Page(SeenBuff);

	/*
	............................ Create the list .............................
	*/
	commands = &modemserialbtn;
#if (0)
	internetbtn.Add_Tail(*commands);
#endif	//(0)
	if (ipx_avail) {
		ipxbtn.Add_Tail(*commands);
	}
	cancelbtn.Add_Tail(*commands);

	/*
	......................... Fill array of button ptrs ......................
	*/
	curbutton = 0;
	buttons[0] = &modemserialbtn;
#if (0)
	buttons[1] = &internetbtn;
#endif	//(0)
	if (ipx_avail) {
		buttons[1] = &ipxbtn;
		buttons[2] = &cancelbtn;
	}
	else {
		buttons[1] = &cancelbtn;
		number_of_buttons--;
	}

	buttons[curbutton]->Turn_On();

	Keyboard::Clear();

	Fancy_Text_Print(TXT_NONE, 0, 0, CC_GREEN, TBLACK,
		TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);

	/*
	-------------------------- Main Processing Loop --------------------------
	*/
	display = REDRAW_ALL;

	process = true;
	pressed = false;
	selection = -1;
	retval = GAME_NORMAL;

	delay.Set(0);

	while (process) {

		/*
		.................................. Redraw .................................
		*/
		if (display) {
			Hide_Mouse();
			if (display >= REDRAW_BACKGROUND) {
				Dialog_Box(d_dialog_x, d_dialog_y, d_dialog_w, d_dialog_h);
				Fancy_Text_Print(TXT_SELECT_MPLAYER_GAME, d_dialog_cx, d_dialog_y + d_margin,
					CC_GREEN, TBLACK, TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
			}
			if (display >= REDRAW_BUTTONS) {
				commands->Flag_List_To_Redraw();
			}
			Show_Mouse();
			display = REDRAW_NONE;
		}

		/*
		............................. Process Commands ............................
		*/
		input = commands->Input();
		if (input) {
			selection = input;
			pressed = true;
		}

		/*
		............................ Process Keyboard .............................
		*/
		input = Keyboard::Get();

		switch (input) {

		case KN_ESC:
			selection = BUTTON_CANCEL;
			pressed = true;
			break;

		case KN_UP:
			buttons[curbutton]->Turn_Off();
			buttons[curbutton]->Flag_To_Redraw();
			curbutton--;
			if (curbutton < 0)
				curbutton = (number_of_buttons - 1);
			buttons[curbutton]->Turn_On();
			buttons[curbutton]->Flag_To_Redraw();
			break;

		case KN_DOWN:
			buttons[curbutton]->Turn_Off();
			buttons[curbutton]->Flag_To_Redraw();
			curbutton++;
			if (curbutton > (number_of_buttons - 1))
				curbutton = 0;
			buttons[curbutton]->Turn_On();
			buttons[curbutton]->Flag_To_Redraw();
			break;

		case KN_RETURN:
			selection = curbutton + BUTTON_MODEMSERIAL;
			if (!ipx_avail) selection--;
			pressed = true;
			break;

		default:
			break;
		}

		if (pressed) {
			//
			// to make sure the selection is correct in case they used the mouse
			//
			buttons[curbutton]->Turn_Off();
			buttons[curbutton]->Flag_To_Redraw();
			curbutton = selection - BUTTON_MODEMSERIAL;
			buttons[curbutton]->Turn_On();
//			buttons[curbutton]->Flag_To_Redraw();
			buttons[curbutton]->IsPressed = true;
			buttons[curbutton]->Draw_Me(true);

			switch (selection) {
			case (BUTTON_MODEMSERIAL):

				//
				// Pop up the modem/serial/com port dialog
				//
				retval = Select_Serial_Dialog();

				if (retval != GAME_NORMAL) {
					process = false;
				}
				else {
					buttons[curbutton]->IsPressed = false;
					display = REDRAW_ALL;
				}
				break;

#if (0)
			case (BUTTON_INTERNET):
// #define ONLY_FOR_E3
#ifdef ONLY_FOR_E3
				Call_Back();
				Show_Internet_Connection_Progress();		//changed to do nothing
				Hide_Mouse();
				Load_Title_Screen("HTITLE.PCX", &HidPage, Palette);
				HidPage.Blit(SeenBuff);
				Show_Mouse();
				Call_Back();
				CCMessageBox().Process("Error! - Unable to ping KANE.WESTWOOD.COM");

				buttons[curbutton]->IsPressed = false;
				display = REDRAW_ALL;
#endif	//ONLY_FOR_E3


#ifdef FORCE_WINSOCK
				if (Winsock.Init()) {
					Read_MultiPlayer_Settings();
					int result = Choose_Internet_Game();

					if (!result) {
						Winsock.Close();
						buttons[curbutton]->IsPressed = false;
						display = REDRAW_ALL;
						break;
					}

					result = Get_Internet_Host_Or_Join();
					if (result == 1) {
						Winsock.Close();
						buttons[curbutton]->IsPressed = false;
						display = REDRAW_ALL;
						break;
					}
					Server = !result;

					if (Server) {
#if (0)
						ModemGameToPlay = INTERNET_HOST;
						Winsock.Start_Server();
#else
						result = Get_IP_Address();
						if (!result) {
							Winsock.Close();
							buttons[curbutton]->IsPressed = false;
							display = REDRAW_ALL;
							break;
						}
						Winsock.Set_Host_Address(PlanetWestwoodIPAddress);
						Winsock.Start_Server();
#endif

					}
					else {
						ModemGameToPlay = INTERNET_JOIN;
						result = Get_IP_Address();
						if (!result) {
							Winsock.Close();
							buttons[curbutton]->IsPressed = false;
							display = REDRAW_ALL;
							break;
						}
						Winsock.Set_Host_Address(PlanetWestwoodIPAddress);
						Winsock.Start_Client();
					}

					//CountDownTimerClass connect_timeout;
					//connect_timeout.Set(15*60);

					////Show_Internet_Connection_Progress();

					if (!Winsock.Get_Connected()) {
						Winsock.Close();
						return(GAME_NORMAL);
					}

					SerialSettingsType *settings;
					settings = &SerialDefaults;
					Init_Null_Modem(settings);
					if (Server) {
						if (Com_Scenario_Dialog()) {
							return (GAME_INTERNET);
						}
						else {
							Winsock.Close();
							return (GAME_NORMAL);
						}
					}
					else {
						if (Com_Show_Scenario_Dialog()) {
							return (GAME_INTERNET);
						}
						else {
							Winsock.Close();
							return (GAME_NORMAL);
						}
					}
				}
#endif //FORCE_WINSOCK
				break;

#endif	//(0)

			case (BUTTON_IPX):
				retval = GAME_IPX;
				process = false;
				break;

			case (BUTTON_CANCEL):
				retval = GAME_NORMAL;
				process = false;
				break;
			}

			pressed = false;
		}

		Call_Back();
		delay.Set(10);
		while (!delay.Expired()) {
			Call_Back();
		}
	}

	return(retval);
}


/***********************************************************************************************
 * Read_MultiPlayer_Settings -- reads multi-player settings from conquer.ini                    *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
void Read_MultiPlayer_Settings (void)
{
	char *buffer;							// INI staging buffer pointer.
	char *tbuffer;							// Accumulation buffer of trigger IDs.
	int len;									// Length of data in buffer.
	char *tokenptr;						// ptr to token
	PhoneEntryClass *phone;				// a phone book entry
	char *entry;							// a phone book entry
	char buf[128];							// buffer for parsing INI entry
	int i;
	CELL cell;

	/*------------------------------------------------------------------------
	Fetch working pointer to the INI staging buffer. Make sure that the buffer
	is cleared out before proceeding.  (Don't use the HidPage for this, since
	the HidPage may be needed for various uncompressions during the INI
	parsing.)
	------------------------------------------------------------------------*/
	buffer = (char *)_ShapeBuffer;
	memset(buffer, '\0', _ShapeBufferSize);

	/*------------------------------------------------------------------------
	Clear the initstring entries
	------------------------------------------------------------------------*/
	for (i = 0; i < InitStrings.Count(); i++) {
		delete[] InitStrings[i];
	}
	InitStrings.Clear();

	/*------------------------------------------------------------------------
	Clear the dialing entries
	------------------------------------------------------------------------*/
	for (i = 0; i < PhoneBook.Count(); i++) {
		delete PhoneBook[i];
	}
	PhoneBook.Clear();

	/*------------------------------------------------------------------------
	Create filename and read the file.
	------------------------------------------------------------------------*/
	CCFileClass file ("CONQUER.INI");
	if (!file.Is_Available()) {
		return;
	} else {
		file.Read(buffer, _ShapeBufferSize-1);
	}
	file.Close();

	if (!Special.IsFromWChat){
		/*------------------------------------------------------------------------
		Get the player's last-used Handle
		------------------------------------------------------------------------*/
		WWGetPrivateProfileString("MultiPlayer", "Handle", "Noname", MPlayerName,
			sizeof(MPlayerName), buffer);

		/*------------------------------------------------------------------------
		Get the player's last-used Color
		------------------------------------------------------------------------*/
		MPlayerPrefColor = WWGetPrivateProfileInt("MultiPlayer", "Color", 0, buffer);
		MPlayerHouse = (HousesType)WWGetPrivateProfileInt("MultiPlayer", "Side",
			HOUSE_GOOD, buffer);
		CurPhoneIdx = WWGetPrivateProfileInt("MultiPlayer", "PhoneIndex", -1, buffer);
	}else{
		CurPhoneIdx = -1;
	}

#if 1
	TrapCheckHeap = WWGetPrivateProfileInt( "MultiPlayer", "CheckHeap", 0, buffer);
#endif

	/*------------------------------------------------------------------------
	Read in default serial settings
	------------------------------------------------------------------------*/
	WWGetPrivateProfileString ("SerialDefaults", "ModemName", "NoName", SerialDefaults.ModemName, MODEM_NAME_MAX, buffer);
	if (!strcmp ( SerialDefaults.ModemName, "NoName")) {
		SerialDefaults.ModemName[0] = 0;
	}
	WWGetPrivateProfileString ("SerialDefaults", "Port", "0", buf, 5, buffer);
	sscanf (buf, "%x", &SerialDefaults.Port);
	SerialDefaults.IRQ = WWGetPrivateProfileInt("SerialDefaults", "IRQ", -1, buffer);
	SerialDefaults.Baud = WWGetPrivateProfileInt("SerialDefaults", "Baud", -1, buffer);
	SerialDefaults.Init = WWGetPrivateProfileInt("SerialDefaults", "Init", 0, buffer);
	SerialDefaults.Compression = WWGetPrivateProfileInt ("SerialDefaults", "Compression", 0, buffer);
	SerialDefaults.ErrorCorrection = WWGetPrivateProfileInt ("SerialDefaults", "ErrorCorrection", 0, buffer);
	SerialDefaults.HardwareFlowControl = WWGetPrivateProfileInt ("SerialDefaults", "HardwareFlowControl", 1, buffer);

	WWGetPrivateProfileString( "SerialDefaults", "DialMethod", DialMethodCheck[ DIAL_TOUCH_TONE ], buf, 128, buffer );

	// check validity

	for (i = 0; i < DIAL_METHODS; i++ ) {
		if (!stricmp( buf, DialMethodCheck[ i ]) ) {
			SerialDefaults.DialMethod = (DialMethodType)i;
			break;
		}
	}

	// if method not found set to touch tone

	if (i == DIAL_METHODS) {
		SerialDefaults.DialMethod = DIAL_TOUCH_TONE;
	}

	SerialDefaults.InitStringIndex =
											WWGetPrivateProfileInt("SerialDefaults",
												"InitStringIndex", 0, buffer);

	SerialDefaults.CallWaitStringIndex =
											WWGetPrivateProfileInt("SerialDefaults",
												"CallWaitStringIndex", CALL_WAIT_CUSTOM,
												buffer);

	WWGetPrivateProfileString ("SerialDefaults", "CallWaitString", "",
		SerialDefaults.CallWaitString, CWAITSTRBUF_MAX, buffer);

	if (SerialDefaults.IRQ == 0 ||
		SerialDefaults.Baud == 0) {

		SerialDefaults.Port = 0;
		SerialDefaults.IRQ = -1;
		SerialDefaults.Baud = -1;
	}

	/*------------------------------------------------------------------------
	Set 'tbuffer' to point past the actual INI data
	------------------------------------------------------------------------*/
	len = strlen(buffer) + 2;
	tbuffer = buffer + len;

	/*------------------------------------------------------------------------
	Read all Base-Scenario names into 'tbuffer'
	------------------------------------------------------------------------*/
	WWGetPrivateProfileString("InitStrings", NULL, NULL, tbuffer,
		ShapeBufferSize-len, buffer);

	/*------------------------------------------------------------------------
	Read in & store each entry
	------------------------------------------------------------------------*/
	while (*tbuffer != '\0') {
		entry = new char[ INITSTRBUF_MAX ];

		entry[0] = 0;

		WWGetPrivateProfileString("InitStrings", tbuffer, NULL, entry,
			INITSTRBUF_MAX, buffer);

		StrUpr( entry );

		InitStrings.Add( entry );

		tbuffer += strlen(tbuffer) + 1;
	}

	// if no entries then have at least one

	if ( tbuffer == (buffer + len) ) {
		entry = new char[ INITSTRBUF_MAX ];
		strcpy( entry, "ATZ" );
		InitStrings.Add( entry );
		SerialDefaults.InitStringIndex = 0;
	} else {
		len = strlen(buffer) + 2;
	}

	/*------------------------------------------------------------------------
	Repeat the process for the phonebook
	------------------------------------------------------------------------*/
	tbuffer = buffer + len;

	/*------------------------------------------------------------------------
	Read in all phone book listings.
	Format: Name=PhoneNum,Port,IRQ,Baud,InitString
	------------------------------------------------------------------------*/

	/*........................................................................
	Read the entry names in
	........................................................................*/
	WWGetPrivateProfileString("PhoneBook", NULL, NULL, tbuffer,
		ShapeBufferSize-len, buffer);

	while (*tbuffer != '\0') {
		/*.....................................................................
		Create a new phone book entry
		.....................................................................*/
		phone = new PhoneEntryClass();

		/*.....................................................................
		Read the entire entry in
		.....................................................................*/
		WWGetPrivateProfileString("PhoneBook", tbuffer, NULL, buf, 128, buffer);

		/*.....................................................................
		Extract name, phone # & serial port settings
		.....................................................................*/
		tokenptr = strtok( buf, "|" );
		if (tokenptr) {
			strcpy( phone->Name, tokenptr );
			StrUpr( phone->Name );
		} else {
			phone->Name[0] = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			strcpy( phone->Number, tokenptr );
			StrUpr( phone->Number );
		} else {
			phone->Number[0] = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			sscanf( tokenptr, "%x", &phone->Settings.Port );
		} else {
			phone->Settings.Port = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.IRQ = atoi( tokenptr );
		} else {
			phone->Settings.IRQ = -1;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.Baud = atoi( tokenptr );
		} else {
			phone->Settings.Baud = -1;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.Compression = atoi( tokenptr );
		} else {
			phone->Settings.Compression = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.ErrorCorrection = atoi( tokenptr );
		} else {
			phone->Settings.ErrorCorrection = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.HardwareFlowControl = atoi( tokenptr );
		} else {
			phone->Settings.HardwareFlowControl = 1;
		}


		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			strcpy( buf, tokenptr );

			// find dial method

			for (i = 0; i < DIAL_METHODS; i++) {
				if ( !stricmp( buf, DialMethodCheck[ i ]) ) {
					phone->Settings.DialMethod = (DialMethodType)i;
					break;
				}
			}

			// if method not found set to touch tone

			if (i == DIAL_METHODS) {
				phone->Settings.DialMethod = DIAL_TOUCH_TONE;
			}
		} else {
			phone->Settings.DialMethod = DIAL_TOUCH_TONE;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.InitStringIndex = atoi( tokenptr );
		} else {
			phone->Settings.InitStringIndex = 0;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			phone->Settings.CallWaitStringIndex = atoi( tokenptr );
		} else {
			phone->Settings.CallWaitStringIndex = CALL_WAIT_CUSTOM;
		}

		tokenptr = strtok( NULL, "|" );
		if (tokenptr) {
			strcpy (phone->Settings.CallWaitString, tokenptr);
		} else {
			phone->Settings.CallWaitString[0] = 0;
		}

		/*.....................................................................
		Add it to our list
		.....................................................................*/
		PhoneBook.Add(phone);

		tbuffer += strlen(tbuffer) + 1;
	}

	/*------------------------------------------------------------------------
	Read special recording playback values, to help find sync bugs
	------------------------------------------------------------------------*/
	if (PlaybackGame) {
		TrapFrame = WWGetPrivateProfileInt ("SyncBug","Frame",0x7fffffff, buffer);

		TrapObjType = (RTTIType)WWGetPrivateProfileInt ("SyncBug","Type",RTTI_NONE, buffer);
		WWGetPrivateProfileString ("SyncBug","Type","NONE",buf,80,buffer);
		if (!stricmp(buf,"AIRCRAFT"))
			TrapObjType = RTTI_AIRCRAFT;
		else if (!stricmp(buf,"ANIM"))
			TrapObjType = RTTI_ANIM;
		else if (!stricmp(buf,"BUILDING"))
			TrapObjType = RTTI_BUILDING;
		else if (!stricmp(buf,"BULLET"))
			TrapObjType = RTTI_BULLET;
		else if (!stricmp(buf,"INFANTRY"))
			TrapObjType = RTTI_INFANTRY;
		else if (!stricmp(buf,"UNIT"))
			TrapObjType = RTTI_UNIT;
		else {
			TrapObjType = RTTI_NONE;
		}

		WWGetPrivateProfileString ("SyncBug","Coord","0",buf,80,buffer);
		sscanf(buf,"%x",&TrapCoord);

		WWGetPrivateProfileString ("SyncBug","this","0",buf,80,buffer);
		sscanf(buf,"%x",&TrapThis);

		WWGetPrivateProfileString ("SyncBug","Cell","0",buf,80,buffer);
		cell = atoi(buf);
		if (cell) {
			TrapCell = &(Map[cell]);
		}
	}
}


/***********************************************************************************************
 * Write_MultiPlayer_Settings -- writes multi-player settings to conquer.ini                   *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
void Write_MultiPlayer_Settings (void)
{
	char * buffer;			// INI staging buffer pointer.
	CCFileClass file;
	int i;
	char entrytext[4];
	char buf[128];							// buffer for parsing INI entry

	/*------------------------------------------------------------------------
	Get a working pointer to the INI staging buffer. Make sure that the buffer
	starts cleared out of any data.
	------------------------------------------------------------------------*/
	buffer = (char *)_ShapeBuffer;
	memset(buffer, '\0', _ShapeBufferSize);

	file.Set_Name("CONQUER.INI");
	if (file.Is_Available()) {
		file.Open(READ);
		file.Read(buffer, _ShapeBufferSize-1);
		file.Close();
	}

	/*------------------------------------------------------------------------
	Save the player's last-used Handle & Color
	------------------------------------------------------------------------*/
	WWWritePrivateProfileInt("MultiPlayer", "PhoneIndex", CurPhoneIdx, buffer);
	WWWritePrivateProfileInt ("MultiPlayer", "Color", MPlayerPrefColor, buffer);
	WWWritePrivateProfileInt ("MultiPlayer", "Side", MPlayerHouse, buffer);
	WWWritePrivateProfileString("MultiPlayer", "Handle", MPlayerName, buffer);

	/*------------------------------------------------------------------------
	Clear all existing SerialDefault entries.
	------------------------------------------------------------------------*/
	WWWritePrivateProfileString ("SerialDefaults", NULL, NULL, buffer);

	/*------------------------------------------------------------------------
	Save default serial settings
	------------------------------------------------------------------------*/
	WWWritePrivateProfileString ("SerialDefaults", "ModemName", SerialDefaults.ModemName, buffer);
	sprintf (buf, "%x", SerialDefaults.Port);
	WWWritePrivateProfileString ("SerialDefaults", "Port", buf, buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "IRQ", SerialDefaults.IRQ, buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "Baud", SerialDefaults.Baud, buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "Init", SerialDefaults.Init, buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "Compression", SerialDefaults.Compression, buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "ErrorCorrection", SerialDefaults.ErrorCorrection, buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "HardwareFlowControl", SerialDefaults.HardwareFlowControl, buffer);
	WWWritePrivateProfileString ("SerialDefaults", "DialMethod", DialMethodCheck[ SerialDefaults.DialMethod ], buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "InitStringIndex", SerialDefaults.InitStringIndex, buffer);
	WWWritePrivateProfileInt ("SerialDefaults", "CallWaitStringIndex", SerialDefaults.CallWaitStringIndex, buffer);
	WWWritePrivateProfileString ("SerialDefaults", "CallWaitString", SerialDefaults.CallWaitString, buffer);



	/*------------------------------------------------------------------------
	Clear all existing InitStrings entries.
	------------------------------------------------------------------------*/
	WWWritePrivateProfileString ("InitStrings", NULL, NULL, buffer);

	/*------------------------------------------------------------------------
	Save each initstring entry
	------------------------------------------------------------------------*/
	for (i = (InitStrings.Count() - 1); i >= 0; i--) {
		sprintf(entrytext, "%d", i);
		WWWritePrivateProfileString ("InitStrings", entrytext, InitStrings[ i ], buffer);
	}


	/*------------------------------------------------------------------------
	Clear all existing phonebook entries.
	------------------------------------------------------------------------*/
	WWWritePrivateProfileString ("PhoneBook", NULL, NULL, buffer);

	/*------------------------------------------------------------------------
	Save each phonebook entry
	------------------------------------------------------------------------*/
	for (i = (PhoneBook.Count() - 1); i >= 0; i--) {

		sprintf(entrytext, "%d", i);

		// format:
		// "Name|Number|Port|IRQ|Baud|Compression|ErrorCorrection|HardwareFlowControl|DialMethod|InitStringIndex|CallWaitStringIndex|CallWaitString"

		sprintf( buf, "%s|%s|%x|%d|%d|%d|%d|%d|%s|%d|%d|%s",
			PhoneBook[i]->Name,
			PhoneBook[i]->Number,
			PhoneBook[i]->Settings.Port,
			PhoneBook[i]->Settings.IRQ,
			PhoneBook[i]->Settings.Baud,
			PhoneBook[i]->Settings.Compression,
			PhoneBook[i]->Settings.ErrorCorrection,
			PhoneBook[i]->Settings.HardwareFlowControl,
			DialMethodCheck[ PhoneBook[i]->Settings.DialMethod ],
			PhoneBook[i]->Settings.InitStringIndex,
			PhoneBook[i]->Settings.CallWaitStringIndex,
			PhoneBook[i]->Settings.CallWaitString);

		WWWritePrivateProfileString ("PhoneBook", entrytext, buf, buffer);
	}

	/*------------------------------------------------------------------------
	Write the INI data out to the file.
	------------------------------------------------------------------------*/
	file.Open(WRITE);
	file.Write(buffer,strlen(buffer));
	file.Close();
}


/***********************************************************************************************
 * Read_Scenario_Descriptions -- reads multi-player scenario #'s # descriptions                *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
void Read_Scenario_Descriptions (void)
{
	char *buffer;							// INI staging buffer pointer.
	int i;

	/*------------------------------------------------------------------------
	Fetch working pointer to the INI staging buffer. Make sure that the buffer
	is cleared out before proceeding.  (Don't use the HidPage for this, since
	the HidPage may be needed for various uncompressions during the INI
	parsing.)
	------------------------------------------------------------------------*/
	buffer = (char *)_ShapeBuffer;
	memset(buffer, '\0', _ShapeBufferSize);

	/*------------------------------------------------------------------------
	Create filename and read the file.
	------------------------------------------------------------------------*/
	CCFileClass file ("CONQUER.INI");
	if (!file.Is_Available()) {
		return;
	} else {
		file.Read(buffer, _ShapeBufferSize-1);
	}
	file.Close();

	/*------------------------------------------------------------------------
	Read scenario descriptions
	------------------------------------------------------------------------*/
	WWGetPrivateProfileString ("Scenarios", "NoDesc0", "No Description", MPlayerDescriptions[0], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[0]));
	WWGetPrivateProfileString ("Scenarios", "NoDesc1", "No Description", MPlayerDescriptions[1], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[1]));
	WWGetPrivateProfileString ("Scenarios", "NoDesc2", "No Description", MPlayerDescriptions[2], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[2]));
	WWGetPrivateProfileString ("Scenarios", "NoDesc3", "No Description", MPlayerDescriptions[3], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[3]));
	WWGetPrivateProfileString ("Scenarios", "NoDesc4", "No Description", MPlayerDescriptions[4], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[4]));

	WWGetPrivateProfileString ("Scenarios", "BaseDesc0", "No Description", MPlayerDescriptions[5], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[5]));
	WWGetPrivateProfileString ("Scenarios", "BaseDesc1", "No Description", MPlayerDescriptions[6], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[6]));
	WWGetPrivateProfileString ("Scenarios", "BaseDesc2", "No Description", MPlayerDescriptions[7], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[7]));
	WWGetPrivateProfileString ("Scenarios", "BaseDesc3", "No Description", MPlayerDescriptions[8], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[8]));
	WWGetPrivateProfileString ("Scenarios", "BaseDesc4", "No Description", MPlayerDescriptions[9], 40, buffer);
	MPlayerFilenum.Add(atoi(MPlayerDescriptions[9]));

	for (i=0; i < 10; i++) {
		if (atoi(MPlayerDescriptions[i])==0){
			MPlayerDescriptions[i][0]='\0';
		}
		else{
			memmove(MPlayerDescriptions[i],MPlayerDescriptions[i]+2,strlen(MPlayerDescriptions[i]));
			MPlayerFilenum[i]=atoi(MPlayerDescriptions[i]-2);
		}
	}
}


/***********************************************************************************************
 * Free_Scenario_Descriptions -- frees memory for the scenario descriptions                    *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
void Free_Scenario_Descriptions(void)
{
	int i;

	for (i = 0; i < PhoneBook.Count(); i++) {
		delete PhoneBook[i];
	}
	PhoneBook.Clear();

	for (i = 0; i < InitStrings.Count(); i++) {
		delete[] InitStrings[i];
	}
	InitStrings.Clear();
}


/***********************************************************************************************
 * Computer_Message -- "sends" a message from the computer                                     *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      buf      - message to send                                                             *
 *      delay    - how long to wait                                                            *
 *      cancel   - cancel key                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
void Computer_Message (void)
{
	char buf[128];							// string buffer
	int i;

	/*------------------------------------------------------------------------
	Clear the buffer
	------------------------------------------------------------------------*/
	memset (buf, 0, 128);

	/*------------------------------------------------------------------------
	Extract the message. The original Win32 build used a range of localized
	strings here; for the port, preserve the "garbled computer" behavior with
	a fixed set of short phrases.
	------------------------------------------------------------------------*/
	static char const* const kMessages[] = {
		"YOU WILL LOSE",
		"INCOMING",
		"WARNING",
		"YOUR BASE IS UNDER ATTACK",
		"YOU HAVE LOST",
		"YOU HAVE WON",
		"INSUFFICIENT FUNDS",
		"UNIT READY",
		"BUILDING",
		"REINFORCEMENTS",
		"OBJECTIVE",
		"MISSION",
	};
	const int max_index = static_cast<int>((sizeof(kMessages) / sizeof(kMessages[0])) - 1);
	strcpy(buf, kMessages[Random_Pick(0, max_index)]);

	/*------------------------------------------------------------------------
	Garble it; 1st 2 chars are always preserved, to let it pass as a legal packet
	------------------------------------------------------------------------*/
	for (i = 0; i < 2; i++) {
		buf[i] = buf[i];
	}
	Garble_Message (buf+2);

	/*------------------------------------------------------------------------
	Save the message.
	------------------------------------------------------------------------*/
	strcpy (LastMessage, buf);
}


/***********************************************************************************************
 * Garble_Message -- "garbles" a message                                                       *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      buf      - message to garble                                                           *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      none.                                                                                  *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   07/05/1995 BR : Created.                                                                  *
 *=============================================================================================*/
static void Garble_Message(char *buf)
{
	int i;

	/*------------------------------------------------------------------------
	Garble by changing every character to 'x', preserving spaces
	------------------------------------------------------------------------*/
	for (i = 0; buf[i]; i++) {
		if (buf[i] == ' ')
			continue;
		buf[i] = 'x';
	}
}


/***********************************************************************************************
 * Surrender_Dialog -- Prompts user for surrendering                                           *
 *                                                                                             *
 * INPUT:                                                                                      *
 *      none.                                                                                  *
 *                                                                                             *
 * OUTPUT:                                                                                     *
 *      true = user surrendered, false = user cancelled                                        *
 *                                                                                             *
 * WARNINGS:                                                                                   *
 *      none.                                                                                  *
 *                                                                                             *
 * HISTORY:                                                                                    *
 *   04/14/1995 BR : Created.                                                                  *
 *=============================================================================================*/
int Surrender_Dialog(void)
{
	int factor			= (SeenBuff.Get_Width() == 320) ? 1 : 2;
	/*........................................................................
	Dialog & button dimensions
	........................................................................*/
	int d_dialog_w = 170*factor;											// dialog width
	int d_dialog_h = 53*factor;											// dialog height
	int d_dialog_x = ((320*factor - d_dialog_w) / 2);				// centered x-coord
	int d_dialog_y = ((200*factor - d_dialog_h) / 2);				// centered y-coord
	int d_dialog_cx = d_dialog_x + (d_dialog_w / 2);		// coord of x-center

	int d_txt6_h = 6*factor+1;												// ht of 6-pt text
	int d_margin = 5*factor;												// margin width/height
	int d_topmargin = 20*factor;											// top margin

	int d_ok_w = 45*factor;												// ok width
	int d_ok_h = 9*factor;													// ok height
	int d_ok_x = d_dialog_cx - d_ok_w - 5*factor;					// ok x
	int d_ok_y = d_dialog_y + d_dialog_h - d_ok_h - d_margin;	// ok y

	int d_cancel_w = 45*factor;											// cancel width
	int d_cancel_h = 9*factor;											// cancel height
	int d_cancel_x = d_dialog_cx + 5*factor;							// cancel x
	int d_cancel_y = d_dialog_y + d_dialog_h - d_cancel_h - d_margin; // cancel y


	/*........................................................................
	Button enumerations
	........................................................................*/
	enum {
		BUTTON_OK = 100,
		BUTTON_CANCEL,
	};

	/*........................................................................
	Redraw values: in order from "top" to "bottom" layer of the dialog
	........................................................................*/
	typedef enum {
		REDRAW_NONE = 0,
		REDRAW_BUTTONS,
		REDRAW_BACKGROUND,
		REDRAW_ALL = REDRAW_BACKGROUND
	} RedrawType;

	/*........................................................................
	Dialog variables
	........................................................................*/
	RedrawType display;							// requested redraw level
	bool process;									// loop while true
	KeyNumType input;
	int retcode;

	/*........................................................................
	Buttons
	........................................................................*/
	ControlClass *commands = NULL;		// the button list

	TextButtonClass okbtn (BUTTON_OK, TXT_OK,
		TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
		d_ok_x, d_ok_y, d_ok_w, d_ok_h);

	TextButtonClass cancelbtn (BUTTON_CANCEL, TXT_CANCEL,
		TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW,
		d_cancel_x, d_cancel_y, d_cancel_w, d_cancel_h);

	/*
	------------------------------- Initialize -------------------------------
	*/
	Set_Logic_Page(SeenBuff);

	/*
	......................... Create the button list .........................
	*/
	commands = &okbtn;
	cancelbtn.Add_Tail(*commands);

	/*
	-------------------------- Main Processing Loop --------------------------
	*/
	display = REDRAW_ALL;
	process = true;
	while (process) {

		/*
		** If we have just received input focus again after running in the background then
		** we need to redraw.
		*/
		if (AllSurfaces.SurfacesRestored){
			AllSurfaces.SurfacesRestored=FALSE;
			display=REDRAW_ALL;
		}

		/*
		........................ Invoke game callback .........................
		*/
		if (Main_Loop()) {
			retcode = 0;
			process = false;
		}

		/*
		...................... Refresh display if needed ......................
		*/
		if (display) {

			/*
			...................... Display the dialog box ......................
			*/
			Hide_Mouse();
			if (display >= REDRAW_BACKGROUND) {
				Dialog_Box(d_dialog_x, d_dialog_y, d_dialog_w, d_dialog_h);
				Draw_Caption(TXT_NONE, d_dialog_x, d_dialog_y, d_dialog_w);

				/*
				....................... Draw the captions .......................
				*/
				Fancy_Text_Print(Text_String(TXT_SURRENDER),
					d_dialog_cx, d_dialog_y + d_topmargin,
					CC_GREEN, TBLACK,
					TPF_CENTER | TPF_6PT_GRAD | TPF_USE_GRAD_PAL | TPF_NOSHADOW);
			}

			/*
			........................ Redraw the buttons ........................
			*/
			if (display >= REDRAW_BUTTONS) {
				commands->Flag_List_To_Redraw();
			}
			Show_Mouse();
			display = REDRAW_NONE;
		}

		/*
		........................... Get user input ............................
		*/
		input = commands->Input();

		/*
		............................ Process input ............................
		*/
		switch (input) {
			case (KN_RETURN):
			case (BUTTON_OK | KN_BUTTON):
				retcode = 1;
				process = false;
				break;

			case (KN_ESC):
			case (BUTTON_CANCEL | KN_BUTTON):
				retcode = 0;
				process = false;
				break;

			default:
				break;
		}
	}

	/*
	--------------------------- Redraw the display ---------------------------
	*/
	HiddenPage.Clear();
	Map.Flag_To_Redraw(true);
	Map.Render();

	return (retcode);
}
