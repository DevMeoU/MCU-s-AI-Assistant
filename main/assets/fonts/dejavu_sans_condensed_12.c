/*******************************************************************************
 * Size: 12 px
 * Bpp: 1
 * Opts: --bpp 1 --size 12 --no-compress --stride 1 --align 1 --font DejaVuSansCondensed.ttf --symbols aAàÀảẢãÃáÁạẠăĂằẰẳẲẵẴắẮặẶâÂầẦẩẨẫẪấẤậẬbBcCdDđĐeEèÈẻẺẽẼéÉẹẸêÊềỀểỂễỄếẾệỆfFgGhHiIìÌỉỈĩĨíÍịỊjJkKlLmMnNoOòÒỏỎõÕóÓọỌôÔồỒổỔỗỖốỐộỘơƠờỜởỞỡỠớỚợỢpPqQrRsStTuUùÙủỦũŨúÚụỤưƯừỪửỬữỮứỨựỰvVwWxXyYỳỲỷỶỹỸýÝỵỴzZ~!@#$%^\&*()_\-+={}\\|:;\"'<>.,/ ?`~
 --format lvgl -o dejavu_sans_condensed_12.c
 ******************************************************************************/

#ifdef __has_include
    #if __has_include("lvgl.h")
        #ifndef LV_LVGL_H_INCLUDE_SIMPLE
            #define LV_LVGL_H_INCLUDE_SIMPLE
        #endif
    #endif
#endif

#ifdef LV_LVGL_H_INCLUDE_SIMPLE
    #include "lvgl.h"
#else
    #include "lvgl/lvgl.h"
#endif



#ifndef DEJAVU_SANS_CONDENSED_12
#define DEJAVU_SANS_CONDENSED_12 1
#endif

#if DEJAVU_SANS_CONDENSED_12

/*-----------------
 *    BITMAPS
 *----------------*/

