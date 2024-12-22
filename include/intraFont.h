/*
 * intraFont.h
 * This file is used to display the PSP's internal font (pgf and bwfon firmware files)
 * intraFont Version 0.31 by BenHur - http://www.psp-programming.com/benhur
 *
 * Uses parts of pgeFont by InsertWittyName - http://insomniac.0x89.org
 * G-spec code by Geecko
 *
 * This work is licensed under the Creative Commons Attribution-Share Alike 3.0 License.
 * See LICENSE for more details.
 *
 */

#ifndef __INTRAFONT_H__
#define __INTRAFONT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "libccc.h"

/** @defgroup intraFont Font Library
 *  @{
 */

#define INTRAFONT_ADVANCE_H        0x00000000 //default: advance horizontaly from one char to the next
#define INTRAFONT_ADVANCE_V        0x00000100
#define INTRAFONT_ALIGN_LEFT       0x00000000 //default: left-align the text
#define INTRAFONT_ALIGN_CENTER     0x00000200
#define INTRAFONT_ALIGN_RIGHT      0x00000400
#define INTRAFONT_ALIGN_FULL       0x00000600 //full justify text to width set by intraFontSetTextWidth()
#define INTRAFONT_SCROLL_LEFT      0x00002000 //in intraFontPrintColumn if text does not fit text is scrolled to the left 
                                            //(requires redrawing at ~60 FPS with x position returned by previous call to intraFontPrintColumn())
#define INTRAFONT_SCROLL_SEESAW    0x00002200 //in intraFontPrintColumn if text does not fit text is scrolled left and right
#define INTRAFONT_SCROLL_RIGHT     0x00002400 //in intraFontPrintColumn if text does not fit text is scrolled to the right
#define INTRAFONT_SCROLL_THROUGH   0x00002600 //in intraFontPrintColumn if text does not fit text is scrolled through (to the left)
#define INTRAFONT_WIDTH_VAR        0x00000000 //default: variable-width
#define INTRAFONT_WIDTH_FIX        0x00000800 //set your custom fixed witdh to 24 pixels: INTRAFONT_WIDTH_FIX | 24 
                                              //(max is 255, set to 0 to use default fixed width, this width will be scaled by size)
#define INTRAFONT_ACTIVE           0x00001000 //assumes the font-texture resides inside sceGuTex already, prevents unecessary reloading -> very small speed-gain                     
#define INTRAFONT_DIRTY            0x00000001 //for desktop, assume texture needs upload to vram                     
#define INTRAFONT_CACHE_MED        0x00000000 //default: 256x256 texture (enough to cache about 100 chars)
#define INTRAFONT_CACHE_LARGE      0x00004000 //512x512 texture(enough to cache all chars of ltn0.pgf or ... or ltn15.pgf or kr0.pgf)
#define INTRAFONT_CACHE_ASCII      0x00008000 //try to cache all ASCII chars during fontload (uses less memory and is faster to draw text, but slower to load font)
                                              //if it fails: (because the cache is too small) it will automatically switch to chache on-the-fly with a medium texture
                              //if it succeeds: (all chars and shadows fit into chache) it will free some now unneeded memory
#define INTRAFONT_CACHE_ALL        0x0000C000 //try to cache all chars during fontload (uses less memory and is faster to draw text, but slower to load font)
                                              //if it fails: (because the cache is too small) it will automatically switch to chache on-the-fly with a large texture
                            //if it succeeds: (all chars and shadows fit into chache) it will free some now unneeded memory
#define INTRAFONT_STRING_ASCII     (0x00010000*CCC_CP000)  //default: interpret strings as ascii text (ISO/IEC 8859-1)
#define INTRAFONT_STRING_CP437     (0x00010000*CCC_CP437)  //interpret strings as ascii text (codepage 437)
#define INTRAFONT_STRING_CP850     (0x00010000*CCC_CP850)  //interpret strings as ascii text (codepage 850)
#define INTRAFONT_STRING_CP866     (0x00010000*CCC_CP866)  //interpret strings as ascii text (codepage 866)
#define INTRAFONT_STRING_SJIS      (0x00010000*CCC_CP932)  //interpret strings as shifted-jis (used for japanese)
#define INTRAFONT_STRING_GBK       (0x00010000*CCC_CP936)  //interpret strings as GBK (used for simplified chinese)
#define INTRAFONT_STRING_KOR       (0x00010000*CCC_CP949)  //interpret strings as Korean codepage 949
#define INTRAFONT_STRING_BIG5      (0x00010000*CCC_CP950)  //interpret strings as BIG5 (used for traditional chinese)
#define INTRAFONT_STRING_CP1251    (0x00010000*CCC_CP1251) //interpret strings as ascii text (codepage windows-1251)
#define INTRAFONT_STRING_CP1252    (0x00010000*CCC_CP1252) //interpret strings as ascii text (codepage windows-1252)
#define INTRAFONT_STRING_UTF8      (0x00010000*CCC_CPUTF8) //interpret strings as UTF-8

