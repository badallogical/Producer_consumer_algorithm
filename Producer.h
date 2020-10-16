#ifndef PRODUCER_H
#define PRODUCER_H

#include "shared.h"
#include<SDL.h>
#include<iostream>
using namespace std;

class Producer{
	static SDL_Renderer* renderer; 
	static SDL_Rect producer_dest;
	static SDL_Rect producer_dest1;
	static SDL_Rect producer_dest2;
	static SDL_Rect producer_dest4;
	static SDL_Rect area;

	
	static void insert_food(){
		//check if empty 
		// sleeping
		if( shared::count == shared::bufsize ){
		SDL_SemWait(shared::lock);
			SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
			SDL_RenderFillRect(shared::renderer, &Producer::area);
			SDL_RenderCopy(shared::renderer, Producer::producerimg[1], NULL, &(Producer::producer_dest));
			SDL_RenderPresent(shared::renderer);
		SDL_SemPost(shared::lock);
		}

		SDL_SemWait(shared::empty);

		//inserting after waking up
		SDL_SemWait(shared::lock);
			SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
			SDL_RenderFillRect(shared::renderer, &Producer::area);
			SDL_RenderCopy(shared::renderer, Producer::producerimg2[2], NULL, &Producer::producer_dest2);
			SDL_RenderPresent(shared::renderer);
		SDL_SemPost(shared::lock);
		time_delay(shared::producer_speed/6);


		// atomic
		SDL_SemWait(shared::lock);
			//Insert food
			printf("Insering food\n");
			
				SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
				SDL_RenderFillRect(shared::renderer, &Producer::area);
				
				SDL_RenderCopy(shared::renderer, Producer::producerimg[2], NULL, &Producer::producer_dest4);
				SDL_RenderPresent(shared::renderer);
				SDL_RenderCopy(shared::renderer, shared::food, NULL, &shared::food_dest);
				SDL_RenderPresent(shared::renderer);
			

			//update food_dest
			shared::food_dest.x += food_width + 10;
			
			shared::count++;
		SDL_SemPost(shared::lock);
	};
	
	static void produce(){
		// produce
		printf("food Produced\n");
		SDL_SemWait(shared::lock);
		SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
		SDL_RenderFillRect(shared::renderer, &Producer::area);
		SDL_SemPost(shared::lock);

		for( int i = 0; i < 3; i++ ){
			SDL_SemWait(shared::lock);
			SDL_RenderCopy(shared::renderer, Producer::producerimg2[i], NULL, &Producer::producer_dest2 );
			SDL_RenderPresent(shared::renderer);
			SDL_SemPost(shared::lock);
			time_delay(shared::producer_speed/3);
		}
		
		
	}
	
	static void update_consumer(){
		SDL_SemPost(shared::fill);
		printf("updated consumer\n");

		SDL_SemWait(shared::lock);
		SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
		SDL_RenderFillRect(shared::renderer, &Producer::area);
		
		SDL_RenderCopy(shared::renderer, Producer::producerimg[0], NULL, &Producer::producer_dest1);
		SDL_RenderPresent(shared::renderer);
		SDL_SemPost(shared::lock);
	}

public:
	static SDL_Texture *producerimg[3];
	static SDL_Texture *producerimg2[3];
	
	static int run(void *data){
		// idel producer
		SDL_SemWait(shared::lock);
		SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
		SDL_RenderFillRect(shared::renderer, &Producer::area);
		
		SDL_RenderCopy(shared::renderer, Producer::producerimg[0], NULL, &(Producer::producer_dest1));
		SDL_RenderPresent(shared::renderer);
		SDL_SemPost(shared::lock);

		// working
		while(shared::status){
			produce();

			insert_food();
			time_delay(shared::producer_speed/6);

			update_consumer();
			time_delay(shared::producer_speed/6);
		}
		return 0;
	}
};


SDL_Renderer* Producer::renderer = shared::renderer;
SDL_Texture* Producer::producerimg[3] = {NULL, NULL};
SDL_Texture* Producer::producerimg2[3] =  { NULL, NULL};
SDL_Rect Producer::producer_dest1 = {10,190,100,180};
SDL_Rect Producer::producer_dest4 = {10,190,100,180};
SDL_Rect Producer::producer_dest2 = {10,190,170,190};
SDL_Rect Producer::producer_dest = {10,180,130,200};
SDL_Rect Producer::area = {10,180,190,200};
#endif
