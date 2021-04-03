#include <stdio.h>
#include <stdlib.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>

#include <menu.h>
#include <window.h>
#include <misc.h>
#include <constant.h>

#define BACKGROUND "menu/background.png"
#define FONT "menu/AlphaWood.ttf"
#define FONT_SIZE 28

SDL_Surface *background;
TTF_Font *police;

void menu_load(){
    background = image_load(BACKGROUND);
    police = TTF_OpenFont(FONT,FONT_SIZE);
}

void menu_unload(){
    SDL_FreeSurface(background);
    TTF_Quit();
}

char menu_display(char **menu_option, unsigned char nbr_options){
    static unsigned char cur = 0;
    SDL_Color default_color = {255, 255, 255};
    SDL_Color selecte_color = {100, 100, 255};

    int height = 524; //to change
    int width = 480; //to change

    int padding_vertical = (int) FONT_SIZE/2;
    //int y_start = (int) (height-(nbr_options*(padding_vertical+FONT_SIZE)-padding_vertical)) / 2;
    int y_start = (int) (0.6*height); 
    int x_start;

    while(1){
        int timer = SDL_GetTicks();
        SDL_Event event;
        window_clear();
    
        window_display_image(background, 0, 0);

        SDL_Surface *text[nbr_options];
        for(int i=0; i<nbr_options; i++){
            if(i == cur)
                text[i] = TTF_RenderText_Blended(police, menu_option[i], selecte_color);
            else
                text[i] = TTF_RenderText_Blended(police, menu_option[i], default_color);
            
            x_start = (int) (width - text[i]->w)/2;       

            window_display_image(text[i], x_start, y_start + i * (padding_vertical+FONT_SIZE));
        }
        window_refresh();

        while(SDL_PollEvent(&event)){
            switch (event.type){
                case SDL_QUIT:
                    return nbr_options;
                
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym){
                        case SDLK_ESCAPE:
				            return nbr_options;

                        case SDLK_DOWN:
                            cur++;
                            if(cur==nbr_options) cur=0;
                            break;
                        
                        case SDLK_UP:
                            if(cur==0) cur = nbr_options-1;
                            else cur--;
                            break;
                        
                        case SDLK_RETURN:
                            return cur;
                        
                        default:
				            break;
                    }
            }
        }

        int ideal_time = (int) 1000 / DEFAULT_GAME_FPS;
        int exe_time = SDL_GetTicks() - timer;
        if (exe_time < ideal_time)
			SDL_Delay(ideal_time - exe_time); // we are ahead of ideal time. let's wait.
    }
    return 1;
}