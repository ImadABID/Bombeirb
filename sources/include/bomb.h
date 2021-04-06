#include <player.h>
#include <map.h>
#include <game.h>

void bomb_tick(struct game* game);
void bomb_place(struct player* player, struct map* map);
void bomb_explode(int x, int y, struct game* game);
int bomb_get_sprite(struct map* map, int x, int y);
int explosion_tick(int x, int y, struct map* map);
int explose_cell(struct map* map, int x, int y, int face);
int box_bonus(int x, int y);
