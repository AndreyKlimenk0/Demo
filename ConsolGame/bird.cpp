#include "bird.h"

Bird::Bird(u32 tiles_count, u32 update_time, IGameUpdater * game_updater) : 
	DynamicActor('~', tiles_count, update_time, game_updater), random_bird_direction(0, 1)
{
}

void Bird::update_position()
{
	u8 direction = static_cast<u8>(random_bird_direction(rng));
	if (direction == 0) {
		if (get_position() > 0) {
			set_position(get_position() - 1);
		}
	}
	else if (direction == 1) {
		if (get_position() < (tiles_count - 1)) {
			set_position(get_position() + 1);
		}
	}
}