/*Store the image of the glyphs*/
static LV_ATTRIBUTE_LARGE_CONST const uint8_t glyph_bitmap[] = {
    /* U+0020 " " */
    0x0,

    /* U+0021 "!" */
    0xfc, 0x80,

    /* U+0022 "\"" */
    0xb6, 0x80,

    /* U+0023 "#" */
    0x14, 0x48, 0x97, 0xf2, 0x9f, 0xd2, 0x28, 0x50,

    /* U+0024 "$" */
    0x21, 0x1f, 0x5a, 0x70, 0xe5, 0x2f, 0x88, 0x40,

    /* U+0025 "%" */
    0x62, 0x4a, 0x25, 0x13, 0x6, 0xb0, 0xa4, 0x52,
    0x49, 0x23, 0x0,

    /* U+0026 "&" */
    0x38, 0x81, 0x3, 0xb, 0x33, 0x63, 0x66, 0x7a,

    /* U+0027 "'" */
    0xe0,

    /* U+0028 "(" */
    0x5a, 0xaa, 0x94,

    /* U+0029 ")" */
    0xa5, 0x55, 0x68,

    /* U+002A "*" */
    0x25, 0x5d, 0xf2, 0x0,

    /* U+002B "+" */
    0x10, 0x20, 0x40, 0x8f, 0xe2, 0x4, 0x8,

    /* U+002C "," */
    0xe0,

    /* U+002D "-" */
    0xe0,

    /* U+002E "." */
    0x80,

    /* U+002F "/" */
    0x12, 0x22, 0x44, 0x44, 0x88, 0x80,

    /* U+003A ":" */
    0x84,

    /* U+003B ";" */
    0x87,

    /* U+003C "<" */
    0x6, 0x33, 0x86, 0x3, 0x80, 0xc0,

    /* U+003D "=" */
    0xfe, 0x0, 0x7, 0xf0,

    /* U+003E ">" */
    0xc0, 0x60, 0x38, 0x33, 0x98, 0x0,

    /* U+003F "?" */
    0xe1, 0x13, 0x64, 0x40, 0x40,

    /* U+0040 "@" */
    0x1e, 0x31, 0x90, 0x73, 0xda, 0x2d, 0x16, 0x8b,
    0x3e, 0x40, 0x31, 0x7, 0x0,

    /* U+0041 "A" */
    0x10, 0x30, 0xa1, 0x42, 0x48, 0x9f, 0x21, 0x82,

    /* U+0042 "B" */
    0xf2, 0x28, 0xa2, 0xf2, 0x38, 0x61, 0xf8,

    /* U+0043 "C" */
    0x3c, 0x86, 0x4, 0x8, 0x10, 0x20, 0x21, 0x3c,

    /* U+0044 "D" */
    0xf9, 0xa, 0xc, 0x18, 0x30, 0x60, 0xc2, 0xf8,

    /* U+0045 "E" */
    0xfc, 0x21, 0xf, 0xc2, 0x10, 0xf8,

    /* U+0046 "F" */
    0xf8, 0x88, 0xf8, 0x88, 0x80,

    /* U+0047 "G" */
    0x3c, 0x86, 0x4, 0x8, 0xf0, 0x60, 0xa1, 0x3c,

    /* U+0048 "H" */
    0x86, 0x18, 0x61, 0xfe, 0x18, 0x61, 0x84,

    /* U+0049 "I" */
    0xff, 0x80,

    /* U+004A "J" */
    0x24, 0x92, 0x49, 0x24, 0xe0,

    /* U+004B "K" */
    0x8a, 0x6b, 0x38, 0xc2, 0x89, 0x26, 0x8c,

    /* U+004C "L" */
    0x84, 0x21, 0x8, 0x42, 0x10, 0xf8,

    /* U+004D "M" */
    0xc7, 0x8f, 0x1d, 0x5a, 0xb5, 0x64, 0xc1, 0x82,

    /* U+004E "N" */
    0xc7, 0x1e, 0x69, 0xb6, 0x59, 0xe3, 0x8c,

    /* U+004F "O" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xa2, 0x38,

    /* U+0050 "P" */
    0xf4, 0x63, 0x1f, 0x42, 0x10, 0x80,

    /* U+0051 "Q" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xa2, 0x38,
    0x18, 0x10,

    /* U+0052 "R" */
    0xf2, 0x28, 0xa2, 0xf2, 0x48, 0xa2, 0x84,

    /* U+0053 "S" */
    0x7c, 0x61, 0x87, 0x4, 0x31, 0xf0,

    /* U+0054 "T" */
    0xfe, 0x20, 0x40, 0x81, 0x2, 0x4, 0x8, 0x10,

    /* U+0055 "U" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x73, 0x78,

    /* U+0056 "V" */
    0x82, 0x85, 0x12, 0x22, 0x45, 0xa, 0xc, 0x10,

    /* U+0057 "W" */
    0xc4, 0x53, 0x14, 0xc5, 0x2b, 0x4a, 0x8c, 0xa3,
    0x28, 0xc6, 0x31, 0x0,

    /* U+0058 "X" */
    0x46, 0x48, 0xa0, 0xc1, 0x7, 0xb, 0x22, 0x42,

    /* U+0059 "Y" */
    0x44, 0x88, 0xa1, 0x41, 0x2, 0x4, 0x8, 0x10,

    /* U+005A "Z" */
    0xfc, 0x30, 0x84, 0x30, 0x84, 0x30, 0xfc,

    /* U+005C "\\" */
    0x88, 0x44, 0x44, 0x22, 0x22, 0x10,

    /* U+005E "^" */
    0x10, 0x71, 0xb2, 0x20,

    /* U+005F "_" */
    0xfc,

    /* U+0060 "`" */
    0x94,

    /* U+0061 "a" */
    0xf4, 0x42, 0xf8, 0xc5, 0xe0,

    /* U+0062 "b" */
    0x84, 0x21, 0x6d, 0xc6, 0x31, 0xdd, 0x80,

    /* U+0063 "c" */
    0x7e, 0x61, 0x8, 0x65, 0xe0,

    /* U+0064 "d" */
    0x8, 0x42, 0xdd, 0xc6, 0x31, 0xdb, 0x40,

    /* U+0065 "e" */
    0x76, 0x63, 0xf8, 0x61, 0xe0,

    /* U+0066 "f" */
    0x34, 0x4f, 0x44, 0x44, 0x44,

    /* U+0067 "g" */
    0x6e, 0xe3, 0x18, 0xed, 0xa1, 0x1b, 0x80,

    /* U+0068 "h" */
    0x84, 0x21, 0x6c, 0xc6, 0x31, 0x8c, 0x40,

    /* U+0069 "i" */
    0x9f, 0xc0,

    /* U+006A "j" */
    0x41, 0x55, 0x55, 0x80,

    /* U+006B "k" */
    0x84, 0x21, 0x3b, 0x73, 0x14, 0x94, 0x40,

    /* U+006C "l" */
    0xff, 0xc0,

    /* U+006D "m" */
    0xb3, 0x66, 0x62, 0x31, 0x18, 0x8c, 0x46, 0x22,

    /* U+006E "n" */
    0xb6, 0x63, 0x18, 0xc6, 0x20,

    /* U+006F "o" */
    0x76, 0xe3, 0x18, 0xed, 0xc0,

    /* U+0070 "p" */
    0xb6, 0xe3, 0x18, 0xee, 0xd0, 0x84, 0x0,

    /* U+0071 "q" */
    0x6e, 0xe3, 0x18, 0xed, 0xa1, 0x8, 0x40,

    /* U+0072 "r" */
    0xba, 0x49, 0x20,

    /* U+0073 "s" */
    0x78, 0x86, 0x11, 0xe0,

    /* U+0074 "t" */
    0x44, 0xf4, 0x44, 0x44, 0x70,

    /* U+0075 "u" */
    0x8c, 0x63, 0x18, 0xcd, 0xa0,

    /* U+0076 "v" */
    0xc5, 0x14, 0x92, 0x28, 0xc3, 0x0,

    /* U+0077 "w" */
    0x49, 0x49, 0x55, 0x55, 0x56, 0x26, 0x22,

    /* U+0078 "x" */
    0x4d, 0xa3, 0xc, 0x31, 0x24, 0x40,

    /* U+0079 "y" */
    0x45, 0x14, 0x92, 0x28, 0xc1, 0x4, 0x21, 0x80,

    /* U+007A "z" */
    0xf8, 0x8c, 0x44, 0x43, 0xe0,

    /* U+007B "{" */
    0x39, 0x8, 0x42, 0x60, 0x84, 0x21, 0x8, 0x70,

    /* U+007C "|" */
    0xff, 0xf8,

    /* U+007D "}" */
    0xe1, 0x8, 0x42, 0xc, 0x84, 0x21, 0x9, 0xc0,

    /* U+007E "~" */
    0x73, 0x38,

    /* U+00C0 "À" */
    0x20, 0x20, 0x0, 0x81, 0x85, 0xa, 0x12, 0x44,
    0xf9, 0xc, 0x10,

    /* U+00C1 "Á" */
    0x8, 0x20, 0x0, 0x81, 0x85, 0xa, 0x12, 0x44,
    0xf9, 0xc, 0x10,

    /* U+00C2 "Â" */
    0x10, 0x50, 0x0, 0x81, 0x85, 0xa, 0x12, 0x44,
    0xf9, 0xc, 0x10,

    /* U+00C3 "Ã" */
    0x38, 0x50, 0x0, 0x81, 0x85, 0xa, 0x12, 0x44,
    0xf9, 0xc, 0x10,

    /* U+00C8 "È" */
    0x41, 0x1, 0xf8, 0x42, 0x1f, 0x84, 0x21, 0xf0,

    /* U+00C9 "É" */
    0x11, 0x1, 0xf8, 0x42, 0x1f, 0x84, 0x21, 0xf0,

    /* U+00CA "Ê" */
    0x22, 0x81, 0xf8, 0x42, 0x1f, 0x84, 0x21, 0xf0,

    /* U+00CC "Ì" */
    0x91, 0x55, 0x55,

    /* U+00CD "Í" */
    0x62, 0xaa, 0xaa,

    /* U+00D2 "Ò" */
    0x20, 0x20, 0x1, 0xc4, 0x50, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+00D3 "Ó" */
    0x10, 0x20, 0x1, 0xc4, 0x50, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+00D4 "Ô" */
    0x10, 0x50, 0x1, 0xc4, 0x50, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+00D5 "Õ" */
    0x38, 0xb0, 0x1, 0xc4, 0x50, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0,

    /* U+00D9 "Ù" */
    0x20, 0x80, 0x21, 0x86, 0x18, 0x61, 0x86, 0x1c,
    0xde,

    /* U+00DA "Ú" */
    0x10, 0x80, 0x21, 0x86, 0x18, 0x61, 0x86, 0x1c,
    0xde,

    /* U+00DD "Ý" */
    0x8, 0x20, 0x2, 0x24, 0x45, 0xa, 0x8, 0x10,
    0x20, 0x40, 0x80,

    /* U+00E0 "à" */
    0x41, 0x9, 0xe8, 0x85, 0xf1, 0x8b, 0xc0,

    /* U+00E1 "á" */
    0x11, 0x9, 0xe8, 0x85, 0xf1, 0x8b, 0xc0,

    /* U+00E2 "â" */
    0x22, 0x95, 0xe8, 0x85, 0xf1, 0x8b, 0xc0,

    /* U+00E3 "ã" */
    0x75, 0x81, 0xe8, 0x85, 0xf1, 0x8b, 0xc0,

    /* U+00E8 "è" */
    0x41, 0x0, 0xec, 0xc7, 0xf0, 0xc3, 0xc0,

    /* U+00E9 "é" */
    0x11, 0x0, 0xec, 0xc7, 0xf0, 0xc3, 0xc0,

    /* U+00EA "ê" */
    0x22, 0x80, 0xec, 0xc7, 0xf0, 0xc3, 0xc0,

    /* U+00EC "ì" */
    0x95, 0x55, 0x50,

    /* U+00ED "í" */
    0x6a, 0xaa, 0xa0,

    /* U+00F2 "ò" */
    0x41, 0x0, 0xed, 0xc6, 0x31, 0xdb, 0x80,

    /* U+00F3 "ó" */
    0x11, 0x0, 0xed, 0xc6, 0x31, 0xdb, 0x80,

    /* U+00F4 "ô" */
    0x22, 0x80, 0xed, 0xc6, 0x31, 0xdb, 0x80,

    /* U+00F5 "õ" */
    0x73, 0x80, 0xed, 0xc6, 0x31, 0xdb, 0x80,

    /* U+00F9 "ù" */
    0x41, 0x9, 0x18, 0xc6, 0x31, 0x9b, 0x40,

    /* U+00FA "ú" */
    0x11, 0x9, 0x18, 0xc6, 0x31, 0x9b, 0x40,

    /* U+00FD "ý" */
    0x8, 0x42, 0x11, 0x45, 0x24, 0x8a, 0x30, 0x41,
    0x8, 0x60,

    /* U+0102 "Ă" */
    0x28, 0x70, 0x0, 0x83, 0x5, 0xa, 0x34, 0x44,
    0xfb, 0x14, 0x10,

    /* U+0103 "ă" */
    0x53, 0x80, 0xe0, 0x85, 0xf1, 0x8b, 0xc0,

    /* U+0110 "Đ" */
    0x7c, 0x42, 0x41, 0x41, 0xf1, 0x41, 0x41, 0x42,
    0x7c,

    /* U+0111 "đ" */
    0x8, 0xf0, 0x9a, 0xda, 0x28, 0xa2, 0xd9, 0xa0,

    /* U+0128 "Ĩ" */
    0xfc, 0x24, 0x92, 0x49, 0x20,

    /* U+0129 "ĩ" */
    0xfc, 0x24, 0x92, 0x48,

    /* U+0168 "Ũ" */
    0x29, 0x40, 0x21, 0x86, 0x18, 0x61, 0x86, 0x1c,
    0xde,

    /* U+0169 "ũ" */
    0x75, 0x81, 0x18, 0xc6, 0x31, 0x9b, 0x40,

    /* U+01A0 "Ơ" */
    0x2, 0x39, 0x46, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x44, 0x38,

    /* U+01A1 "ơ" */
    0x5, 0xfd, 0xa2, 0x8a, 0x2d, 0x9c,

    /* U+01AF "Ư" */
    0x2, 0x85, 0x86, 0x84, 0x84, 0x84, 0x84, 0x84,
    0xcc, 0x78,

    /* U+01B0 "ư" */
    0x1, 0x16, 0x34, 0x48, 0x91, 0x26, 0x34,

    /* U+1EA0 "Ạ" */
    0x10, 0x70, 0xa1, 0x46, 0x48, 0x9f, 0x63, 0x82,
    0x0, 0x40,

    /* U+1EA1 "ạ" */
    0xf4, 0x42, 0xf8, 0xc5, 0xe0, 0x20,

    /* U+1EA2 "Ả" */
    0x18, 0x10, 0x40, 0x1, 0x3, 0xa, 0x14, 0x24,
    0x89, 0xf2, 0x18, 0x20,

    /* U+1EA3 "ả" */
    0x30, 0x81, 0xe8, 0x85, 0xf1, 0x8b, 0xc0,

    /* U+1EA4 "Ấ" */
    0x4, 0x28, 0xa0, 0x1, 0x3, 0xa, 0x14, 0x24,
    0x89, 0xf2, 0x18, 0x20,

    /* U+1EA5 "ấ" */
    0x4, 0xa5, 0x0, 0xf2, 0x20, 0x9e, 0x8a, 0x27,
    0x80,

    /* U+1EA6 "Ầ" */
    0x8, 0x28, 0xa0, 0x1, 0x3, 0xa, 0x14, 0x24,
    0x89, 0xf2, 0x18, 0x20,

    /* U+1EA7 "ầ" */
    0x11, 0x14, 0xf, 0x44, 0x2f, 0x8c, 0x5e,

    /* U+1EA8 "Ẩ" */
    0x6, 0x2, 0x1c, 0x0, 0x18, 0x18, 0x38, 0x24,
    0x24, 0x64, 0x7e, 0x42, 0xc2,

    /* U+1EA9 "ẩ" */
    0x18, 0xa5, 0x0, 0xf2, 0x20, 0x9e, 0x8a, 0x27,
    0x80,

    /* U+1EAA "Ẫ" */
    0x38, 0x20, 0xa0, 0x1, 0x3, 0xa, 0x14, 0x24,
    0x89, 0xf2, 0x18, 0x20,

    /* U+1EAB "ẫ" */
    0x73, 0x94, 0xf, 0x44, 0x2f, 0x8c, 0x5e,

    /* U+1EAC "Ậ" */
    0x10, 0x50, 0x0, 0x83, 0x85, 0xa, 0x32, 0x44,
    0xfb, 0xc, 0x10, 0x2, 0x0,

    /* U+1EAD "ậ" */
    0x22, 0x91, 0xe8, 0x85, 0xf1, 0x8b, 0xc0, 0x40,

    /* U+1EAE "Ắ" */
    0x8, 0x60, 0xe0, 0x1, 0x3, 0xa, 0x14, 0x24,
    0x89, 0xf2, 0x18, 0x20,

    /* U+1EAF "ắ" */
    0x11, 0x1c, 0xf, 0x44, 0x2f, 0x8c, 0x5e,

    /* U+1EB0 "Ằ" */
    0x20, 0x60, 0xe0, 0x1, 0x3, 0xa, 0x14, 0x24,
    0x89, 0xf2, 0x18, 0x20,

    /* U+1EB1 "ằ" */
    0x41, 0x1c, 0xf, 0x44, 0x2f, 0x8c, 0x5e,

    /* U+1EB2 "Ẳ" */
    0x18, 0x10, 0xc0, 0xc0, 0x3, 0x6, 0x14, 0x2c,
    0x49, 0x93, 0xf4, 0x30, 0x40,

    /* U+1EB3 "ẳ" */
    0x60, 0x88, 0xe0, 0x7a, 0x21, 0x7c, 0x62, 0xf0,

    /* U+1EB4 "Ẵ" */
    0x38, 0x40, 0xe0, 0x1, 0x3, 0xa, 0x14, 0x24,
    0x89, 0xf2, 0x18, 0x20,

    /* U+1EB5 "ẵ" */
    0x70, 0x1c, 0xf, 0x44, 0x2f, 0x8c, 0x5e,

    /* U+1EB6 "Ặ" */
    0x28, 0x70, 0x0, 0x83, 0x85, 0xa, 0x34, 0x44,
    0xfb, 0x14, 0x10, 0x2, 0x0,

    /* U+1EB7 "ặ" */
    0x53, 0x80, 0xe0, 0x85, 0xf1, 0x8b, 0xc0, 0x40,

    /* U+1EB8 "Ẹ" */
    0xfc, 0x21, 0xf, 0xc2, 0x10, 0xf8, 0x8,

    /* U+1EB9 "ẹ" */
    0x76, 0x63, 0xf8, 0x61, 0xe0, 0x20,

    /* U+1EBA "Ẻ" */
    0x70, 0x88, 0xf, 0xc2, 0x10, 0xfc, 0x21, 0xf,
    0x80,

    /* U+1EBB "ẻ" */
    0x20, 0x88, 0x7, 0x66, 0x3f, 0x86, 0x1e,

    /* U+1EBC "Ẽ" */
    0x55, 0x81, 0xf8, 0x42, 0x1f, 0x84, 0x21, 0xf0,

    /* U+1EBD "ẽ" */
    0x72, 0x80, 0xec, 0xc7, 0xf0, 0xc3, 0xc0,

    /* U+1EBE "Ế" */
    0x8, 0xa5, 0x0, 0xfa, 0x8, 0x20, 0xfa, 0x8,
    0x20, 0xf8,

    /* U+1EBF "ế" */
    0x8, 0x85, 0x0, 0x73, 0x28, 0xbe, 0x83, 0x7,
    0x80,

    /* U+1EC0 "Ề" */
    0x11, 0x54, 0xf, 0xc2, 0x10, 0xfc, 0x21, 0xf,
    0x80,

    /* U+1EC1 "ề" */
    0x11, 0x14, 0x7, 0x66, 0x3f, 0x86, 0x1e,

    /* U+1EC2 "Ể" */
    0x18, 0x26, 0x84, 0xfa, 0x8, 0x20, 0xfa, 0x8,
    0x20, 0xf8,

    /* U+1EC3 "ể" */
    0x1c, 0xa5, 0x0, 0x73, 0x28, 0xbe, 0x83, 0x7,
    0x80,

    /* U+1EC4 "Ễ" */
    0x71, 0x14, 0xf, 0xc2, 0x10, 0xfc, 0x21, 0xf,
    0x80,

    /* U+1EC5 "ễ" */
    0x71, 0x14, 0x7, 0x66, 0x3f, 0x86, 0x1e,

    /* U+1EC6 "Ệ" */
    0x22, 0x81, 0xf8, 0x42, 0x1f, 0x84, 0x21, 0xf0,
    0x10,

    /* U+1EC7 "ệ" */
    0x22, 0x80, 0xec, 0xc7, 0xf0, 0xc3, 0xc0, 0x40,

    /* U+1EC8 "Ỉ" */
    0x65, 0x4, 0x92, 0x49, 0x24,

    /* U+1EC9 "ỉ" */
    0xc0, 0x24, 0x92, 0x48,

    /* U+1ECA "Ị" */
    0xff, 0xa0,

    /* U+1ECB "ị" */
    0x9f, 0xd0,

    /* U+1ECC "Ọ" */
    0x38, 0x8a, 0xc, 0x18, 0x30, 0x60, 0xa2, 0x38,
    0x0, 0x40,

    /* U+1ECD "ọ" */
    0x76, 0xe3, 0x18, 0xed, 0xc0, 0x20,

    /* U+1ECE "Ỏ" */
    0x18, 0x10, 0x40, 0x3, 0x88, 0xa0, 0xc1, 0x83,
    0x6, 0xa, 0x23, 0x80,

    /* U+1ECF "ỏ" */
    0x30, 0x88, 0x7, 0x6e, 0x31, 0x8e, 0xdc,

    /* U+1ED0 "Ố" */
    0x4, 0x20, 0xa0, 0x3, 0x88, 0xa0, 0xc1, 0x83,
    0x6, 0xa, 0x23, 0x80,

    /* U+1ED1 "ố" */
    0x9, 0x14, 0x7, 0x6e, 0x31, 0x8e, 0xdc,

    /* U+1ED2 "Ồ" */
    0x8, 0x28, 0xa0, 0x3, 0x88, 0xa0, 0xc1, 0x83,
    0x6, 0xa, 0x23, 0x80,

    /* U+1ED3 "ồ" */
    0x11, 0x14, 0x7, 0x6e, 0x31, 0x8e, 0xdc,

    /* U+1ED4 "Ổ" */
    0xc, 0x8, 0xd0, 0x43, 0x88, 0xa0, 0xc1, 0x83,
    0x6, 0xa, 0x23, 0x80,

    /* U+1ED5 "ổ" */
    0x1c, 0xa5, 0x0, 0x73, 0x68, 0xa2, 0x8b, 0x67,
    0x0,

    /* U+1ED6 "Ỗ" */
    0x38, 0xb0, 0x41, 0x40, 0x7, 0x11, 0x41, 0x83,
    0x6, 0xc, 0x14, 0x47, 0x0,

    /* U+1ED7 "ỗ" */
    0x71, 0x14, 0x7, 0x6e, 0x31, 0x8e, 0xdc,

    /* U+1ED8 "Ộ" */
    0x10, 0x50, 0x1, 0xc4, 0x50, 0x60, 0xc1, 0x83,
    0x5, 0x11, 0xc0, 0x2, 0x0,

    /* U+1ED9 "ộ" */
    0x22, 0x80, 0xed, 0xc6, 0x31, 0xdb, 0x80, 0x40,

    /* U+1EDA "Ớ" */
    0x8, 0x10, 0x2, 0x39, 0x46, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38,

    /* U+1EDB "ớ" */
    0x10, 0x80, 0x5f, 0xda, 0x28, 0xa2, 0xd9, 0xc0,

    /* U+1EDC "Ờ" */
    0x10, 0x10, 0x2, 0x39, 0x46, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38,

    /* U+1EDD "ờ" */
    0x40, 0x80, 0x5f, 0xda, 0x28, 0xa2, 0xd9, 0xc0,

    /* U+1EDE "Ở" */
    0x18, 0x8, 0x10, 0x2, 0x39, 0x46, 0x82, 0x82,
    0x82, 0x82, 0x82, 0x44, 0x38,

    /* U+1EDF "ở" */
    0x20, 0x42, 0x1, 0x7f, 0x68, 0xa2, 0x8b, 0x67,
    0x0,

    /* U+1EE0 "Ỡ" */
    0x38, 0x38, 0x2, 0x39, 0x46, 0x82, 0x82, 0x82,
    0x82, 0x82, 0x44, 0x38,

    /* U+1EE1 "ỡ" */
    0x71, 0xc0, 0x5f, 0xda, 0x28, 0xa2, 0xd9, 0xc0,

    /* U+1EE2 "Ợ" */
    0x2, 0x39, 0x46, 0x82, 0x82, 0x82, 0x82, 0x82,
    0x44, 0x38, 0x0, 0x10,

    /* U+1EE3 "ợ" */
    0x5, 0xfd, 0xa2, 0x8a, 0x2d, 0x9c, 0x0, 0x80,

    /* U+1EE4 "Ụ" */
    0x86, 0x18, 0x61, 0x86, 0x18, 0x73, 0x78, 0x1,
    0x0,

    /* U+1EE5 "ụ" */
    0x8c, 0x63, 0x18, 0xcd, 0xa0, 0x20,

    /* U+1EE6 "Ủ" */
    0x30, 0x41, 0x0, 0x86, 0x18, 0x61, 0x86, 0x18,
    0x73, 0x78,

    /* U+1EE7 "ủ" */
    0x31, 0x1, 0x18, 0xc6, 0x31, 0x9b, 0x40,

    /* U+1EE8 "Ứ" */
    0x10, 0x10, 0x2, 0x85, 0x86, 0x84, 0x84, 0x84,
    0x84, 0x84, 0xcc, 0x78,

    /* U+1EE9 "ứ" */
    0x10, 0x40, 0x84, 0x58, 0xd1, 0x22, 0x44, 0x98,
    0xd0,

    /* U+1EEA "Ừ" */
    0x20, 0x20, 0x2, 0x85, 0x86, 0x84, 0x84, 0x84,
    0x84, 0x84, 0xcc, 0x78,

    /* U+1EEB "ừ" */
    0x40, 0x40, 0x84, 0x58, 0xd1, 0x22, 0x44, 0x98,
    0xd0,

    /* U+1EEC "Ử" */
    0x30, 0x10, 0x0, 0x2, 0x85, 0x86, 0x84, 0x84,
    0x84, 0x84, 0x84, 0xcc, 0x78,

    /* U+1EED "ử" */
    0x30, 0x40, 0x4, 0x58, 0xd1, 0x22, 0x44, 0x98,
    0xd0,

    /* U+1EEE "Ữ" */
    0x28, 0x50, 0x2, 0x85, 0x86, 0x84, 0x84, 0x84,
    0x84, 0x84, 0xcc, 0x78,

    /* U+1EEF "ữ" */
    0x71, 0x60, 0x4, 0x58, 0xd1, 0x22, 0x44, 0x98,
    0xd0,

    /* U+1EF0 "Ự" */
    0x2, 0x85, 0x86, 0x84, 0x84, 0x84, 0x84, 0x84,
    0xcc, 0x78, 0x0, 0x10,

    /* U+1EF1 "ự" */
    0x1, 0x16, 0x34, 0x48, 0x91, 0x26, 0x34, 0x0,
    0x40,

    /* U+1EF2 "Ỳ" */
    0x20, 0x20, 0x2, 0x24, 0x45, 0xa, 0x8, 0x10,
    0x20, 0x40, 0x80,

    /* U+1EF3 "ỳ" */
    0x40, 0x81, 0x11, 0x45, 0x24, 0x8a, 0x30, 0x41,
    0x8, 0x60,

    /* U+1EF4 "Ỵ" */
    0xc6, 0x88, 0xa1, 0x41, 0x2, 0x4, 0x8, 0x10,
    0x0, 0x40,

    /* U+1EF5 "ỵ" */
    0x85, 0x24, 0x92, 0x30, 0xc3, 0x8, 0x29, 0x80,

    /* U+1EF6 "Ỷ" */
    0x18, 0x10, 0x40, 0x4, 0x28, 0x8a, 0x14, 0x10,
    0x20, 0x40, 0x81, 0x0,

    /* U+1EF7 "ỷ" */
    0x30, 0x40, 0x22, 0x89, 0x25, 0x14, 0x30, 0x82,
    0x8, 0xc0,

    /* U+1EF8 "Ỹ" */
    0x38, 0x70, 0x2, 0x24, 0x45, 0xa, 0x8, 0x10,
    0x20, 0x40, 0x80,

    /* U+1EF9 "ỹ" */
    0x71, 0xc5, 0x22, 0x89, 0x25, 0x14, 0x30, 0x82,
    0x8, 0xc0
};


