#include "actor.h"

void Actor::set_symbol(u8 new_symbol)
{
	symbol = new_symbol;
}

u8 Actor::get_symbol() const
{
	return symbol;
}
