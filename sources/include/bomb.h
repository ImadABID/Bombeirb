#include <player.h>
#include <map.h>

struct bomb;
int bomb_tick(int x, int y);
int bomb_place(struct player* player, struct map* map);
int bomb_explode(int x, int y);