/*---------------------
 *  GLYPH DESCRIPTION
 *--------------------*/

static const lv_font_fmt_txt_glyph_dsc_t glyph_dsc[] = {
    {.bitmap_index = 0, .adv_w = 0, .box_w = 0, .box_h = 0, .ofs_x = 0, .ofs_y = 0} /* id = 0 reserved */,
    {.bitmap_index = 0, .adv_w = 55, .box_w = 1, .box_h = 1, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1, .adv_w = 69, .box_w = 1, .box_h = 9, .ofs_x = 2, .ofs_y = 0},
    {.bitmap_index = 3, .adv_w = 79, .box_w = 3, .box_h = 3, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 5, .adv_w = 145, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 13, .adv_w = 110, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 21, .adv_w = 164, .box_w = 9, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 32, .adv_w = 135, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 40, .adv_w = 47, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 41, .adv_w = 67, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 44, .adv_w = 67, .box_w = 2, .box_h = 11, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 47, .adv_w = 86, .box_w = 5, .box_h = 5, .ofs_x = 0, .ofs_y = 4},
    {.bitmap_index = 51, .adv_w = 145, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 58, .adv_w = 55, .box_w = 1, .box_h = 3, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 59, .adv_w = 62, .box_w = 3, .box_h = 1, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 60, .adv_w = 55, .box_w = 1, .box_h = 1, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 61, .adv_w = 58, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 67, .adv_w = 58, .box_w = 1, .box_h = 6, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 68, .adv_w = 58, .box_w = 1, .box_h = 8, .ofs_x = 1, .ofs_y = -1},
    {.bitmap_index = 69, .adv_w = 145, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 75, .adv_w = 145, .box_w = 7, .box_h = 4, .ofs_x = 1, .ofs_y = 2},
    {.bitmap_index = 79, .adv_w = 145, .box_w = 7, .box_h = 6, .ofs_x = 1, .ofs_y = 1},
    {.bitmap_index = 85, .adv_w = 92, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 90, .adv_w = 173, .box_w = 9, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 103, .adv_w = 118, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 111, .adv_w = 119, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 118, .adv_w = 121, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 126, .adv_w = 133, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 134, .adv_w = 109, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 140, .adv_w = 99, .box_w = 4, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 145, .adv_w = 134, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 153, .adv_w = 130, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 160, .adv_w = 51, .box_w = 1, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 162, .adv_w = 51, .box_w = 3, .box_h = 12, .ofs_x = -1, .ofs_y = -3},
    {.bitmap_index = 167, .adv_w = 113, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 174, .adv_w = 96, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 180, .adv_w = 149, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 188, .adv_w = 129, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 195, .adv_w = 136, .box_w = 7, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 203, .adv_w = 104, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 209, .adv_w = 136, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 219, .adv_w = 120, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 226, .adv_w = 110, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 232, .adv_w = 105, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 240, .adv_w = 126, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 247, .adv_w = 118, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 255, .adv_w = 171, .box_w = 10, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 267, .adv_w = 118, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 275, .adv_w = 105, .box_w = 7, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 283, .adv_w = 118, .box_w = 6, .box_h = 9, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 290, .adv_w = 58, .box_w = 4, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 296, .adv_w = 145, .box_w = 7, .box_h = 4, .ofs_x = 1, .ofs_y = 6},
    {.bitmap_index = 300, .adv_w = 86, .box_w = 6, .box_h = 1, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 301, .adv_w = 86, .box_w = 2, .box_h = 3, .ofs_x = 1, .ofs_y = 8},
    {.bitmap_index = 302, .adv_w = 106, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 307, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 314, .adv_w = 95, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 319, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 326, .adv_w = 106, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 331, .adv_w = 61, .box_w = 4, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 336, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 343, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 350, .adv_w = 48, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 352, .adv_w = 48, .box_w = 2, .box_h = 13, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 356, .adv_w = 100, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 363, .adv_w = 48, .box_w = 1, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 365, .adv_w = 168, .box_w = 9, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 373, .adv_w = 110, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 378, .adv_w = 106, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 383, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 390, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 397, .adv_w = 71, .box_w = 3, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 400, .adv_w = 90, .box_w = 4, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 404, .adv_w = 68, .box_w = 4, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 409, .adv_w = 110, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 414, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 420, .adv_w = 141, .box_w = 8, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 427, .adv_w = 102, .box_w = 6, .box_h = 7, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 433, .adv_w = 102, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 441, .adv_w = 91, .box_w = 5, .box_h = 7, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 446, .adv_w = 110, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 454, .adv_w = 58, .box_w = 1, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 456, .adv_w = 110, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 464, .adv_w = 145, .box_w = 7, .box_h = 2, .ofs_x = 1, .ofs_y = 3},
    {.bitmap_index = 466, .adv_w = 118, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 477, .adv_w = 118, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 488, .adv_w = 118, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 499, .adv_w = 118, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 510, .adv_w = 109, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 518, .adv_w = 109, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 526, .adv_w = 109, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 534, .adv_w = 51, .box_w = 2, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 537, .adv_w = 51, .box_w = 2, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 540, .adv_w = 136, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 551, .adv_w = 136, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 562, .adv_w = 136, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 573, .adv_w = 136, .box_w = 7, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 584, .adv_w = 126, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 593, .adv_w = 126, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 602, .adv_w = 105, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 613, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 620, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 627, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 634, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 641, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 648, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 655, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 662, .adv_w = 48, .box_w = 2, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 665, .adv_w = 48, .box_w = 2, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 668, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 675, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 682, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 689, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 696, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 703, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 710, .adv_w = 102, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 720, .adv_w = 118, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 731, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 738, .adv_w = 134, .box_w = 8, .box_h = 9, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 747, .adv_w = 110, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 755, .adv_w = 51, .box_w = 3, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 760, .adv_w = 48, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 764, .adv_w = 126, .box_w = 6, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 773, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 780, .adv_w = 158, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 790, .adv_w = 106, .box_w = 6, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 796, .adv_w = 148, .box_w = 8, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 806, .adv_w = 110, .box_w = 7, .box_h = 8, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 813, .adv_w = 118, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 823, .adv_w = 106, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 829, .adv_w = 118, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 841, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 848, .adv_w = 118, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 860, .adv_w = 106, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 869, .adv_w = 118, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 881, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 888, .adv_w = 118, .box_w = 8, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 901, .adv_w = 106, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 910, .adv_w = 118, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 922, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 929, .adv_w = 118, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 942, .adv_w = 106, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 950, .adv_w = 118, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 962, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 969, .adv_w = 118, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 981, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 988, .adv_w = 118, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1001, .adv_w = 106, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1009, .adv_w = 118, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1021, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1028, .adv_w = 118, .box_w = 7, .box_h = 14, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1041, .adv_w = 106, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1049, .adv_w = 109, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1056, .adv_w = 106, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1062, .adv_w = 109, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1071, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1078, .adv_w = 109, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1086, .adv_w = 106, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1093, .adv_w = 109, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1103, .adv_w = 106, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1112, .adv_w = 109, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1121, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1128, .adv_w = 109, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1138, .adv_w = 106, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1147, .adv_w = 109, .box_w = 5, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1156, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1163, .adv_w = 109, .box_w = 5, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1172, .adv_w = 106, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1180, .adv_w = 51, .box_w = 3, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1185, .adv_w = 48, .box_w = 3, .box_h = 10, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1189, .adv_w = 51, .box_w = 1, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1191, .adv_w = 48, .box_w = 1, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1193, .adv_w = 136, .box_w = 7, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1203, .adv_w = 106, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1209, .adv_w = 136, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1221, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1228, .adv_w = 136, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1240, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1247, .adv_w = 136, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1259, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1266, .adv_w = 136, .box_w = 7, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1278, .adv_w = 106, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1287, .adv_w = 136, .box_w = 7, .box_h = 14, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1300, .adv_w = 106, .box_w = 5, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1307, .adv_w = 136, .box_w = 7, .box_h = 14, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1320, .adv_w = 106, .box_w = 5, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1328, .adv_w = 158, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1340, .adv_w = 106, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1348, .adv_w = 158, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1360, .adv_w = 106, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1368, .adv_w = 158, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1381, .adv_w = 106, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1390, .adv_w = 158, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1402, .adv_w = 106, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1410, .adv_w = 158, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1422, .adv_w = 106, .box_w = 6, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1430, .adv_w = 126, .box_w = 6, .box_h = 11, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1439, .adv_w = 110, .box_w = 5, .box_h = 9, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1445, .adv_w = 126, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1455, .adv_w = 110, .box_w = 5, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1462, .adv_w = 148, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1474, .adv_w = 110, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1483, .adv_w = 148, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1495, .adv_w = 110, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1504, .adv_w = 148, .box_w = 8, .box_h = 13, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1517, .adv_w = 110, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1526, .adv_w = 148, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1538, .adv_w = 110, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = 0},
    {.bitmap_index = 1547, .adv_w = 148, .box_w = 8, .box_h = 12, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1559, .adv_w = 110, .box_w = 7, .box_h = 10, .ofs_x = 1, .ofs_y = -2},
    {.bitmap_index = 1568, .adv_w = 105, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1579, .adv_w = 102, .box_w = 6, .box_h = 13, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1589, .adv_w = 105, .box_w = 7, .box_h = 11, .ofs_x = 0, .ofs_y = -2},
    {.bitmap_index = 1599, .adv_w = 102, .box_w = 6, .box_h = 10, .ofs_x = 0, .ofs_y = -3},
    {.bitmap_index = 1607, .adv_w = 105, .box_w = 7, .box_h = 13, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1619, .adv_w = 102, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -3},
    {.bitmap_index = 1629, .adv_w = 105, .box_w = 7, .box_h = 12, .ofs_x = 0, .ofs_y = 0},
    {.bitmap_index = 1640, .adv_w = 102, .box_w = 6, .box_h = 13, .ofs_x = 1, .ofs_y = -3}
};

