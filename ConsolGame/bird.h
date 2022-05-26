#ifndef BIRD_H
#define BIRD_H

#include <random>
#include "dynamic_actor.h"

/**
* @brief class realize bird game mechenics. on a tiles line there is a bird tile with symbol '~'. the bird moves in random direction per set time.
*/
class Bird : public DynamicActor {
public:
	Bird();
private:
	/**
	* @brief updates a position in one tile in random direction (left or right side).
	* @return void
	*/
	void update_position();
	std::uniform_int_distribution<std::mt19937::result_type> random_bird_direction;
};

#endif