#include<stdio.h>
#include<stdlib.h>

#include <window.h>
#include <monster.h>
#include <sprite.h>

struct monster{
    int x,y;
    enum direction direction;
    char alive;
};

// monster Getters&Seters

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

void monster_set_status(struct monster *monster, char status){
    monster->alive = status;
}

char monster_get_status(struct monster *monster){
    return monster->alive;
}

void monster_set_direction(struct monster *monster, enum direction direction){
    monster->direction = direction;
}

enum direction monster_get_direction(struct monster *monster){
    return monster->direction;
}

//Monsters

struct monster *monsters_alloc(int n){
    return malloc(n*sizeof(struct monster));
}

void monsters_free(struct monster *Monsters){
    free(Monsters);
}

struct monster* monsters_get_by_index(struct monster *Monsters, int i){
    return Monsters+i;
}

void monsters_display(struct monster *Monsters, int nbr_Monsters){
    for(int i=0; i<nbr_Monsters; i++){
        if(Monsters[i].alive)
            window_display_image(sprite_get_monster(Monsters[i].direction), Monsters[i].x * SIZE_BLOC, Monsters[i].y * SIZE_BLOC);
    }
}

void monster_step(struct monster *monster, enum direction direction){
    int dx=0;
    int dy=0;

    switch (direction){
        case NORTH:
            dy=1;
            break;
        
        case EAST:
            dx=1;
            break;
        
        case SOUTH:
            dx=-1;
            break;
        
        case WEST:
            dy=-1;
            break;
    }

    monster->x+=dx;
    monster->y+=dy;
}