/*---------------------
 *  CHARACTER MAPPING
 *--------------------*/

static const uint16_t unicode_list_4[] = {
    0x0, 0x1, 0x2, 0x3, 0x8, 0x9, 0xa, 0xc,
    0xd, 0x12, 0x13, 0x14, 0x15, 0x19, 0x1a, 0x1d,
    0x20, 0x21, 0x22, 0x23, 0x28, 0x29, 0x2a, 0x2c,
    0x2d, 0x32, 0x33, 0x34, 0x35, 0x39, 0x3a, 0x3d,
    0x42, 0x43, 0x50, 0x51, 0x68, 0x69, 0xa8, 0xa9,
    0xe0, 0xe1, 0xef, 0xf0
};

/*Collect the unicode lists and glyph_id offsets*/
static const lv_font_fmt_txt_cmap_t cmaps[] =
{
    {
        .range_start = 32, .range_length = 16, .glyph_id_start = 1,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 58, .range_length = 33, .glyph_id_start = 17,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 92, .range_length = 1, .glyph_id_start = 50,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 94, .range_length = 33, .glyph_id_start = 51,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    },
    {
        .range_start = 192, .range_length = 241, .glyph_id_start = 84,
        .unicode_list = unicode_list_4, .glyph_id_ofs_list = NULL, .list_length = 44, .type = LV_FONT_FMT_TXT_CMAP_SPARSE_TINY
    },
    {
        .range_start = 7840, .range_length = 90, .glyph_id_start = 128,
        .unicode_list = NULL, .glyph_id_ofs_list = NULL, .list_length = 0, .type = LV_FONT_FMT_TXT_CMAP_FORMAT0_TINY
    }
};



