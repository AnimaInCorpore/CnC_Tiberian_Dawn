/* Portable DDE server shim for the ported build */
#pragma once

#include <cstdint>

// This header replaces the legacy Windows DDE glue with a minimal,
// cross-platform stub implementation that preserves the original
// public API surface used by the rest of the port.

class DDEServerClass {
public:
  DDEServerClass();
  ~DDEServerClass();

  char *Get_MPlayer_Game_Info();
  int  Get_MPlayer_Game_Info_Length() const { return MPlayerGameInfoLength; }
  bool Callback(unsigned char *data, long length);
  void Delete_MPlayer_Game_Info();
  void Enable();
  void Disable();
  int  Time_Since_Heartbeat() const;

  enum {
    DDE_PACKET_START_MPLAYER_GAME,
    DDE_PACKET_GAME_RESULTS,
    DDE_PACKET_HEART_BEAT,
    DDE_TICKLE,
    DDE_CONNECTION_FAILED
  };

private:
  char *MPlayerGameInfo = nullptr;
  int   MPlayerGameInfoLength = 0;
  bool  IsEnabled = false;
  int   LastHeartbeat = 0;
};

extern DDEServerClass DDEServer;
extern bool Send_Data_To_DDE_Server(const char *data, int length, int packet_type);

/***********************************************************************************************
 ***              C O N F I D E N T I A L  ---  W E S T W O O D  S T U D I O S               ***
 ***********************************************************************************************
 *                                                                                             *
 *                 Project Name : Command & Conquer - Red Alert                                *
 *                                                                                             *
 *                    File Name : CCDDE.H                                                      *
 *                                                                                             *
 *                   Programmer : Steve Tall                                                   *
 *                                                                                             *
 *                   Start Date : 10/04/95                                                     *
 *                                                                                             *
 *                  Last Update : August 5th, 1996 [ST]                                        *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 * Overview:                                                                                   *
 *   C&C's interface to the DDE class                                                          *
 *                                                                                             *
 *---------------------------------------------------------------------------------------------*
 *                                                                                             *
 * Functions:                                                                                  *
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#include "compat.h"

#if 0 // TD_PORT_USE_SDL2 (legacy DDE code disabled in port)
// SDL2 port stub: DDE integration is not implemented.
class DDEServerClass {
 public:
  DDEServerClass() = default;
  ~DDEServerClass() = default;

  char* Get_MPlayer_Game_Info() { return nullptr; }
  int Get_MPlayer_Game_Info_Length() { return 0; }
  BOOL Callback(unsigned char*, long) { return FALSE; }
  void Delete_MPlayer_Game_Info() {}
  void Enable() {}
  void Disable() {}
  int Time_Since_Heartbeat() { return 0; }
};

extern DDEServerClass DDEServer;

#elif defined(WIN32)

#include "dde.h"

class DDEServerClass {

	public:

		DDEServerClass (void);
		~DDEServerClass (void);


		char 	*Get_MPlayer_Game_Info (void);					//Returns pointer to game info
		int    Get_MPlayer_Game_Info_Length(){return(MPlayerGameInfoLength);};	//Len of game info
		BOOL	 Callback(unsigned char *data, long length);	//DDE callback function
		void	 Delete_MPlayer_Game_Info(void);					//release the game info memory
		void	 Enable(void);											//Enable the DDE callback
		void	 Disable(void);										//Disable the DDE callback
		int	 Time_Since_Heartbeat(void);						//Returns the time since the last hearbeat from WChat

		/*
		** Enumeration for DDE packet types from WChat
		*/
		enum {
			DDE_PACKET_START_MPLAYER_GAME,		//Start game packet. This includes game options
			DDE_PACKET_GAME_RESULTS,				//Game results packet. The game statistics.
			DDE_PACKET_HEART_BEAT,					//Heart beat packet so we know WChat is still there.
			DDE_TICKLE,									//Message to prompt other app to take focus.
			DDE_CONNECTION_FAILED
		};


	private:

		char 				*MPlayerGameInfo;			//Pointer to game start packet
		int				MPlayerGameInfoLength;	//Length of game start packet.
		BOOL				IsEnabled;					//Flag for DDE callback enable
		int				LastHeartbeat;		// Time since last heartbeat packet was received from WChat

};

#endif // CCDDE legacy guards
