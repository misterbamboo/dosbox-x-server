#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#include "bitmapinfoheader.h"
#include "dosbox.h"
// #include "control.h"
#include "hardware.h"
#include "logging.h"
#include "setup.h"
#include "support.h"
#include "mem.h"
// #include "mapper.h"
#include "pic.h"
#include "vga.h"
#include "mixer.h"
// #include "render.h"
// #include "cross.h"
#include "wave_mmreg.h"

#if (C_SSHOT) || (C_AVCODEC)
#include <zlib.h>
#include <png.h>
#include "../libs/zmbv/zmbv.h"
#endif

#include "riff_wav_writer.h"
#include "avi_writer.h"
#include "rawint.h"

#include <map>

void SERVER_Init() {
}