#include "sign.h"


Sign::Sign(u32 update_time, IGameUpdater * game_updater) : DynamicActor('x', update_time, game_updater)
{
}

bool Sign::was_shot() const
{
	return shot;
}

void Sign::update_position(Direction &direction, u32 &tiles_number)
{
	if (GetAsyncKeyState(VK_LEFT)) {
		direction = LEFT_SIDE;
		tiles_number = 1;
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		direction = RIGHT_SIDE;
		tiles_number = 1;
	}

	if (GetAsyncKeyState(VK_SPACE)) {
		shot = true;
	}
	else {
		shot = false;
	}
}