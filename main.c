#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<SDL2/SDL.h>
#include<stdbool.h>
#include<SDL2/SDL_mixer.h>
#include<SDL2/SDL_ttf.h>

#define FONT_PATH "Roboto.ttf" 
#define TABLE_SIZE 25
#define SCREEN_SIZE 500
#define CELL_SIZE 20

void generateTable(int (*table)[TABLE_SIZE]);
void printTable(SDL_Renderer *renderer, int (*table)[TABLE_SIZE]);
void clearScreen(SDL_Renderer *renderer);
void drawRect(SDL_Renderer *renderer, int pos_x, int pos_y);
void clearTable(int (*table)[TABLE_SIZE]);
void updateTable(int (*table)[TABLE_SIZE], int (*next)[TABLE_SIZE]);
int countNeighbors(int (*table)[TABLE_SIZE], int x, int y);
void drawText(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect);
void showGameOver(SDL_Renderer *renderer, TTF_Font *font);

int main() {
    int table[TABLE_SIZE][TABLE_SIZE] = {0};
    int next[TABLE_SIZE][TABLE_SIZE] = {0};
    int table_history[3][TABLE_SIZE][TABLE_SIZE] = {0};
    int neighbors = 0;
    int is_live = 0;
    int similar = 0;

    // init SDL
    SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDL_Window* window = NULL;
    // init Mixer
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music *bg_mus = Mix_LoadMUS("music/background.mp3");
    Mix_Chunk *loose = Mix_LoadWAV("music/lose.mp3");
    Mix_PlayMusic(bg_mus, -1);
    bool quit = false;
    SDL_Event e;
    window = SDL_CreateWindow
    (
        "The game of life",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_SIZE,
        SCREEN_SIZE,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED);
    // show text
    TTF_Init();
    TTF_Font *font = TTF_OpenFont(FONT_PATH, 56);

    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }


    generateTable(table);
    // table[0][0] = 0;
    // table[0][1] = 1;
    // table[0][2] = 0;
    // table[1][0] = 0;
    // table[1][1] = 0;
    // table[1][2] = 1;
    // table[2][0] = 1;
    // table[2][1] = 1;
    // table[2][2] = 1;
     for (int i = 0; i < TABLE_SIZE; i++)
        {
            for (int j = 0; j < TABLE_SIZE; j++)
            {
                printf("%d\t", table[i][j]);
            }
            printf("\n");
        }
    while( !quit ) {
        SDL_Delay(10);
        while( SDL_PollEvent( &e ) != 0 ) {
            if( e.type == SDL_QUIT ) {
                quit = true;
            }
        }

        // create new generation
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            for (int j = 0; j < TABLE_SIZE; j++)
            {
                if((j == 0) || (j == TABLE_SIZE - 1) || (i == 0) || (i == TABLE_SIZE - 1)) {
                    next[i][j] = table[i][j];
                }
                neighbors = countNeighbors(table, i, j);
                if(table[i][j] == 0) {
                    if ( neighbors == 3)
                    {
                        next[i][j] = 1;
                    }
                    else {
                        next[i][j] = 0;
                    }
                }
                if(table[i][j] == 1) {
                    if(neighbors >= 2 && neighbors <= 3) {
                        next[i][j] = 1;
                    }
                    else {
                        next[i][j] = 0;
                    }
                }
            }
        }

        // write history
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            for (int j = 0; j < TABLE_SIZE; j++)
            {
                // for (int k = 2; k > 1; k--)
                // {
                //     // table_history[k][i][j] = table_history[k-1][i][j];
                //     // printf("%d\n", k);
                // table_history[1][i][j] = table_history[0][i][j];
                // }
                table_history[2][i][j] = table_history[1][i][j];
                table_history[1][i][j] = table_history[0][i][j];
                table_history[0][i][j] = table[i][j];
            }
            
        }

        updateTable(table, next);

        // compare if table is null
        is_live = 0;
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            for (int j = 0; j < TABLE_SIZE; j++)
            {
                is_live += table[i][j];
                printf("%d\t", table[i][j]);
            }
            printf("\n");
        }

        if(is_live == 0) {
            showGameOver(renderer, font);
            SDL_Delay(3000);
            Mix_FreeMusic(bg_mus);
            Mix_PlayChannel(-1, loose, 0);
            printf("Game over\n");
            quit = true;
        }

        // for (int k = 1; k < HISTORY_LIMIT; k++)
        // {
        //     similar = 0;
            
        //     for (int i = 0; i < TABLE_SIZE; i++)
        //     {
        //         for (int j = 0; j < TABLE_SIZE; j++)
        //         {
        //             if(table_history[k][i][j] == table[i][j]) {
        //                 similar++;
        //             }
        //         }
        //     }
        //     printf("k=%d\n", k);
        //     printf("%d\n", similar);
        //     if(similar == TABLE_SIZE * TABLE_SIZE) {
        //         showGameOver(renderer, font);
        //         Mix_FreeMusic(bg_mus);
        //         Mix_PlayChannel(-1, loose, 0);
        //         SDL_Delay(3000);
        //         quit = true;
        //         break;
        //     }
        // }
        

        similar = 0;
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            for (int j = 0; j < TABLE_SIZE; j++)
            {
                if(table_history[1][i][j] == table[i][j]) {
                    similar++;
                }
            }
        }

        if(similar == TABLE_SIZE * TABLE_SIZE) {
            showGameOver(renderer, font);
            Mix_FreeMusic(bg_mus);
            Mix_PlayChannel(-1, loose, 0);
            SDL_Delay(3000);
            quit = true;
        }

        similar = 0;
        for (int i = 0; i < TABLE_SIZE; i++)
        {
            for (int j = 0; j < TABLE_SIZE; j++)
            {
                if(table_history[2][i][j] == table[i][j]) {
                    similar++;
                }
            }
        }
        if(similar == TABLE_SIZE * TABLE_SIZE) {
            showGameOver(renderer, font);
            Mix_FreeMusic(bg_mus);
            Mix_PlayChannel(-1, loose, 0);
            SDL_Delay(3000);
            quit = true;
        }


        printTable(renderer, table);
        SDL_Delay(150);
        clearScreen(renderer);
    }
    
    SDL_DestroyWindow(window);
    Mix_FreeMusic(bg_mus);
    Mix_FreeChunk(loose);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}