/** @note The following definitions are used internally by ::intraFont and have no other relevance.*/
#define FILETYPE_PGF      0x00
#define FILETYPE_BWFON    0x01
#define PGF_BMP_H_ROWS    0x01
#define PGF_BMP_V_ROWS    0x02
#define PGF_BMP_OVERLAY   0x03
#define PGF_NO_EXTRA1     0x04
#define PGF_NO_EXTRA2     0x08
#define PGF_NO_EXTRA3     0x10
#define PGF_CHARGLYPH     0x20
#define PGF_SHADOWGLYPH   0x40 //warning: this flag is not contained in the metric header flags and is only provided for simpler call to intraFontGetGlyph - ONLY check with (flags & PGF_CHARGLYPH)
#define PGF_CACHED        0x80
#define PGF_WIDTH_MASK    0x000000FF
#define PGF_OPTIONS_MASK  0x00003FFF
#define PGF_ALIGN_MASK    0x00000600
#define PGF_SCROLL_MASK   0x00002600
#define PGF_CACHE_MASK    0x0000C000
#define PGF_STRING_MASK   0x00FF0000


/**
 * A Glyph struct
 *
 * @note This is used internally by ::intraFont and has no other relevance.
 */
typedef struct {
  uint16_t x;         //in pixels
  uint16_t y;         //in pixels
  uint8_t width;      //in pixels
  uint8_t height;     //in pixels
  int8_t left;                //in pixels
  int8_t top;                 //in pixels
  uint8_t flags;
  uint16_t shadowID;  //to look up in shadowmap
  int8_t advance;             //in quarterpixels
  uint32_t ptr;        //offset 
} Glyph;

typedef struct {
  uint16_t x;         //in pixels
  uint16_t y;         //in pixels
  uint8_t flags;
} GlyphBW;

/**
 * A PGF_Header struct
 *
 * @note This is used internally by ::intraFont and has no other relevance.
 */
typedef struct {
  uint16_t header_start;
  uint16_t header_len;
  int8_t pgf_id[4];
  uint32_t revision;
  uint32_t version;
  uint32_t charmap_len;
  uint32_t charptr_len;
  uint32_t charmap_bpe;
  uint32_t charptr_bpe;
  uint8_t junk00[21];
  uint8_t family[64];
  uint8_t style[64];
  uint8_t junk01[1];
  uint16_t charmap_min;
  uint16_t charmap_max;
  uint8_t junk02[50];
  uint32_t fixedsize[2];
  uint8_t junk03[14];
  uint8_t table1_len;
  uint8_t table2_len;
  uint8_t table3_len;
  uint8_t advance_len;
  uint8_t junk04[102];
  uint32_t shadowmap_len;
  uint32_t shadowmap_bpe;
  uint8_t junk05[4];
  uint32_t shadowscale[2];
  //currently no need ;
} PGF_Header;

typedef struct fontVertex fontVertex;

/**
 * A Font struct
 * Order is ruined but packs better - mrneo240
 */
typedef struct intraFont {
  char* filename;
  uint8_t* fontdata;
  
  uint8_t* texture;          /**< The bitmap data */
  uint32_t textureID;          /**< OpenGL texture id */
  uint32_t texWidth;           /**< Texture size (power2) */
  uint32_t texHeight;          /**< Texture height (power2) */  

  uint16_t* charmap_compr;   /**< Compression info on compressed charmap */  
  uint16_t* charmap;         /**< Character map */  
  Glyph* glyph;                    /**< Character glyphs */
  GlyphBW* glyphBW;
  Glyph* shadowGlyph;              /**<  Shadow glyph(s) */
  struct intraFont* altFont;
  fontVertex *v;
  uint32_t v_size;
  
  float size;
  uint32_t color;
  uint32_t shadowColor;
  float angle, Rsin, Rcos;                /**< For rotation */
  uint32_t options;

  int16_t isRotated;
  uint16_t texX;
  uint16_t texY;
  uint16_t texYSize;
  uint16_t n_chars;
  uint16_t n_shadows;

  uint8_t fileType;          /**< FILETYPE_PGF or FILETYPE_BWFON */
  
  int8_t advancex;                   /**< in quarterpixels */
  int8_t advancey;                   /**< in quarterpixels */
  uint8_t charmap_compr_len; /**< length of compression info */
  uint8_t shadowscale;       /**< shadows in pgf file (width, height, left and top properties as well) are scaled by factor of (shadowscale>>6) */  
} intraFont;


/**
 * Initialise the Font library
 *
 * @returns 1 on success.
 */
int intraFontInit(void);

/**
 * Shutdown the Font library
 */
void intraFontShutdown(void);

/**
 * Load a pgf font.
 *
 * @param filename - Path to the font
 *
 * @param  options - INTRAFONT_XXX flags as defined above including flags related to CACHE (ored together)
 *
 * @returns A ::intraFont struct
 */
intraFont* intraFontLoad(const char *filename,unsigned int options);

