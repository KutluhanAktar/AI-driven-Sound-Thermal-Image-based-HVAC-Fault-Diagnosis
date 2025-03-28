#define home_width  40
#define home_height 40
PROGMEM static const unsigned char home_bits[] = {
0x00, 0x00, 0xFC, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x07, 0xFF, 0xC0, 0x00, 0x00,
0x0F, 0xFF, 0xE0, 0x00, 0x00, 0x1F, 0xFF, 0xF0, 0x00, 0x00, 0x3F, 0xFF, 0xF0, 0x00, 0x00, 0x3F,
0xFF, 0xF8, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x07, 0xFF, 0xFF, 0xFF, 0xE0, 0x0F, 0xFF, 0xFF,
0xFF, 0xF0, 0x1F, 0xFF, 0xFF, 0xFF, 0xF8, 0x3F, 0xFF, 0xFF, 0xFF, 0xFC, 0x7F, 0xFF, 0xFF, 0xFF,
0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F,
0xFF, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFE, 0x3F, 0xFF,
0xFF, 0xFF, 0xFC, 0x1F, 0xFF, 0xFF, 0xFF, 0xF8, 0x0F, 0xFF, 0xFF, 0xFF, 0xF0, 0x03, 0xFF, 0xFF,
0xFF, 0xC0, 0x00, 0x7F, 0xFF, 0xFE, 0x00, 0x00, 0x01, 0xDB, 0x80, 0x00, 0x00, 0x01, 0xDB, 0x80,
0x00, 0x07, 0x81, 0xDB, 0x81, 0xE0, 0x0F, 0xFF, 0xDB, 0xFF, 0xF0, 0x0F, 0xFF, 0xDB, 0xFF, 0xF0,
0x0F, 0xFF, 0x99, 0xFF, 0xF0, 0x07, 0x80, 0x18, 0x01, 0xE0, 0x00, 0x00, 0x38, 0x00, 0x00, 0x00,
0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00,
0x3C, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 0x00, 
};

#define scan_width  32
#define scan_height 45
PROGMEM static const unsigned char scan_bits[] = {
0x1F, 0xFF, 0xFF, 0xC0, 0x7F, 0xFF, 0xFF, 0xF0, 0x7F, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0,
0xF8, 0x00, 0x00, 0xF0, 0xF1, 0xE0, 0x00, 0xF0, 0xF1, 0xF0, 0x00, 0xF6, 0xF0, 0xF0, 0x00, 0xF7,
0xF0, 0xF0, 0x00, 0xF7, 0xF0, 0xF0, 0x00, 0xF7, 0xF0, 0xF0, 0x00, 0xF7, 0xF0, 0xF3, 0xC0, 0xF7,
0xF0, 0xF7, 0xE0, 0xF7, 0xF0, 0xF7, 0xF8, 0xF7, 0xF0, 0xF7, 0xFE, 0xF7, 0xF0, 0xF0, 0x7E, 0xF7,
0xF1, 0xE0, 0x1E, 0xF7, 0xF1, 0xE0, 0x0E, 0xF7, 0xF1, 0xC0, 0x00, 0xF7, 0xF8, 0x00, 0x00, 0xF6,
0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xFF, 0xFF, 0xF0, 0x7C, 0x1F, 0x83, 0xF0, 0x3D, 0xCF, 0xB9, 0xE0,
0x3C, 0x0F, 0x83, 0xC0, 0x1E, 0x3F, 0xC7, 0xC0, 0x1F, 0xFF, 0xFF, 0x80, 0x0F, 0xFF, 0xFF, 0x80,
0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xF8, 0x00, 0x00, 0x80, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00,
0x00, 0x7F, 0xF0, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x7F, 0xF0, 0x00,
0x00, 0x7F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x7F, 0xF0, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x7F, 0xF0, 0x00, 0x00, 0x7F, 0xF0, 0x00,
0x00, 0x3F, 0xC0, 0x00, 
};

