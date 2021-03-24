/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#include <SDL/SDL_image.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <map.h>
#include <constant.h>
#include <misc.h>
#include <sprite.h>
#include <window.h>
#include <bomb.h>
#include <monster.h>
#include <str_formating.h>

struct map {
	int width;
	int height;
	unsigned char* grid;
	struct monster *Monsters;
	int nbr_Monsters;
	double Monsters_movement_probability; //speed;
};

#define CELL(i,j) ( (i) + (j) * map->width)

struct map* map_new(int width, int height)
{
	assert(width > 0 && height > 0);

	struct map* map = malloc(sizeof *map);
	if (map == NULL )
		error("map_new : malloc map failed");

	map->width = width;
	map->height = height;

	map->grid = malloc(height * width);
	if (map->grid == NULL) {
		error("map_new : malloc grid failed");
	}

	// Grid cleaning
	int i, j;
	for (i = 0; i < width; i++)
	  for (j = 0; j < height; j++)
	    map->grid[CELL(i,j)] = CELL_EMPTY;

	return map;
}

int map_is_inside(struct map* map, int x, int y)
{
	assert(map);
	return x >= 0 && y >= 0 && x < map_get_width(map) && y < map_get_height(map);
}

void map_free(struct map *map)
{
	if (map == NULL )
		return;
	free(map->grid);
	monsters_free(map->Monsters);
	free(map);
}

int map_get_width(struct map* map)
{
	assert(map != NULL);
	return map->width;
}

int map_get_height(struct map* map)
{
	assert(map);
	return map->height;
}

int map_get_cell(struct map* map, int x, int y){
	return map->grid[CELL(x,y)];
}

enum cell_type map_get_cell_type(struct map* map, int x, int y)
{
	assert(map && map_is_inside(map, x, y));
	return map->grid[CELL(x,y)] & 0xf0;
}

void map_set_cell_type(struct map* map, int x, int y, enum cell_type type)
{
	assert(map && map_is_inside(map, x, y));
	map->grid[CELL(x,y)] = type;
}

void display_bonus(struct map* map, int x, int y, unsigned char type)
{
	// bonus is encoded with the 4 most significant bits
	switch (type & 0x0f) {
	case BONUS_BOMB_RANGE_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_INC), x, y);
		break;

	case BONUS_BOMB_RANGE_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_RANGE_DEC), x, y);
		break;

	case BONUS_BOMB_NB_DEC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_DEC), x, y);
		break;

	case BONUS_BOMB_NB_INC:
		window_display_image(sprite_get_bonus(BONUS_BOMB_NB_INC), x, y);
		break;
	case BONUS_LIFE:
		window_display_image(sprite_get_bonus(BONUS_LIFE), x, y);
		break;
	}
}

void display_scenery(struct map* map, int x, int  y, unsigned char type)
{
	switch (type & 0x0f) { // sub-types are encoded with the 4 less significant bits
	case SCENERY_STONE:
		window_display_image(sprite_get_stone(), x, y);
		break;
	case SCENERY_EXPLOSION:
		//les explosions degage apres x tick
		if(explosion_tick(x,y) == 0){
			int bonus = box_bonus(x/SIZE_BLOC, y/SIZE_BLOC);
			if(bonus == 0){
				map_set_cell_type(map,x/SIZE_BLOC, y/SIZE_BLOC, CELL_EMPTY);
			} else {
				map_set_cell_type(map,x/SIZE_BLOC, y/SIZE_BLOC, CELL_BONUS + bonus);
			}
		}

		window_display_image(sprite_get_explosion(), x, y);
		break;
	case SCENERY_TREE:
		window_display_image(sprite_get_tree(), x, y);
		break;
	case SCENERY_PRINCESS:
		window_display_image(sprite_get_princess(), x, y);
		break;
	}
}

void display_door(struct map* map, int x, int y, unsigned char type){
	switch (type & 0x0f){
	case DOOR_CLOSED:
		window_display_image(sprite_get_door_closed(), x, y);
		break;
	
	default:
		window_display_image(sprite_get_door_opened(), x, y);
		break;
	}
}

