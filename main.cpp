#include<SDL.h>
#include<iostream>
#include<SDL_thread.h>
#include<SDL_image.h>
#include "Producer.h"
#include "Consumer.h"
#include <fstream>
#include <string>
#include "shared.h"
#include <limits.h>
#include <SDL_ttf.h>
using namespace std;

#define width 800
#define height 479
#define max_low 60000
#define delta 500

//******** Accessories **********************************//
SDL_Texture* load(string path){
	SDL_Surface *temp = IMG_Load(path.c_str());
	SDL_Texture *tex = NULL;
	if( temp == NULL ){
		*shared::logfile << IMG_GetError;
		shared::status = false;
	}
	else{
		tex = SDL_CreateTextureFromSurface( shared::renderer, temp);
	}
	SDL_FreeSurface( temp );
	return tex;
}

void initialze_assets( ){
		Producer::producerimg[0] = load("Assets//producer//producer1.png");
		Producer::producerimg[1] = load("Assets//producer//producer3.png");
		Producer::producerimg[2] = load("Assets//producer//producer4.png");
		
		// producing 
		Producer::producerimg2[0] = load("Assets//producer//producer21.png");
		Producer::producerimg2[1] = load("Assets//producer//producer22.png");
		Producer::producerimg2[2] = load("Assets//producer//producer23.png");

		Consumer::consumerimg[0] = load("Assets//consumer//consumer1.png");
		Consumer::consumerimg[1] = load("Assets//consumer//consumer2.png");
		Consumer::consumerimg[2] = load("Assets//consumer//consumer3.png");
		Consumer::consumerimg[3] = load("Assets//consumer//consumer4.png");
		Consumer::consumerimg[4] = load("Assets//consumer//consumer5.png");
		shared::food = load("Assets//producer//food.png");
}
//*****************Accessories *************************//

