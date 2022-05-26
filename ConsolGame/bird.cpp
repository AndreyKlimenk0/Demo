#include "bird.h"

Bird::Bird() : DynamicActor('~'), random_bird_direction(0, 1)
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