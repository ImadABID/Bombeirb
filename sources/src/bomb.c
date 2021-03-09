#include <map.h>
#include <player.h>
#include <window.h>
#include <game.h>
#include <constant.h>

struct bomb {
	int x;
	int y;
  int time;
};



struct bomb bombList[9];
int explosionList[12][12];

int explosion_time = 20;
int bomb_time = 100;

//init les bombes
void bomb_init(){
  for (int i = 0; i < 9; i++) {
    bombList[i].time = -1;
		bombList[i].x = -1;
		bombList[i].y = -1;
  }
}

int bomb_get_sprite(int x, int y){
	x = x/40;
	y = y/40;


	for (int i = 0; i < 9; i++) {
		if(bombList[i].x == x && bombList[i].y == y){
			return 4*bombList[i].time/(bomb_time+1);
		}
	}

	return 0;
}


void bomb_place(struct player* player, struct map* map){
  //La on pose une bombe

  //Verifier que c pas deja une bombe
	if(map_get_cell_type(map, player_get_x(player),player_get_y(player)) == CELL_EMPTY && player_get_nb_bomb(player) > 0){

		player_dec_nb_bomb(player);
		for (int i = 0; i < 9; i++) {
	    if(bombList[i].time == -1){
	      bombList[i].x = player_get_x(player);
	      bombList[i].y = player_get_y(player);
	      bombList[i].time = bomb_time;
				break;
			}
	  }
		map_set_cell_type(map, player_get_x(player),player_get_y(player), CELL_BOMB);
	}

}

void bomb_explode(struct bomb bomb, struct game* game){
	int r = player_get_range(game_get_player(game));
	struct map* map = game_get_current_map(game);

	explosionList[bomb.x][bomb.y] = explosion_time;
	map_set_cell_type(map,bomb.x, bomb.y, CELL_EXPLOSION);
	int north = 1;
	int south = 1;
	int east = 1;
	int west = 1;

	for (int i = 1; i < r; i++) {
		if(!map_is_inside(map, bomb.x, bomb.y-i) || map_get_cell_type(map,bomb.x, bomb.y-i) != CELL_EMPTY ){

			//si une caisse, faire peter la caisse quand meme
			north = 0;

		}
		if(!map_is_inside(map, bomb.x, bomb.y+i) || map_get_cell_type(map,bomb.x, bomb.y+i) != CELL_EMPTY ){
			south = 0;
		}
		if(!map_is_inside(map, bomb.x+i, bomb.y) || map_get_cell_type(map,bomb.x+i, bomb.y) != CELL_EMPTY ){
			east = 0;
		}
		if(!map_is_inside(map, bomb.x-i, bomb.y) || map_get_cell_type(map,bomb.x-i, bomb.y) != CELL_EMPTY ){
			west = 0;
		}

		printf("c x%i x%i  y%i y%i\n", bomb.x-i, bomb.x+i, bomb.y-i, bomb.y+i);
		if(north){
			map_set_cell_type(map,bomb.x, bomb.y-i, CELL_EXPLOSION);
			explosionList[bomb.x][bomb.y-i] = explosion_time;
		}
		if(south){
			map_set_cell_type(map,bomb.x, bomb.y+i, CELL_EXPLOSION);
			explosionList[bomb.x][bomb.y+i] = explosion_time;
		}
		if(east){
			map_set_cell_type(map,bomb.x+i, bomb.y, CELL_EXPLOSION);
			explosionList[bomb.x+i][bomb.y] = explosion_time;
		}
		if(west){
			map_set_cell_type(map,bomb.x-i, bomb.y, CELL_EXPLOSION);
			explosionList[bomb.x-i][bomb.y] = explosion_time;
		}
	}

}


//Fonction degueu mais ca fait le taff
int explosion_tick(int x, int y){
	return explosionList[x/SIZE_BLOC][y/SIZE_BLOC]--;
}


void bomb_tick(struct game* game){
	for (int i = 0; i < 9; i++) {
		if(bombList[i].time>0){
			bombList[i].time--;
		}

		if(bombList[i].time == 0){
			bombList[i].time = -1;
			bomb_explode(bombList[i], game);
		}
	}
}
