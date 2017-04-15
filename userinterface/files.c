/*
    files.c
*/

#include "files.h"
#include <userinterface.h>
#include <tools.h>
#include <_stdio.h>


static mchar file_name [MAX_PATH_SIZE];
static bool file_exists = false;

const mchar* file_name_get() { return file_name; }
bool file_exists_get() { return file_exists; }


bool open_file (const mchar* fileName)
{
    mchar* fileContent = NULL;
    if(!Openfile (fileName, &fileContent, 0))
    { sprintf2(errorMessage(), CST21("Error: cannot open file '%s'\n"), fileName); return false; }
    display_main_text(fileContent);
    mchar_free(fileContent);
    return true;
}
