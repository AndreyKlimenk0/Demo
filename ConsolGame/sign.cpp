#include "sign.h"

Sign::Sign() : DynamicActor('x'), shot(false)
{
}

bool Sign::was_shot() const
{
	return shot;
}

void Sign::update_position()
{
	if (GetAsyncKeyState(VK_LEFT)) {
		if (get_position() > 0) {
			set_position(get_position() - 1);
		}
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		if (get_position() < (tiles_count - 1)) {
			set_position(get_position() + 1);
		}
	}

	if (GetAsyncKeyState(VK_SPACE)) {
		shot = true;
	}
	else {
		shot = false;
	}
}