/*--------------------
 *  ALL CUSTOM DATA
 *--------------------*/

#if LVGL_VERSION_MAJOR == 8
/*Store all the custom data of the font*/
static  lv_font_fmt_txt_glyph_cache_t cache;
#endif

#if LVGL_VERSION_MAJOR >= 8
static const lv_font_fmt_txt_dsc_t font_dsc = {
#else
static lv_font_fmt_txt_dsc_t font_dsc = {
#endif
    .glyph_bitmap = glyph_bitmap,
    .glyph_dsc = glyph_dsc,
    .cmaps = cmaps,
    .kern_dsc = NULL,
    .kern_scale = 0,
    .cmap_num = 6,
    .bpp = 1,
    .kern_classes = 0,
    .bitmap_format = 0,
#if LVGL_VERSION_MAJOR == 8
    .cache = &cache
#endif

};



/*-----------------
 *  PUBLIC FONT
 *----------------*/

/*Initialize a public general font descriptor*/
#if LVGL_VERSION_MAJOR >= 8
const lv_font_t dejavu_sans_condensed_12 = {
#else
lv_font_t dejavu_sans_condensed_12 = {
#endif
    .get_glyph_dsc = lv_font_get_glyph_dsc_fmt_txt,    /*Function pointer to get glyph's data*/
    .get_glyph_bitmap = lv_font_get_bitmap_fmt_txt,    /*Function pointer to get glyph's bitmap*/
    .line_height = 17,          /*The maximum line height required by the font*/
    .base_line = 3,             /*Baseline measured from the bottom of the line*/
#if !(LVGL_VERSION_MAJOR == 6 && LVGL_VERSION_MINOR == 0)
    .subpx = LV_FONT_SUBPX_NONE,
#endif
#if LV_VERSION_CHECK(7, 4, 0) || LVGL_VERSION_MAJOR >= 8
    .underline_position = -1,
    .underline_thickness = 1,
#endif
    .static_bitmap = 0,
    .dsc = &font_dsc,          /*The custom font data. Will be accessed by `get_glyph_bitmap/dsc` */
#if LV_VERSION_CHECK(8, 2, 0) || LVGL_VERSION_MAJOR >= 9
    .fallback = NULL,
#endif
    .user_data = NULL,
};



#endif /*#if DEJAVU_SANS_CONDENSED_12*/
