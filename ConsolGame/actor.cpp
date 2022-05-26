#include "actor.h"

void Actor::set_symbol(u8 new_symbol)
{
	symbol = new_symbol;
}

void Actor::set_position(u32 new_position)
{
	position = new_position;
}

u8 Actor::get_symbol() const
{
	return symbol;
}

u32 Actor::get_position() const
{
	return position;
}
