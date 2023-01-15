#include "graphics.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include "rs232.c"



#define window_x 1280
#define window_y 720

#define serial_port             7
#define serial_baudrate         38400

#define serial_mode             "8n1"

#define ball_radius             8
#define ball_color              YELLOW
#define field_color             RED
#define paddle_color            MAGENTA
#define background_color        BLACK
#define paddle_dimension_x      15
#define paddle_dimension_y      50
#define delay_time              5
int ball_movement_x =           1;
int ball_movement_y =           -1;

#define interrupt_byte          8
#define interrupt_delay         1

#define ball_start_position_x 1000                                                 //comment this to make the ball start from the middle of the screen (x)
#define ball_start_position_y 20                                                  //comment this to make the ball start from the middle of the screen (y)

//#define paddle_start_position_a                                                   //comment this to make the paddle spawn in the middle of the screen (x)
//#define paddle_start_position_b                                                   //comment this to make the paddle spawn in the middle of the screen (x)





#define True 1
#define False 0



void initialize();
void update_ball();
void draw_ball(int x, int y, int r, int c);
void update_pong(char which, int pos);
void draw_pong(int T_or_F, int x, int y);
void serial_input();
int get_input(char);

void collision_paddle_a();
void collision_paddle_b();
void collision_upper_wall();
void collision_bottom_wall();
void score_point_left_wall();


void unsigned_char_to_signed(char dest[], unsigned char source[]);
long map(long x, long in_min, long in_max, long out_min, long out_max);





#ifndef ball_start_position_x
    #define ball_start_position_x getmaxx()/2
#endif

#ifndef ball_start_position_y
    #define ball_start_position_y getmaxy()/2
#endif

#ifndef paddle_start_position_a
    #define paddle_start_position_a getmaxy()/2
#endif

#ifndef paddle_start_position_b
    #define paddle_start_position_b getmaxy()/2
#endif

#ifndef paddle_position_a
    #define paddle_position_a 10
#endif

#ifndef paddle_position_b
    #define paddle_position_b (getmaxx() - 10 - paddle_dimension_x)
#endif

#define serial_number (serial_port - 1)


int old_paddle_position_a;
int old_paddle_position_b;

int ball_old_position_x;
int ball_old_position_y;

char input_string[11];                                 //"Axxx-Bxxx" + '\n' + '/0'

int input_int_a;
int input_int_b;

int score_paddle_a;
int score_paddle_b;

int main()
{

    initialize();

    while(True)
    {

        serial_input();
        //serial_input_test();

        update_pong('a', get_input('a'));
        update_pong('b', get_input('b'));

        update_ball();
        delay(delay_time);

    }

}

void initialize()
{
    initwindow(window_x, window_y, "Pong, but better!");

    old_paddle_position_a = paddle_start_position_a;
    old_paddle_position_b = paddle_start_position_b;

    ball_old_position_x = ball_start_position_x;
    ball_old_position_y = ball_start_position_y;


    RS232_OpenComport(serial_number, serial_baudrate, serial_mode);
    RS232_flushRXTX(serial_number);
}

void draw_ball(int x, int y, int r, int c)
{
    setcolor(c);
    circle(x, y, r);
    setfillstyle(1, c);
    floodfill(x, y, c);
}


int get_input(char which)
{
    int temp;

    //printf("\npaddle_a: %d  paddle_b: %d" , input_int_a, input_int_b);

    if(which == 'a')
    {
        temp = map(input_int_a, 0, 255, 0, getmaxy());
    }
    if(which == 'b')
    {
        temp = map(input_int_b, 0, 255, 0, getmaxy());
    }
    return temp;
}

void update_pong(char which, int pos)
{
    if((which == 'a') && (pos != old_paddle_position_a))
    {
        draw_pong(False, paddle_position_a, old_paddle_position_a);
        draw_pong(True, paddle_position_a, pos);
        old_paddle_position_a = pos;
    }
    if((which == 'b') && (pos != old_paddle_position_b))
    {
        draw_pong(False, paddle_position_b, old_paddle_position_b);
        draw_pong(True, paddle_position_b, pos);
        old_paddle_position_b = pos;
    }

}