void map_display(struct map* map)
{
	assert(map != NULL);
	assert(map->height > 0 && map->width > 0);

	int x, y;
	for (int i = 0; i < map->width; i++) {
	  for (int j = 0; j < map->height; j++) {
	    x = i * SIZE_BLOC;
	    y = j * SIZE_BLOC;

	    unsigned char type = map->grid[CELL(i,j)];

	    switch (type & 0xf0) {
		case CELL_SCENERY:
		  display_scenery(map, x, y, type);
		  break;
	    case CELL_BOX:
	      window_display_image(sprite_get_box(), x, y);
	      break;
		case CELL_BOMB:
			window_display_image(sprite_get_bomb(bomb_get_sprite(x,y)), x, y);
			break;
	    case CELL_BONUS:
	      display_bonus(map, x, y, type);
	      break;
	    case CELL_KEY:
	      window_display_image(sprite_get_key(), x, y);
	      break;
	    case CELL_DOOR:
		  display_door(map, x, y, type);
	      break;
	    }
	  }
	}

	map_update_monsters(map, map->Monsters, map->nbr_Monsters);
	monsters_display(map->Monsters, map->nbr_Monsters);
}

struct map* map_get_static(void)
{
	struct map* map = map_new(STATIC_MAP_WIDTH, STATIC_MAP_HEIGHT);

	unsigned char themap[STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT] = {
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
	  CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_BOX, CELL_EMPTY, CELL_EMPTY, CELL_BOMB, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_BOX, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_STONE, CELL_STONE, CELL_EMPTY, CELL_EMPTY, CELL_STONE, CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY , CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_TREE, CELL_BOX, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_TREE, CELL_TREE, CELL_TREE, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,  CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_STONE,  CELL_EMPTY, CELL_EMPTY,
	  CELL_BOX, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE, CELL_STONE,  CELL_BOX_LIFE, CELL_EMPTY,
	  CELL_BOX,  CELL_EMPTY, CELL_DOOR, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY, CELL_EMPTY,
		};

	for (int i = 0; i < STATIC_MAP_WIDTH * STATIC_MAP_HEIGHT; i++)
		map->grid[i] = themap[i];

	return map;
}

struct map* map_get(char *map_prefix, int level, int nbr_levels){

	char *map_path = malloc(10*sizeof(char));
	map_path = strcpy(map_path,"map/");
	char *level_str = malloc(10*sizeof(char));
	sprintf(level_str,"%d",level);
	map_path = strcat(map_path,map_prefix);
	map_path = strcat(map_path,"_");
	map_path = strcat(map_path,level_str);
	free(level_str);

	FILE *f = fopen(map_path,"r");
	if(f==NULL){
		fprintf(stderr,"Error : %s not founded\n",map_path);
		free(map_path);
		exit(EXIT_FAILURE);
	}
	free(map_path);

	int width, height;
	char *rest = malloc(10*sizeof(char));
	fscanf(f,"%s",rest);
	int *dim = str_format_to_int(2, rest);
	width = dim[0];
	height = dim[1];
	free(dim);


	struct map* map = map_new(width, height);

	for(int i=0;i<width*height;i++){
			char *str_info=malloc(3);
			fscanf(f,"%s",str_info);
			map->grid[i] = (char)atoi(str_info);
			free(str_info);
	}

    fclose(f);

	// Generating Monsters
	map->nbr_Monsters = level+1;
	map->Monsters = map_generate_monsters_randomly(map->nbr_Monsters, map);
	map->Monsters_movement_probability = ((double) (level+1)) / ((double) nbr_levels);
	// le coefficient va etre choisi en fonction du prefix de la map

	return map;
}

struct monster *map_generate_monsters_randomly(int n, struct map *map){
	struct monster *Monsters = monsters_alloc(n);

	srand(time(0));
    for(int i=0; i<n; i++){
		struct monster *monster = monsters_get_by_index(Monsters, i);
		monster_set_status(monster, 1);
		monster_set_direction(monster, (int) ((double) rand()/RAND_MAX*3+0.5));
		monster_set_x(monster, (int) ((double) rand()/RAND_MAX*(map_get_width(map)-1)+0.5));
		monster_set_y(monster, (int) ((double) rand()/RAND_MAX*(map_get_height(map)-1)+0.5));
        while(!map_accept_monster(monster, map)){
            monster_set_x(monster, (int) ((double) rand()/RAND_MAX*(map_get_width(map)-1)+0.5));
			monster_set_y(monster, (int) ((double) rand()/RAND_MAX*(map_get_height(map)-1)+0.5));
        }
        map_set_cell_type(map, monster_get_x(monster), monster_get_y(monster), CELL_MONSTER);
    }

