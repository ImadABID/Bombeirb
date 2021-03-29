#ifndef MONSTER_H_
#define MONSTER_H_

#include <stdio.h>

#include <constant.h>

struct monster;

//Sauvegarde / Chargement partie
void monster_save(struct monster *monster, FILE *f);
void monster_load(struct monster *monster, FILE *f);

// monster Getters&Seters

int monster_get_struct_size();

void monster_set_x(struct monster *monster, int x);
int monster_get_x(struct monster *monster);

void monster_set_y(struct monster *monster, int y);
int monster_get_y(struct monster *monster);

void monster_set_status(struct monster *monster, char status);
char monster_get_status(struct monster *monster);

void monster_set_direction(struct monster *monster, enum direction direction);
enum direction monster_get_direction(struct monster *monster);

//monster
void monster_step(struct monster *monster, enum direction direction);

//monsters
struct monster *monsters_alloc(int n);
struct monster *monsters_get_by_index(struct monster *Monsters, int i);
void monsters_free(struct monster *Monsters);
void monsters_display(struct monster *Monsters, int nbr_Monsters);

#endif