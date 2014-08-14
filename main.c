#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ncurses.h"
#include "fangkuai.h"
#include "signal.h"
#include "unistd.h"

#define PLACE_LINES 21
#define PLACE_COLS 12
#define BLOCK_WIDTH 5
#define BLOCK_HEIGHT 2

WINDOW *place[PLACE_LINES][PLACE_COLS] = {};
int place_buf[PLACE_LINES][PLACE_COLS] = {
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1} ,
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1} ,
};

int current[4][4] = {};
int next[4][4] = {};

typedef struct _POSITION 
{
    int x;
    int y;
} POSITION;

POSITION current_position;

int current_status = 0;
int (*current_fangkuai)[4][4];

void init_place();
WINDOW *create_newwin(int height, int width, int starty, int startx);
void paint_win(WINDOW *win, int color);
void paint_fangkuai(int (*fk)[4], int y, int x, int color);
void transform();
int check_move(int direction);
int move_fangkuai(int direction);
int check_transform(int (*fangkuai)[4]);

static void fall_down()
{
    if(move_fangkuai(1) == 0)
    {
        persistence();
        cut_the_floor();
        load_new_fangkuai();
    }
    alarm(1);
}

int main(int argc, char *argv[])
{   
    current_position.x = 5;
    current_position.y = 0;
    WINDOW *my_win;
    int startx, starty, width, height;
    int ch;
    initscr();    
    start_color();
    init_pair(1,COLOR_WHITE,COLOR_BLACK);
    init_pair(2,COLOR_BLACK,COLOR_WHITE);
    init_pair(3,COLOR_RED,COLOR_WHITE);
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    refresh();
    init_place();

    //current_fangkuai = fangkuai[2];
    load_new_fangkuai();
    //paint_fangkuai(current_fangkuai[current_status], current_position.y, current_position.x, 2);
    //paint_current(1);
    
    signal(SIGALRM, fall_down);
    alarm(1);

    while((ch = getch()) != KEY_F(1))
    {
        switch(ch)
        {
            case KEY_LEFT:
                move_fangkuai(2);
                break;
            case KEY_RIGHT:
                move_fangkuai(3);
                break;
            case KEY_UP:
                paint_fangkuai(current_fangkuai[current_status], current_position.y, current_position.x, 1);
                transform();
                paint_fangkuai(current_fangkuai[current_status], current_position.y, current_position.x, 2);
                break;
            case KEY_DOWN:
                move_fangkuai(1);
                break;
        }
    };
    endwin();
    return 0;
}

int count=0;

void init_place()
{
    int place_width = BLOCK_WIDTH * PLACE_COLS;
    int place_height = BLOCK_HEIGHT * PLACE_LINES;
    int place_startx = (COLS - place_width) / 2;
    int place_starty = (LINES - place_height) / 2;
    int x=0, y=0;
    for(y=0;y<PLACE_LINES;y++)
    {
        for(x=0;x<PLACE_COLS;x++)
        {
            int startx = place_startx + BLOCK_WIDTH * x;
            int starty = place_starty + BLOCK_HEIGHT * y;
            place[y][x] = create_newwin(BLOCK_HEIGHT, BLOCK_WIDTH, starty, startx);
            int color = place_buf[y][x] + 1;
            paint_win(place[y][x], color);
        }
    }
}

void paint_place()
{
    int i=0,j=0;
    for(i=0; i<PLACE_LINES; i++)
    {
        for(j=0; j<PLACE_COLS; j++)
        {
            paint_win(place[i][j], place_buf[i][j] + 1);
        }
    }
}

void paint_fangkuai(int (*fk)[4], int y, int x, int color)
{
    int i=0, j=0;
    for(i=0;i<4;i++)
    {
        for(j=0;j<4;j++)
        {
            if(((j+x) < PLACE_COLS) && ((i+y) < PLACE_LINES ))
            {
                if(fk[i][j] == 1)
                {
                    paint_win(place[i+y][j+x], color);
                }
            }
        }
    }
}

void paint_win(WINDOW *win, int color)
{
    wbkgd(win, COLOR_PAIR(color));
    wrefresh(win);
}


WINDOW *create_newwin(int height, int width, int starty, int startx)
{   
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    // wbkgd(local_win,COLOR_PAIR(2));
    //wrefresh(local_win);

    return local_win;
}

void transform()
{
    int x=0, y=0;
    if(check_transform(current_fangkuai[current_status]) == 0)
    {
        return;
    }
    current_status ++;
    current_status = current_status % 4;
}

int check_transform(int (*fangkuai)[4])
{
    int i=0, j=0;
    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            if(fangkuai[i][j] == 1)
            {
                if(place_buf[current_position.y + (3 - j)][current_position.x + i] == 1)
                {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int check_move(int direction)
{
    //down: 1, left: 2 right: 3
    int i = 0, j = 0;
    for(i=0; i < 4; i++)
    {
        for(j=0; j<4;j++)
        {
            if(current_fangkuai[current_status][i][j] == 1)
            {
                if(direction == 1)
                {
                    if(place_buf[current_position.y + 1 + i][current_position.x + j] == 1)
                    {
                        return 0;
                    }
                }
                if(direction == 2)
                {
                    if(place_buf[current_position.y + i][current_position.x + j - 1] == 1)
                    {
                        return 0;
                    }
                }
                if(direction == 3)
                {
                    if(place_buf[current_position.y + i][current_position.x + j + 1] == 1)
                    {
                        return 0;
                    }
                }
            }
        }
    }
    return 1;
}

int move_fangkuai(int direction)
{
    if(check_move(direction))
    {
        paint_fangkuai(current_fangkuai[current_status], current_position.y, current_position.x, 1);
        if(direction == 1)
        {
            current_position.y++;
        }
        else if(direction == 2)
        {
            current_position.x--;
        }
        else
        {
            current_position.x++;
        }
        paint_fangkuai(current_fangkuai[current_status], current_position.y, current_position.x, 2);
        return 1;
    }
    return 0;
}

int persistence()
{
    int i =0, j =0;
    for(i=0; i<4; i++)
    {
        for(j=0; j<4; j++)
        {
            place_buf[current_position.y + i][current_position.x+j] += current_fangkuai[current_status][i][j];
        }
    }
    return 1;
}

int load_new_fangkuai()
{
    current_position.y = 0;
    current_position.x = 5;
    current_status = 0;
    int f = rand() % 7;
    current_fangkuai = fangkuai[f];
    paint_fangkuai(current_fangkuai[current_status], current_position.y, current_position.x, 2);
}

int cut_the_floor()
{
    int i = PLACE_COLS;
    int l = 0;
    int change = 0;
    for(l=current_position.y; l<current_position.y + 4 && l < PLACE_LINES-1; l++)
    {
        int full = 1;
        for(i = 1; i < PLACE_COLS-1; i++)
        {
            if(place_buf[l][i] == 0)
            {
                full = 0;
                break;
            }
        }
        if(full)
        {
            change = 1;
            int o=0, p=0;
            for(o=l; o>0; o-- )
            {
                for(p = 1; p < PLACE_COLS - 1; p++)
                {
                    place_buf[o][p] = place_buf[o-1][p];
                }
            }
            for(p=1; p<PLACE_COLS -1; p++)
            {
                place_buf[0][p] = 0;
            }
        }
    }
    if(change)
    {
        paint_place();
    }
    return 1;
}