    return Monsters;
}

char map_accept_monster(struct monster *monster, struct map *map){
	int x = monster_get_x(monster);
	int y = monster_get_y(monster);

	if( x<0 || y<0 || x>=map->width || y>=map->height )
		return 0;

    char yes = map_get_cell_type(map, x, y) == CELL_EMPTY;
	
	if(x-1>=0){
		if(y-1>=0)
			yes = yes && map_get_cell_type(map, x-1,y-1) != CELL_DOOR;
		
		yes = yes && map_get_cell_type(map, x-1,y) != CELL_DOOR;

		if(y+1<map->height)
			yes = yes && map_get_cell_type(map, x-1,y+1) != CELL_DOOR;
	}

	if(y-1>=0)
		yes = yes && map_get_cell_type(map, x,y-1) != CELL_DOOR;

	if(y+1<map->height)
		yes = yes && map_get_cell_type(map, x,y+1) != CELL_DOOR;


	if(x+1<map->width){
		if(y-1>=0)
			yes = yes && map_get_cell_type(map, x+1,y-1) != CELL_DOOR;

		yes = yes && map_get_cell_type(map, x+1,y) != CELL_DOOR;

		if(y+1<map->height)
			yes = yes && map_get_cell_type(map, x+1,y+1) != CELL_DOOR;
	}

    return yes;
}

void map_update_monsters(struct map *map, struct monster *Monsters, int nbr_Monsters){
	map_monsters_group_movement_manager(map, Monsters, nbr_Monsters, map_move_monster_randomly, NULL);
}

void map_monsters_group_movement_manager(
	struct map *map,
	struct monster *Monsters,
	int nbr_Monsters,
	void monster_move_func(struct map *, struct monster *),
	void *(monsters_planning_func)(struct map *map, struct monster *Monsters, int nbr_Monsters))
{
	const unsigned int TminToMove = (int) 20/nbr_Monsters;
	static unsigned int t = 0;

	double monster_to_move_index;
	double u = (double) rand()/RAND_MAX;

	if(t<TminToMove){
		t++;
		return;
	}

	if(u<map->Monsters_movement_probability){
		if(monsters_planning_func != NULL){
			monsters_planning_func(map, Monsters, nbr_Monsters);
		}
		t=0;
		monster_to_move_index = (int) (((double) rand()) / ((double) RAND_MAX) * ( (double) map->nbr_Monsters-1) + 0.5 );
		struct monster *monster = monsters_get_by_index(Monsters, monster_to_move_index);
		if(monster_get_status(monster)){
			monster_move_func(map, monster);
		}
	}
}

void map_move_monster_randomly(struct map *map, struct monster *monster){
	double p_stay = 0.2;
	double p_same_direction = 0.2;

	int last_x,last_y;
	double u = (double) rand()/RAND_MAX;

	if(u<p_stay){
		return ;
	}else if(u>=p_stay && u < p_stay + p_same_direction){
		last_x = monster_get_x(monster);
		last_y = monster_get_y(monster);
		monster_step(monster, monster_get_direction(monster));
		if(map_accept_monster(monster, map)){
			map_set_cell_type(map, last_x, last_y, CELL_EMPTY);
			map_set_cell_type(map, monster_get_x(monster), monster_get_y(monster), CELL_MONSTER);
		}else{
			monster_set_x(monster, last_x);
			monster_set_y(monster, last_y);
			map_move_monster_randomly(map, monster);
		}
	}else{
		last_x = monster_get_x(monster);
		last_y = monster_get_y(monster);
		monster_set_direction(monster, (int) ((double) rand()/RAND_MAX*3+0.5));
		monster_step(monster, monster_get_direction(monster));
		if(map_accept_monster(monster, map)){
			map_set_cell_type(map, last_x, last_y, CELL_EMPTY);
			map_set_cell_type(map, monster_get_x(monster), monster_get_y(monster), CELL_MONSTER);
		}else{
			monster_set_x(monster, last_x);
			monster_set_y(monster, last_y);
			map_move_monster_randomly(map, monster);
		}
	}
}