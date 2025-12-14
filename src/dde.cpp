#include "dde.h"
#include <cstring>

DWORD Instance_Class::id_inst = 1;
BOOL Instance_Class::process_pokes = FALSE;
char Instance_Class::ascii_name[32] = {0};
BOOL CALLBACK (*Instance_Class::callback)(LPBYTE, long) = NULL;

Instance_Class::Instance_Class(LPSTR name1, LPSTR name2)
{
    // Portable stub: record ascii name and initialize handles to null
    strncpy(ascii_name, name1, sizeof(ascii_name)-1);
    remote_name = nullptr;
    local_name = nullptr;
    system_topic = nullptr;
    poke_topic = nullptr;
    poke_item = nullptr;
    conv_handle = 0;
    dde_error = FALSE;
}

Instance_Class::~Instance_Class()
{
    // Nothing to clean up in the stub implementation
}

BOOL Instance_Class::Enable_Callback(BOOL flag)
{
    process_pokes = flag;
    return process_pokes;
}

BOOL Instance_Class::Register_Server(BOOL CALLBACK (*callback_fnc)(LPBYTE, long))
{
    callback = callback_fnc;
    return TRUE;
}

BOOL Instance_Class::Test_Server_Running(HSZ)
{
    // No remote server available in this portable build
    return FALSE;
}

BOOL Instance_Class::Open_Poke_Connection(HSZ)
{
    // Pretend we can always "connect" for local testing
    return TRUE;
}

BOOL Instance_Class::Close_Poke_Connection(void)
{
    return TRUE;
}

BOOL Instance_Class::Poke_Server(LPBYTE /*poke_data*/, DWORD /*poke_length*/)
{
    // Simulate a successful poke
    return TRUE;
}

HDDEDATA CALLBACK Instance_Class::dde_callback(UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD)
{
    return (HDDEDATA)NULL;
}
