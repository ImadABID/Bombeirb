#include <player.h>
#include <map.h>
#include <game.h>

struct bomb;
void bomb_init();
void bomb_tick(struct game* game);
void bomb_place(struct player* player, struct map* map);
void bomb_explode(struct bomb bomb, struct game* game);
int bomb_get_sprite(int x, int y);
int explosion_tick(int x, int y);
int explose_cell(struct map* map, int x, int y, int face);
int box_bonus(int x, int y);
