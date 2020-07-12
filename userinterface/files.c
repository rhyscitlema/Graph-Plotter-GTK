/*
    files.c
*/

#include "files.h"
#include <userinterface.h>
#include <tools.h>
#include <_stdio.h>


static wchar file_name [MAX_PATH_SIZE];
static bool file_exists = false;

const wchar* file_name_get() { return file_name; }
bool file_exists_get() { return file_exists; }


bool open_file (const wchar* fileName)
{
    Array2 content={0};
    if((content = FileOpen2(fileName, content)).size<=0)
    { sprintf2(errorMessage(), L"Error: cannot open file '%s'\n", fileName); return false; }
    display_main_text(content.data);
    wchar_free(content.data);
    return true;
}