#define inspect_width  48
#define inspect_height 38
PROGMEM static const unsigned char inspect_bits[] = {
0x1F, 0xFF, 0xFF, 0xFF, 0xFF, 0xF8, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF,
0xFF, 0xFE, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00,
0x00, 0x00, 0x00, 0x07, 0xE0, 0x30, 0x00, 0x00, 0x00, 0x07, 0xE0, 0xFC, 0x00, 0x00, 0x00, 0x07,
0xE1, 0xFE, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xFF, 0x00, 0x00, 0x00, 0x07, 0xE1, 0xFF, 0x00, 0x00,
0x00, 0x07, 0xE1, 0xFF, 0x00, 0x03, 0x00, 0x07, 0xE1, 0xFF, 0x00, 0x07, 0x80, 0x07, 0xE1, 0xFE,
0x00, 0x0F, 0xC0, 0x07, 0xE0, 0xFE, 0x00, 0x1F, 0xE0, 0x07, 0xE0, 0x78, 0x00, 0x3F, 0xF0, 0x07,
0xE0, 0x00, 0x00, 0x7F, 0xF8, 0x07, 0xE0, 0x00, 0x00, 0xFF, 0xFC, 0x07, 0xE0, 0x00, 0x01, 0xFF,
0xFE, 0x07, 0xE0, 0x02, 0x03, 0xFF, 0xFF, 0x07, 0xE0, 0x07, 0x07, 0xFF, 0xFF, 0x87, 0xE0, 0x0F,
0x8F, 0xFF, 0xFF, 0xC7, 0xE0, 0x1F, 0xDF, 0xFF, 0xFF, 0xC7, 0xE0, 0x3F, 0xFF, 0xFF, 0xFF, 0xC7,
0xE0, 0x7F, 0xFF, 0xFF, 0xFF, 0xC7, 0xE0, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xE1, 0xFF, 0xFF, 0xFF,
0xFF, 0xC7, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xE1, 0xFF,
0xFF, 0xFF, 0xFF, 0xC7, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xC7, 0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0x87,
0xE0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x07, 0xF0, 0x00, 0x00, 0x00,
0x00, 0x0F, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x7F, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE, 0x3F, 0xFF,
0xFF, 0xFF, 0xFF, 0xFC, 
};

// Define the assigned interface logo information as arrays.
PROGMEM static const unsigned char *interface_logos[] = {home_bits, scan_bits, inspect_bits};
int interface_widths[] = {home_width, scan_width, inspect_width};
int interface_heights[] = {home_height, scan_height, inspect_height};

#define saved_width  24
#define saved_height 24
PROGMEM static const unsigned char saved_bits[] = {
0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x07, 0xC0, 0x00, 0x07, 0xC0, 0x00, 0x07, 0xC0, 0x00,
0x0F, 0xC0, 0x00, 0x1F, 0xC0, 0x00, 0x1F, 0x80, 0x00, 0x3F, 0xFC, 0x00, 0x7F, 0xFE, 0xFC, 0xFF,
0xFF, 0xFD, 0xFF, 0xFF, 0xFD, 0xFF, 0xFF, 0xFD, 0xFF, 0xFE, 0xFD, 0xFF, 0xFE, 0xFD, 0xFF, 0xFE,
0xFD, 0xFF, 0xFE, 0xFD, 0xFF, 0xFE, 0xCD, 0xFF, 0xFE, 0xDD, 0xFF, 0xFC, 0xFD, 0xFF, 0xFC, 0x7C,
0xFF, 0xFC, 0x00, 0x1F, 0xF8, 0x00, 0x00, 0x00, 
};

#define not_saved_width  24
#define not_saved_height 24
PROGMEM static const unsigned char not_saved_bits[] = {
0x00, 0x7E, 0x00, 0x03, 0xFF, 0xC0, 0x07, 0xFF, 0xE0, 0x0F, 0xFF, 0xF0, 0x1F, 0x00, 0xF8, 0x3F,
0x00, 0x7C, 0x7F, 0x80, 0x3E, 0x7F, 0xC0, 0x1E, 0x73, 0xE0, 0x0E, 0xF1, 0xF0, 0x0F, 0xF0, 0xF8,
0x0F, 0xF0, 0x7C, 0x0F, 0xF0, 0x3E, 0x0F, 0xF0, 0x1F, 0x0F, 0xF0, 0x0F, 0x8F, 0x70, 0x07, 0xCE,
0x78, 0x03, 0xFE, 0x7C, 0x01, 0xFE, 0x3E, 0x00, 0xFC, 0x1F, 0x00, 0xF8, 0x0F, 0xFF, 0xF0, 0x07,
0xFF, 0xE0, 0x03, 0xFF, 0xC0, 0x00, 0x7E, 0x00, 
};

// Define the assigned buffer status icon information as arrays.
PROGMEM static const unsigned char *status_logos[] = {not_saved_bits, saved_bits};
int status_widths[] = {not_saved_width, saved_width};
int status_heights[] = {not_saved_height, saved_height};