/**
 * Free the specified font.
 *
 * @param font - A valid ::intraFont
 */
void intraFontUnload(intraFont *font);

/**
 * Activate the specified font.
 *
 * @param font - A valid ::intraFont
 */
void intraFontActivate(intraFont *font);

/**
 * Set font style
 *
 * @param font - A valid ::intraFont
 *
 * @param size - Text size
 *
 * @param color - Text color
 *
 * @param angle - Text angle (in degrees)
 *
 * @param shadowColor - Shadow color (use 0 for no shadow)
 *
 * @param options - INTRAFONT_XXX flags as defined above except flags related to CACHE (ored together)
 */
void intraFontSetStyle(intraFont *font, float size, unsigned int color, unsigned int shadowColor, float angle, unsigned int options);

/**
 * Set type of string encoding to be used in intraFontPrint[f]
 *
 * @param font - A valid ::intraFont
 *
 * @param options - INTRAFONT_STRING_XXX flags as defined above except flags related to CACHE (ored together)
 */
void intraFontSetEncoding(intraFont *font, unsigned int options);

/**
 * Set alternative font
 *
 * @param font - A valid ::intraFont
 *
 * @param altFont - A valid ::intraFont that's to be used if font does not contain a character
 */
void intraFontSetAltFont(intraFont *font, intraFont *altFont);

/**
 * Draw UCS-2 encoded text along the baseline starting at x, y.
 *
 * @param font - A valid ::intraFont
 *
 * @param x - X position on screen
 *
 * @param y - Y position on screen
 *
 * @param width - column width for automatic line breaking (intraFontPrintColumn... versions only)
 *
 * @param text - UCS-2 encoded text to draw
 *
 * @param length - char length of text to draw (...Ex versions only)
 *
 * @returns The x position after the last char
 */
float intraFontPrintUCS2        (intraFont *font, float x, float y, const unsigned short *text);
float intraFontPrintUCS2Ex      (intraFont *font, float x, float y, const unsigned short *text, int length);
float intraFontPrintColumnUCS2  (intraFont *font, float x, float y, float width, const unsigned short *text);
float intraFontPrintColumnUCS2Ex(intraFont *font, float x, float y, float width, const unsigned short *text, int length);

/**
 * Draw text along the baseline starting at x, y.
 *
 * @param font - A valid ::intraFont
 *
 * @param x - X position on screen
 *
 * @param y - Y position on screen
 *
 * @param width - column width for automatic line breaking (intraFontPrintColumn... versions only)
 *
 * @param text - Text to draw (ASCII & extended ASCII, S-JIS or UTF-8 encoded)
 *
 * @param length - char length of text to draw (...Ex versions only)
 *
 * @returns The x position after the last char
 */
float intraFontPrint        (intraFont *font, float x, float y, const char *text);
float intraFontPrintEx      (intraFont *font, float x, float y, const char *text, int length);
float intraFontPrintColumn  (intraFont *font, float x, float y, float width, const char *text);
float intraFontPrintColumnEx(intraFont *font, float x, float y, float width, const char *text, int length);

/**
 * Draw text along the baseline starting at x, y (with formatting).
 *
 * @param font - A valid ::intraFont
 *
 * @param x - X position on screen
 *
 * @param y - Y position on screen
 *
 * @param width - column width for automatic line breaking (intraFontPrintfColumn... versions only)
 *
 * @param text - Text to draw (ASCII & extended ASCII, S-JIS or UTF-8 encoded)
 *
 * @param length - char length of text to draw (...Ex versions only)
 *
 * @returns The x position after the last char
 */
float intraFontPrintf        (intraFont *font, float x, float y, const char *text, ...);
//the following functions might be implemented in a future version of intraFont
//float intraFontPrintfEx      (intraFont *font, float x, float y, const char *text, int length, ...);
//float intraFontPrintfColumn  (intraFont *font, float x, float y, float width, const char *text, ...);
//float intraFontPrintfColumnEx(intraFont *font, float x, float y, float width, const char *text, int length, ...);

/**
 * Measure a length of text if it were to be drawn
 *
 * @param font - A valid ::intraFont
 *
 * @param text - Text to measure (ASCII & extended ASCII, S-JIS or UTF-8 encoded)
 *
 * @param length - char length of text to measure (...Ex version only)
 *
 * @returns The total width of the text (until the first newline char)
 */
float intraFontMeasureText  (intraFont *font, const char *text);
float intraFontMeasureTextEx(intraFont *font, const char *text, int length);

/**
 * Measure a length of UCS-2 encoded text if it were to be drawn
 *
 * @param font - A valid ::intraFont
 *
 * @param text - UCS-2 encoded text to measure
 *
 * @param length - char length of text to measure (...Ex version only)
 *
 * @returns The total width of the text (until the first newline char)
 */
float intraFontMeasureTextUCS2  (intraFont *font, const unsigned short *text); 
float intraFontMeasureTextUCS2Ex(intraFont *font, const unsigned short *text, int length); 

/** @} */

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // __INTRAFONT_H__
