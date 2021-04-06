#ifndef MONSTER_H_
#define MONSTER_H_

#include <stdio.h>

#include "constant.h"

struct monster;

//Sauvegarde / Chargement partie
void monster_save(struct monster *monster, FILE *f);
char monster_load(struct monster *monster, FILE *f);

// monster Getters&Seters

int monster_get_struct_size();

void monster_set_x(struct monster *monster, int x);
int monster_get_x(struct monster *monster);

void monster_set_y(struct monster *monster, int y);
int monster_get_y(struct monster *monster);

void monster_set_direction(struct monster *monster, enum direction direction);
enum direction monster_get_direction(struct monster *monster);

struct monster* monster_get_next_monster(struct monster *monster);
void monster_set_next_monster(struct monster *monster, struct monster *monster_next);

//monster
void monster_step(struct monster *monster, enum direction direction);

//monsters
struct monster *monsters_alloc(int n);
struct monster *monsters_get_by_index(struct monster *Monsters, int i);
void monsters_display(struct monster *monster_list);

//List
struct monster *monster_list_new();
void monster_list_free(struct monster * ml);

struct monster* monster_list_get(struct monster *ml);
struct monster* monster_list_read(struct monster *ml);

void monster_list_append(struct monster *ml, struct monster *m);
void monster_list_delet(struct monster *ml, struct monster *m);

char monster_list_empty(struct monster *monster_list);
int monster_list_lenght(struct monster *ml);

struct monster *monster_list_get_by_index(struct monster *monster_list, int i);
struct monster *monster_list_get_by_position(struct monster *monster_list, int x, int y);

#endif