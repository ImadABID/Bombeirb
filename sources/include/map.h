/*******************************************************************************
 * This file is part of Bombeirb.
 * Copyright (C) 2018 by Laurent Réveillère
 ******************************************************************************/
#ifndef MAP_H_
#define MAP_H_

#include <stdio.h>

enum cell_type {
	CELL_EMPTY=0x00,   		//  0000 0000 //1 octet
	CELL_SCENERY=0x10, 		//  0001 0000
	CELL_BOX=0x20,   		//  0010 0000
	CELL_DOOR=0x30,      	//  0011 0000
	CELL_KEY=0x40,       	//  0100 0000
	CELL_BONUS=0x50, 		// 	0101 0000
	CELL_MONSTER=0x60, 		// 	0110 0000
	CELL_BOMB=0x70, 	   		// 	0111 0000
	CELL_EXPLOSION=0x80

};

enum bonus_type {
	BONUS_BOMB_RANGE_DEC=1,
	BONUS_BOMB_RANGE_INC,
	BONUS_BOMB_NB_DEC,
	BONUS_BOMB_NB_INC,
	BONUS_MONSTER,
	BONUS_LIFE
};

enum scenery_type {
	SCENERY_STONE = 1,    // 0001
	SCENERY_TREE  = 2,    // 0010
	SCENERY_EXPLOSION  = 3,    // 0011
	SCENERY_PRINCESS = 4  // 0100
};

enum door_type {
	DOOR_CLOSED			= 0,	// 0000
	DOOR_OPENED_NEXT	= 1,	// 0001
	DOOR_OPENED_PREV	= 2		// 0010
};

enum compose_type {
	CELL_TREE     = CELL_SCENERY | SCENERY_TREE,
	CELL_STONE    = CELL_SCENERY | SCENERY_STONE,
	CELL_PRINCESS = CELL_SCENERY | SCENERY_PRINCESS,

  CELL_BOX_RANGEINC = CELL_BOX | BONUS_BOMB_RANGE_DEC,
  CELL_BOX_RANGEDEC = CELL_BOX | BONUS_BOMB_RANGE_INC,
	CELL_BOX_BOMBINC  = CELL_BOX | BONUS_BOMB_NB_DEC,
  CELL_BOX_BOMBDEC  = CELL_BOX | BONUS_BOMB_NB_INC,
  CELL_BOX_LIFE     = CELL_BOX | BONUS_MONSTER,
  CELL_BOX_MONSTER  = CELL_BOX | BONUS_LIFE,
};

struct map;

// Create a new empty map
struct map* map_new(int width, int height);
void map_free(struct map* map);
void map_save(struct map* map, FILE *f);
struct map *map_load_progress(FILE *f);

struct map *map_get_by_index(struct map **Maps, int i);

// Return the height and width of a map
int map_get_width(struct map* map);
int map_get_height(struct map* map);

int map_get_cell(struct map* map, int x, int y);

// Return the type of a cell
enum cell_type map_get_cell_type(struct map* map, int x, int y);

// Set the type of a cell
void  map_set_cell_type(struct map* map, int x, int y, enum cell_type type);
void  map_set_cell(struct map* map, int x, int y, int cell);


// Test if (x,y) is within the map
int map_is_inside(struct map* map, int x, int y);

// Return a default static map
struct map* map_get_static();
struct map* map_get(char *map_prefix, int level, int nbr_levels);

// Display the map on the screen
void map_display(struct map* map);

//For monsters
struct monster *map_generate_monsters_randomly(int n, struct map *map);
char map_accept_monster(struct monster *monster, struct map *map);
void map_update_monsters(struct map *map, struct monster *monster_list);

//Monster life management
void map_add_monster_at(struct map *map, int x, int y);
void map_kill_the_monster_at(struct map *map, int x, int y);

//Monster movement manager
void map_monsters_group_movement_manager(
	struct map *map,
	struct monster *monster_list,
	void monster_move_func(struct map *, struct monster *),
	void *(monsters_planning_func)(struct map *map, struct monster *monster_list));

//Monters : Random strategy
void map_move_monster_randomly(struct map *map, struct monster *monster);

#endif /* MAP_H_ */
