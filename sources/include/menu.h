#ifndef __MENU_H__
#define __MENU_H__

void menu_load();
void menu_unload();

char menu_display(char *message, char **menu_option, unsigned char nbr_options);

void menu_free_options_ttf(SDL_Surface **surf, unsigned char nbr_options);

#endif