void generateTable(int (*table)[TABLE_SIZE]) {
    srand(time(0));
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            table[i][j] = rand() % 2 ;
        }
        
    }
}

void clearTable(int (*table)[TABLE_SIZE]) {
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            table[i][j] = 0;
        }
        
    }
}

void printTable(SDL_Renderer *renderer, int (*table)[TABLE_SIZE]) {
    srand(time(0));
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            // printf("%d ", table[i][j]);
            if(table[i][j]) {
                drawRect(renderer, CELL_SIZE * j, CELL_SIZE * i);
            }
        }
        // printf("\n");   
    }
}

void updateTable(int (*table)[TABLE_SIZE], int (*next)[TABLE_SIZE]) {
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        for (int j = 0; j < TABLE_SIZE; j++)
        {
            table[i][j] = next[i][j];
        }
        
    }
    
}

int countNeighbors(int (*table)[TABLE_SIZE], int x, int y) {
    int sum = 0;
    int cur_col = 0;
    int cur_row = 0;
    for (int i = -1; i < 2; i++)
    {
        for (int j = -1; j < 2; j++)
        {
            cur_col = (x + i + TABLE_SIZE) % TABLE_SIZE ;
            cur_row = (y + j + TABLE_SIZE) % TABLE_SIZE;
            sum += table[cur_col][cur_row];
        }
    }

    sum -= table[x][y];
    return sum;
    
}




// SDL Functions
void clearScreen(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void drawRect(SDL_Renderer *renderer, int pos_x, int pos_y) {
    SDL_Rect r;
    r.x = pos_x;
    r.y = pos_y;
    r.w = CELL_SIZE;
    r.h = CELL_SIZE;
    SDL_SetRenderDrawColor( renderer, 0, 255, 0, 255 );
    SDL_RenderFillRect( renderer, &r );
    SDL_RenderPresent(renderer);
}

void drawText(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {0, 255, 0, 0};
    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

void showGameOver(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Texture *texture1, *texture2;
    SDL_Rect rect1, rect2;
    drawText(renderer, SCREEN_SIZE / 4, SCREEN_SIZE / 4, "Game", font, &texture1, &rect1);
    drawText(renderer, SCREEN_SIZE / 4, rect1.y + rect1.h, "over", font, &texture2, &rect2);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture1, NULL, &rect1);
    SDL_RenderCopy(renderer, texture2, NULL, &rect2);
    SDL_RenderPresent(renderer);
}