void draw_pong(int T_or_F, int x, int y)
{


    if(T_or_F == False)
    {
        setfillstyle(1, background_color);
        //floodfill(x + 1, y + 1, background_color);
        bar(x, y, paddle_dimension_x + x, paddle_dimension_y + y);
    }else
    {
        setfillstyle(1, paddle_color);
        bar(x, y, paddle_dimension_x + x, paddle_dimension_y + y);
    }

}


void update_ball()
{
    draw_ball(ball_old_position_x, ball_old_position_y, ball_radius, background_color);


    collision_upper_wall();
    collision_bottom_wall();
    collision_paddle_a();
    collision_paddle_b();

    ball_old_position_x += ball_movement_x;
    ball_old_position_y += ball_movement_y;
    draw_ball(ball_old_position_x, ball_old_position_y, ball_radius, ball_color);
}

void serial_input()
{
    unsigned char temp[11];
    char temp_char[11];

    //RS232_flushRXTX(serial_number);
    //RS232_SendByte(serial_number, interrupt_byte);
    //delay(interrupt_delay);
    RS232_flushRXTX(serial_number);
    while(RS232_PollComport(serial_number, temp, sizeof(input_string)) == NULL);
    RS232_flushRXTX(serial_number);

    printf("%s\n\n\n\n\n\n", temp);



    unsigned_char_to_signed(input_string, temp);

    if((input_string[0] == 'A') && (input_string[5] == 'B'))
    {
        strncpy(temp_char, input_string, 4);
        input_string[0] = '0';
        input_int_a = atoi(input_string);

        strcpy(temp_char, input_string);
        for (int i = 1; i < 6; ++i) {
            temp_char[i] = '0';
        }
        input_int_b = atoi(temp_char);


    }
}


void unsigned_char_to_signed(char dest[], unsigned char source[])
{
    int i = 0;
    while(source[i] != '\0')
    {
        dest[i] = source[i];
        i++;
    }
}


long map(long x, long in_min, long in_max, long out_min, long out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}





void collision_paddle_a()
{
    if((ball_old_position_y > old_paddle_position_a - ball_radius) && (ball_old_position_y < old_paddle_position_a + paddle_dimension_y + ball_radius))                 //checks if the ball is in the vertical zone of the paddle (the position controlled by the player)
    {
        if(ball_old_position_x < paddle_position_a + ball_radius + paddle_dimension_x + 5)                                                                              //checks if the ball is colliding with the surface of the paddle (the position defined by a constant)
        {
            if(ball_old_position_x > ball_radius + paddle_dimension_x - 4)                                                                                              //checks if the ball already passed over the paddle
            {
                ball_movement_x = -ball_movement_x;
                //ball_movement_y = -ball_movement_y;
            }
        }
    }
}

void collision_paddle_b()
{
    if((ball_old_position_y > old_paddle_position_b - ball_radius) && (ball_old_position_y < old_paddle_position_b + paddle_dimension_y + ball_radius))         //checks if the ball is in the
    {
        if(ball_old_position_x > paddle_position_b - (ball_radius + 2))
        {
            if(ball_old_position_x < paddle_position_b + 2)
            {
                ball_movement_x = -ball_movement_x;
                //ball_movement_y = -ball_movement_y;
            }

        }
    }
}

void collision_upper_wall(){
    if(((ball_old_position_y + ball_movement_y + ball_radius) > window_y))
    {
        ball_movement_y = -ball_movement_y;
    }
}

void collision_bottom_wall()
{
    if(((ball_old_position_y + ball_movement_y - ball_radius) < 0))
    {
        ball_movement_y = -ball_movement_y;
    }
}


void score_point_right_wall_paddle_a()
{
    if(ball_old_position_x + ball_movement_x > window_x)

}

void score_point_left_wall_paddle_b()
{

}