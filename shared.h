#ifndef SHARED_H
#define SHARED_H

#include<SDL_thread.h>
#include<SDL.h>
#include<fstream>
using namespace std;
#define delay 1000
#define food_width 70
#define food_height 50

//**************Common Functions ****************
void time_delay(Uint32 t = delay){
	Uint32 start = SDL_GetTicks();
	while( SDL_GetTicks() - start < t);
}
//**************Common Functions ****************

class shared{
   public :
    static const int bufsize = 4;
	static SDL_sem *lock;
	static SDL_sem *empty;
	static SDL_sem *fill; 
	static bool status;
	static SDL_Texture* food;
	static SDL_Rect food_dest;
	static SDL_Renderer *renderer;
	static ofstream *logfile;
	static int count;
	static unsigned int producer_speed;
	static unsigned int consumer_speed;
};

// ****************************Initialize static variables********************************* 
SDL_sem* shared::lock = SDL_CreateSemaphore(1);
SDL_sem* shared::empty = SDL_CreateSemaphore(shared::bufsize);
SDL_sem* shared::fill = SDL_CreateSemaphore(0);
bool shared::status = true;
SDL_Texture* shared::food = NULL;
SDL_Rect shared::food_dest = {210,250,food_width,food_height};
SDL_Renderer* shared::renderer = NULL;
ofstream *shared::logfile = NULL;
int shared::count = 0;
unsigned int shared::producer_speed = 2000;
unsigned int shared::consumer_speed = 3000;
// ****************************Initialize static variables********************************* 
#endif