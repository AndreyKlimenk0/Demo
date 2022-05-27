#ifndef BIRD_H
#define BIRD_H

#include <random>
#include "dynamic_actor.h"
#include "helper.h"


class IGameUpdater;
/**
* @brief class realize bird game mechenics. on a tiles line there is a bird tile with symbol '~'. the bird moves in random direction per set time.
*/
class Bird : public DynamicActor {
public:
	Bird(u32 update_time, IGameUpdater *game_updater);
private:
	/**
	* @brief updates a position in one tile in random direction (left or right side).
	* @return void
	*/
	void update_position(int &tiles_offset) override;

	std::random_device dev;
	std::mt19937 rng;
	std::uniform_int_distribution<std::mt19937::result_type> random_bird_direction;
};

#endif