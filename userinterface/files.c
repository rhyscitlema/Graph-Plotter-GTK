/*
    files.c
*/

#include <files.h>
#include <userinterface.h>
#include <tools.h>
#include <_stdio.h>


static wchar g_file_name[MAX_PATH_LEN+1];
const wchar* get_file_name() { return g_file_name[0] ? g_file_name : NULL; }


bool open_file (const wchar* fileName)
{
    value stack = stackArray();
    if(VERROR(FileOpen2(fileName, stack)))
    { display_message(getMessage(vGet(stack))); return false; }
    strcpy22(g_file_name, fileName);
    display_main_text(getStr2(vGet(stack)));
    return true;
}

