#include "sign.h"


Sign::Sign(u32 update_time, IGameUpdater * game_updater) : DynamicActor('x', update_time, game_updater)
{
}

bool Sign::was_shot() const
{
	return shot;
}

void Sign::update_position(int &tiles_offset)
{
	if (GetAsyncKeyState(VK_LEFT)) {
		tiles_offset = -1;
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		tiles_offset = 1;
	}

	if (GetAsyncKeyState(VK_SPACE)) {
		shot = true;
	}
	else {
		shot = false;
	}
}