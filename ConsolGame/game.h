#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <condition_variable>
#include <vector>

#include "actor.h"
#include "dynamic_actor.h"
#include "tile.h"
#include "bird.h"
#include "sign.h"
#include "goal.h"
#include "helper.h"
#include "view.h"


class DynamicActor;

class IGameUpdater {
public:
	virtual void update(DynamicActor *actor, int tiles_offset) = 0;
};

/**
* @brief class initialize and run a game it take number tiles in the game and time per which will update bird and sign game entiry.
*/
class Game : public IGameUpdater {
public:
	Game(u32 tiles_count, u32 bird_update_time, u32 sign_update_time);
	/**
	* @brief init bird and sign classed, fills tiles line.
	* @param tells how many tiles will be in game.
	* @param through this time update bird position
	* @param through this time update sign position
	* @return void
	*/
	void init();
	/**
	* @brief run main loop where update bird and sign position on tiles line
	* @return void
	*/
	void run();

	void update(DynamicActor *actor, int tiles_offset);

private:
	/**
	* @brief calls shotdown methods in bird and sign classes and exit the program
	* @return void
	*/
	void shutdown();
	/**
	* @brief outputs tiles in a windows console
	* @return void
	*/

	void generate_random_places_for_bird_and_sign();

	bool end_game;
	u32 tiles_count;
	u32 bird_position;
	u32 sign_position;
	Bird bird;
	Sign sign;
	Goal goal;
	std::vector<Tile> tiles;
	std::vector<char> drawn_tiles;

	std::condition_variable cv;
	std::mutex cv_mutex;
	std::mutex update_mutex;

	std::random_device dev;
	std::mt19937 rng;

	View view;
};
#endif