int main(int argc, char* args[] ){

	// open the file 
	ofstream file("log.txt");
	shared::logfile = &file;

	// init sdl
	if( SDL_Init( SDL_INIT_VIDEO ) == -1 ){
		*shared::logfile << SDL_GetError();
		shared::status = false;
	}
	else{
		//  init image
		if( !( IMG_Init( IMG_INIT_PNG) & IMG_INIT_PNG ) ){
			*shared::logfile << IMG_GetError();
			shared::status = false;
		}
		else{
			//window and renderer
			SDL_Window *window = SDL_CreateWindow("Producer Consumer version 0.1", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN );
			if( window == NULL ){
				*shared::logfile << SDL_GetError();
				shared::status = false;
			}
			else{
				
				SDL_Renderer *renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
				if( renderer == NULL ){
					*shared::logfile << SDL_GetError();
					shared::status = false;
				}
				else{

					// Initialize shared resources
					shared::renderer = renderer;

					//font and text
					if( TTF_Init() == -1 ){
						*shared::logfile << TTF_GetError();
						shared::status = false;
					}
					SDL_Color color = {255,50,50};
					TTF_Font *font = TTF_OpenFont("Assets//calibrii.ttf",255);
					SDL_Surface *tmp = NULL;
					SDL_Texture *text = SDL_CreateTextureFromSurface( renderer, (tmp = TTF_RenderText_Solid(font, "Producer-Consumer", color)) );
					SDL_FreeSurface(tmp);
					SDL_Rect font_rect = {200,80,400,100};

					// Initialize all assets
					initialze_assets();

					//create buffer conware belt
					SDL_Rect conware_belt = {190, 300, 400, 50 };
					SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
					SDL_RenderClear(renderer);
					SDL_RenderCopy(renderer, text, NULL, &font_rect );
					SDL_SetRenderDrawColor( renderer, 200, 200, 0, 0 );
					SDL_RenderFillRect( renderer, &conware_belt);
					
					// land
					SDL_SetRenderDrawColor( renderer, 129,200,48,255 );
					conware_belt.x = 0;
					conware_belt.y = 379;
					conware_belt.w = 800;
					conware_belt.h = 100;
					SDL_RenderFillRect(renderer, &conware_belt);
					SDL_RenderPresent(renderer);

					// producer and consumer power bar
					SDL_Rect p_speed_rect = {30,400,100,30};
					SDL_Rect c_speed_rect = {650,400,100,30};


					// create producer and consumer
					SDL_Thread *producerid = SDL_CreateThread( Producer::run , "Producer", NULL );
					SDL_Thread *consumerid = SDL_CreateThread( Consumer::run , "Consumer", NULL );

					// event handling
					SDL_Event e;
					while( shared::status ){
						while( SDL_PollEvent( &e ) != 0 ){
							if( e.type == SDL_QUIT ){
								shared::status = false;
							}
							else if( e.type == SDL_KEYDOWN ){
								if( e.key.keysym.sym == SDLK_UP && (SDL_GetModState() & KMOD_LCTRL) ){
									if( shared::producer_speed != 0 && shared::producer_speed - delta >= 0 ){
										shared::producer_speed -= delta;
									}
									cout << " Producer " <<shared::producer_speed <<" percentage " <<((1*max_low/ (float)shared::producer_speed)/120) * 100 << endl;
								}
								else if( e.key.keysym.sym == SDLK_DOWN && (SDL_GetModState() & KMOD_LCTRL) ){
									
									if( shared::producer_speed != max_low && shared::producer_speed + delta <= max_low ){
										shared::producer_speed += delta;
									}
									cout << "Producer " << shared::producer_speed <<" percentage " <<((1*max_low/ (float)shared::producer_speed)/120) * 100 << endl;
								}
								else if( e.key.keysym.sym == SDLK_UP && SDL_GetModState() & KMOD_RCTRL ){
									if( shared::consumer_speed != 0 && shared::consumer_speed - delta >= 0  ){
										shared::consumer_speed -= delta;
									}
									cout << "Consumer " << shared::consumer_speed <<"percentage " <<((1*max_low/ (float)shared::consumer_speed)/120) * 100 << endl;
								}
								else if(e.key.keysym.sym == SDLK_DOWN && SDL_GetModState() & KMOD_RCTRL ){
									if( shared::consumer_speed != max_low && shared::consumer_speed + delta <= max_low )
										shared::consumer_speed += delta;
									cout << "Consumer " << shared::consumer_speed << "percentage " << ((1*max_low/ (float)shared::consumer_speed)/120) * 100  << endl;
								}

								
								//display producer speed
								tmp = TTF_RenderText_Solid(font, to_string(((1*max_low/ (float)shared::producer_speed)/120) * 100).c_str(),color);
								text = SDL_CreateTextureFromSurface(renderer, tmp );
								SDL_FreeSurface(tmp);

								SDL_SemWait(shared::lock);
								SDL_SetRenderDrawColor(renderer, 129,200,48,255);
								SDL_RenderFillRect(renderer, &p_speed_rect);
								SDL_RenderCopy(renderer, text, NULL, &p_speed_rect);
								SDL_RenderPresent(renderer);
								SDL_SemPost(shared::lock);

								//destroy texture 
								// SDL_DestroyTexture(text);
								// text = NULL;

								tmp = TTF_RenderText_Solid(font, to_string(((1*max_low/ (float)shared::consumer_speed)/120) * 100).c_str(),color);
								text = SDL_CreateTextureFromSurface(renderer,tmp);
								SDL_FreeSurface(tmp);

								// display consumer speed
								SDL_SemWait(shared::lock);
								SDL_SetRenderDrawColor(renderer, 129,200,48,255);
								SDL_RenderFillRect(renderer, &c_speed_rect);
								SDL_RenderCopy(renderer, text, NULL, &c_speed_rect);
								SDL_RenderPresent(renderer);
								SDL_SemPost(shared::lock);

								//destroy texture
								// SDL_DestroyTexture(text);
								// text = NULL;
								
							}
						}
					}
				}
			}

		}
	}

	shared::logfile->close();
	return 0;
}