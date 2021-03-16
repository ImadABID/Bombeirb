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

//La map ou chaque case represente
//le life time des explosions
int explosionList[12][12];

//La map ou chaque case represente
//le bonus présent sur la case
int bonusMap[12][12];

//C'est pas très propre comme maniere de coder
//ça augmente la complexité en mémoire
//mais ça baisse la complexité en temps
//et l'accès est + pratique que faire une boucle
//jusqu'a trouver le match des coords x y
//voila


//Les lifetime en tick
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

//Explosion d'une case
int explose_cell(struct map* map, int x, int y, int face){
	if(map_is_inside(map, x, y)){
		if(map_get_cell_type(map,x, y) != CELL_EMPTY){
			face = 0;
		}

		if(map_get_cell_type(map,x, y) == CELL_BOX){
			//dropper caisse dans joueur
			bonusMap[x][y] = map_get_cell(map,x,y)-CELL_BOX;
			map_set_cell_type(map,x, y, CELL_EXPLOSION);
			explosionList[x][y] = explosion_time;
		}

	//si caisse alors face = 0
	//donc si vide:
	if(face){
		bonusMap[x][y] = 0;
		map_set_cell_type(map,x, y, CELL_EXPLOSION);
		explosionList[x][y] = explosion_time;
	}
	}
	return face;
}

//gere l'explosion d'une bombe
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
		north = explose_cell(map, bomb.x, bomb.y-i, north);
		south = explose_cell(map, bomb.x, bomb.y+i, south);
		east = explose_cell(map, bomb.x+i, bomb.y, east);
		west = explose_cell(map, bomb.x-i, bomb.y, west);
	}

}

//Fonction degueu mais ca fait le taff
int explosion_tick(int x, int y){
	return explosionList[x/SIZE_BLOC][y/SIZE_BLOC]--;
}

int box_bonus(int x, int y){
	return bonusMap[x][y];
}


//tick tte les bombes
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
