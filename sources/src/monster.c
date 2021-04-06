#include<stdio.h>
#include<stdlib.h>

#include <window.h>
#include <monster.h>
#include <sprite.h>

struct monster{
    int x,y;
    enum direction direction;
    struct monster *next_monster;
};

// monster Getters&Seters

int monster_get_struct_size(){
    return sizeof(struct monster);
}

void monster_set_x(struct monster *monster, int x){
    monster->x = x;
}

int monster_get_x(struct monster *monster){
    return monster->x;
}

void monster_set_y(struct monster *monster, int y){
    monster->y = y;
}

int monster_get_y(struct monster *monster){
    return monster->y;
}

void monster_set_direction(struct monster *monster, enum direction direction){
    monster->direction = direction;
}

enum direction monster_get_direction(struct monster *monster){
    return monster->direction;
}

struct monster* monster_get_next_monster(struct monster *monster){
    return monster->next_monster;
}

void monster_set_next_monster(struct monster *monster,struct monster *monster_next){
    monster->next_monster = monster_next;
}

void monster_step(struct monster *monster, enum direction direction){
    int dx=0;
    int dy=0;

    switch (direction){
        case NORTH:
            dy=-1;
            break;
        
        case EAST:
            dx=1;
            break;
        
        case SOUTH:
            dy=1;
            break;
        
        case WEST:
            dx=-1;
            break;
    }

    monster->x+=dx;
    monster->y+=dy;
}

//Sauvegarde / Chargement partie
void monster_save(struct monster *monster, FILE *f){
    fwrite(&monster->x, sizeof(monster->x), 1, f);
    fwrite(&monster->y, sizeof(monster->y), 1, f);
    fwrite(&monster->direction, sizeof(monster->direction), 1, f);

    char is_it_the_last = 0;
    if(monster->next_monster==NULL)
        is_it_the_last = 1;
    fwrite(&is_it_the_last, sizeof(char), 1, f);
}

char monster_load(struct monster *monster, FILE *f){
    fread(&monster->x, sizeof(monster->x), 1, f);
    fread(&monster->y, sizeof(monster->y), 1, f);
    fread(&monster->direction, sizeof(monster->direction), 1, f);
    
    char is_it_the_last;
    fread(&is_it_the_last, sizeof(char), 1, f);
    
    return 1-is_it_the_last;
}

//Monster list
struct monster *monster_list_new(){
    struct monster *m=malloc(sizeof(struct monster));
    m->next_monster = NULL;
    return m;
}

void monster_list_free(struct monster * ml){
    if(ml->next_monster != NULL)
        monster_list_free(ml->next_monster);

    free(ml);
}

void monster_list_append(struct monster *ml, struct monster *m){
    struct monster *mn = malloc(sizeof(struct monster));
    mn->x=m->x;
    mn->y=m->y;
    mn->direction=m->direction;
    mn->next_monster = NULL;

    while(ml->next_monster!=NULL)
        ml = ml->next_monster;

    ml->next_monster = mn;
}

struct monster* monster_list_get(struct monster *ml){
    struct monster* m = ml->next_monster;
    ml->next_monster = m->next_monster;
    return m;
}

struct monster* monster_list_read(struct monster *ml){
    struct monster* m = ml->next_monster;
    return m;
}

char monster_list_empty(struct monster *monster_list){
    return monster_list->next_monster == NULL;
}

int monster_list_lenght(struct monster *ml){
    int i=0;
    while(ml->next_monster!=NULL){
        ml = ml->next_monster;
        i++;
    }
    return i;
}

struct monster *monster_list_get_by_index(struct monster *monster_list, int i){
    int j=0;
    while(monster_list->next_monster!=NULL){
        monster_list = monster_list->next_monster;
        if(i==j){
            return monster_list;
        }
        j++;
    }

    fprintf(stderr,"Index exceds monster list\n");
    exit(-1);
}

//Monsters

void monsters_display(struct monster *monster_list){
    struct monster *monster;
    while(!monster_list_empty(monster_list)){
        monster = monster_list_read(monster_list);
        monster_list = monster_list->next_monster;
        //printf("monsters_display : monster->direction = %d\n", monster->direction);
        window_display_image(sprite_get_monster(monster->direction), monster->x * SIZE_BLOC, monster->y * SIZE_BLOC);
    }
}