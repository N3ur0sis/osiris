#ifndef INCLUDE_FRAMEBUFFER_H
#define INCLUDE_FRAMEBUFFER_H

/** fb_write_cell:
 *  Writes a character with the given foreground and background to position i
 *  in the framebuffer.
 */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);

/** fb_move_cursor:
 *  Moves the cursor of the framebuffer to the given position
 */
void fb_move_cursor(unsigned short pos);

/** fb_write:
 *  Writes a buffer of characters to the screen and advances the cursor.
 */
int fb_write(char *buf, unsigned int len);

#endif
