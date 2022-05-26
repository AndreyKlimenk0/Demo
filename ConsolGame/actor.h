#ifndef ACTOR_H
#define ACTOR_H

#include "helper.h"

class Actor {
public:
	Actor() : symbol('E') {};
	Actor(u8 symbol) : symbol(symbol) {};

	void set_symbol(u8 new_symbol);
	void set_position(u32 new_position);

	u8 get_symbol() const;
	u32 get_position() const;

private:
	u8 symbol;
	u32 position;
};

#endif