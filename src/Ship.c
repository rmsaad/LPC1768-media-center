/* GIMP RGB C-Source image dump (Ship.c) */

#define SHIP_WIDTH (31)
#define SHIP_HEIGHT (21)
#define SHIP_BYTES_PER_PIXEL (2) /* 2:RGB16, 3:RGB, 4:RGBA */
#define SHIP_PIXEL_DATA ((unsigned char*) SHIP_pixel_data)
const unsigned char SHIP[31 * 21 * 2 + 1] =
("\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\000\000\000\000\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377"
 "\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377"
 "\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\000\000\000"
 "\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
 "\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\000\000\000\000\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\000\000\000\000\000\000\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\000\000\000\000\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377\377"
 "\377\377\377\377\377\377\377");
