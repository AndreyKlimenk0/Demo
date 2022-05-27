#include "bird.h"

Bird::Bird(u32 update_time, IGameUpdater * game_updater) : 
	DynamicActor('~', update_time, game_updater), random_bird_direction(0, 1)
{
}

void Bird::update_position(int &tiles_offset)
{
	u8 random_direction = static_cast<u8>(random_bird_direction(rng));
	if (random_direction == 0) {
		tiles_offset = -1;
	}
	else if (random_direction == 1) {
		tiles_offset = 1;
	}
}