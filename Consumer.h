#ifndef CONSUMER_H
#define CONSUMER_H

#include "shared.h"
#include<SDL.h>
class Consumer{
	static SDL_Renderer* renderer;
	static SDL_Rect consumer_dest;
	static SDL_Rect consumer_dest4;

	static void consume(){
		printf("consumes food\n");
		for( int i = 0; i < 3; i++ ){
			SDL_SemWait(shared::lock);
				SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
				SDL_RenderFillRect(shared::renderer, &Consumer::consumer_dest4);		
				SDL_RenderCopy(shared::renderer, Consumer::consumerimg[1], NULL, &Consumer::consumer_dest);
				SDL_RenderPresent(shared::renderer);
			SDL_SemPost(shared::lock);
				time_delay(shared::consumer_speed/6);

			SDL_SemWait(shared::lock);
				SDL_RenderCopy(shared::renderer, Consumer::consumerimg[2], NULL, &Consumer::consumer_dest);
				SDL_RenderPresent(shared::renderer);
			SDL_SemPost(shared::lock);
				time_delay(shared::consumer_speed/6);
		}
		SDL_SemWait(shared::lock);
			SDL_RenderCopy(shared::renderer, Consumer::consumerimg[4], NULL, &Consumer::consumer_dest);
			SDL_RenderPresent(shared::renderer);
		SDL_SemPost(shared::lock);
	}

	static void take_food(){
		// check if food available
		if( shared::count == 0 ){
			SDL_SemWait(shared::lock);
					SDL_RenderCopy(shared::renderer, Consumer::consumerimg[3], NULL, &Consumer::consumer_dest4);
					SDL_RenderPresent(shared::renderer);
			SDL_SemPost(shared::lock);
		}
		SDL_SemWait(shared::fill);	// wait while no fill

		SDL_SemWait(shared::lock);
				SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
				SDL_RenderFillRect(shared::renderer, &Consumer::consumer_dest4);
				SDL_RenderCopy(shared::renderer, Consumer::consumerimg[0], NULL, &Consumer::consumer_dest);
				SDL_RenderPresent(shared::renderer);
		SDL_SemPost(shared::lock);
		time_delay(shared::consumer_speed/6);

		//atomic operation
		SDL_SemWait(shared::lock);
			//update food_dest

			shared::food_dest.x -= food_width + 10;
			printf("Taking food\n");
			
			
				//remove
				SDL_SetRenderDrawColor(shared::renderer, 255,255,255,255);
				SDL_RenderFillRect(shared::renderer,&shared::food_dest);
				
				SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
				SDL_RenderFillRect(shared::renderer, &Consumer::consumer_dest4);
				SDL_RenderCopy(shared::renderer, Consumer::consumerimg[1], NULL, &Consumer::consumer_dest);
				SDL_RenderPresent(shared::renderer);
			
				shared::count--;
		SDL_SemPost(shared::lock);
	}

	static void update_producer(){
		// update producer about the empty
		SDL_SemPost(shared::empty);
		printf("Updated producer\n");
	}

	public:
	static SDL_Texture* consumerimg[5];

	static int run(void* data){
		SDL_SemWait(shared::lock);
			SDL_SetRenderDrawColor(shared::renderer, 255, 255, 255, 255 );
			SDL_RenderFillRect(shared::renderer, &Consumer::consumer_dest4);
			SDL_RenderCopy(shared::renderer, Consumer::consumerimg[0], NULL, &Consumer::consumer_dest);
			SDL_RenderPresent(shared::renderer);
		SDL_SemPost(shared::lock);
		time_delay(shared::consumer_speed/6);

		while(shared::status){
			take_food();

			update_producer();
			time_delay(shared::consumer_speed/6);

			consume();
			time_delay(shared::consumer_speed/6);
		}
		return 0;
	}
};

SDL_Renderer* Consumer::renderer = shared::renderer;
SDL_Texture* Consumer::consumerimg[5] = {NULL, NULL};
SDL_Rect Consumer::consumer_dest = {600,180,160,200};
SDL_Rect Consumer::consumer_dest4 = {600,120,200,260};
#endif