#ifndef ACTOR_H
#define ACTOR_H

#include "helper.h"

class Actor {
public:
	Actor() : symbol('E') {};
	Actor(u8 symbol) : symbol(symbol) {};

	void set_symbol(u8 new_symbol);
	u8 get_symbol() const;

private:
	u8 symbol;
};

#endif