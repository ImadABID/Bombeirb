#include <map.h>
#include <player.h>
#include <window.h>

int bomb_place(struct player* player, struct map* map){
  //La on pose une bombe
  map_set_cell_type(map, player_get_x(player),player_get_y(player), CELL_BOMB);
  return 0;
}
int bomb_tick(int x, int y){
  return SDL_GetTicks();
}
