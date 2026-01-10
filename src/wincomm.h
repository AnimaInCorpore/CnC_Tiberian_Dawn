#pragma once

#include "legacy_compat.h"

/*
 * Minimal serial/modem declarations needed to build stubbed comms modules.
 * The full Win32 implementation will be replaced by an SDL/portable backend.
 */

#ifndef MPLAYER_NAME_MAX
#define MPLAYER_NAME_MAX 12
#endif

#ifndef COMPAT_MESSAGE_LENGTH
#define COMPAT_MESSAGE_LENGTH 28
#endif

#ifndef INITSTRBUF_MAX
#define INITSTRBUF_MAX 41
#endif

#ifndef CWAITSTRBUF_MAX
#define CWAITSTRBUF_MAX 16
#endif

#ifndef PORTBUF_MAX
#define PORTBUF_MAX 64
#endif

#ifndef MODEM_NAME_MAX
#define MODEM_NAME_MAX (PORTBUF_MAX - 1)
#endif

typedef enum DetectPortType {
    PORT_VALID = 0,
    PORT_INVALID,
    PORT_IRQ_INUSE
} DetectPortType;

typedef enum DialStatusType {
    DIAL_CONNECTED = 0,
    DIAL_NO_CARRIER,
    DIAL_BUSY,
    DIAL_ERROR,
    DIAL_NO_DIAL_TONE,
    DIAL_CANCELED
} DialStatusType;

typedef enum DialMethodType {
    DIAL_TOUCH_TONE = 0,
    DIAL_PULSE,
    DIAL_METHODS
} DialMethodType;

typedef struct {
    int Port;
    int IRQ;
    int Baud;
    DialMethodType DialMethod;
    int InitStringIndex;
    int CallWaitStringIndex;
    char CallWaitString[CWAITSTRBUF_MAX];
    bool Init;
    bool Compression;
    bool ErrorCorrection;
    bool HardwareFlowControl;
    char ModemName[MODEM_NAME_MAX];
} SerialSettingsType;
