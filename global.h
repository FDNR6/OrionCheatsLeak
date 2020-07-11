#pragma warning(disable : 4530)
#pragma comment(linker,"/MERGE:.rdata=.text")
#pragma comment(linker,"/MERGE:.pdata=.text")
extern "C" int _fltused = 0;

#include <windows.h>
#include <winternl.h>
#include <intrin.h>
#include <stdio.h>
#include <cfloat>
#include <d3d11.h>



#include "encypt.h"
#include "utils.h"
#include "math.h"
#include "config.h"
#include "offsets.h"
#include "sdk.h"
#include "render.h"
#include "gui.h"

