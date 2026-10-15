// quakedef.h -- primary header for the engine (hw.dll)
//
// Cry of Fear ships its own build of the GoldSrc engine (2013-04-21, Visual
// C++ 6.0). Valve never released this source; the declarations follow the
// Linux build's debug information (tools/dwarf_headers.py) and are checked
// against the Windows binary file by file.

#ifndef QUAKEDEF_H
#define QUAKEDEF_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char	byte;
typedef int				qboolean;
typedef int				int32;
typedef void *			FileHandle_t;

#include "common.h"
#include "filesystem_.h"
#include "console.h"
#include "sys.h"

#endif // QUAKEDEF_H
