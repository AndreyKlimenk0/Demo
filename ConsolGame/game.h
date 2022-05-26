#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <cstdint>
#include <random>
#include <string>
#include <chrono>
#include <Windows.h>
#include <thread>
#include <condition_variable>
#include <vector>


typedef std::uint8_t u8;
typedef std::uint32_t u32;

class DynamicActor;

class IGameUpdater {
public:
	virtual void update(DynamicActor *actor) = 0;
};

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

class Tile : public Actor {
public:
	Tile() : Actor('_') {};
};

class Goal : public Actor {
public:
	Goal() : Actor('$') {};
};

/**
* @brief Tile class is a base class. the class helps to make interactive tile objects with own game mechenics.
		 when you inherite from this class you must overload update_position method and describe own game mechenics.
*/
class DynamicActor : public Actor {
public:
	DynamicActor(u8 symbol);

	/**
	* @brief init member fileds, run update method in a new thread and generate random place for tile in tiles line.
	*
	* @param tiles_count is additional information is used in inherited classed.
	* @param update_time teakes milliseconds and tells in how many time to call method update_position.
	* @param tile when it is nullputr the function simple generate new position
			 if it is not nullptr the function generate position which does not intersect with tile->position
	* @return void
	*/
	void init(u32 tiles_count, u32 update_time, const DynamicActor *tile, IGameUpdater *game_updater);
	/**
	* @brief stop infinity loop in update method and join the thread.
	* @return void
	*/
	void shutdown();
	/**
	* @brief uses update_time member in order to call through specified time update_postion method;
	* @return void
	*/
	void update();
	
	std::chrono::milliseconds get_update_time() const;

protected:
	u32 tiles_count;
	std::random_device dev;
	std::mt19937 rng;
	IGameUpdater *game_updater;

private:
	/**
	* @brief a derived class must overload the method and realize own game logic for updating tile position,
	* the method is called by update method automatically through some time.
	* @return void
	*/
	virtual void update_position() = 0;

	bool run_tread;
	std::thread tile_thread;
	std::condition_variable cv;
	std::chrono::milliseconds update_time;
};

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

/**
* @brief class realizes sign mechnies for a user. it uses user input in order to toka aim on bird tile and make a shot.
*/
class Sign : public DynamicActor {
public:
	Sign();
	/**
	* @return true if user pressed space key, false if user did not press space key.
	*/
	bool was_shot() const;
private:
	/**
	* @brief updates a position on one tile based on user input from keyboard (left or right arrow keys).
	* @return void
	*/
	void update_position();
	bool shot;
};

/**
* @brief class initialize and run a game it take number tiles in the game and time per which will update bird and sign game entiry.
*/
class Game : public IGameUpdater {
public:
	Game() {};
	/**
	* @brief init bird and sign classed, fills tiles line.
	* @param tells how many tiles will be in game.
	* @param through this time update bird position
	* @param through this time update sign position
	* @return void
	*/
	void init(u32 tiles_count, u32 bird_update_time, u32 sign_update_time);
	/**
	* @brief run main loop where update bird and sign position on tiles line
	* @return void
	*/
	void run();

	void update(DynamicActor *actor);

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
	void output_tiles();
	/**
	* @brief updates bird and sign position in tiles line and checks if was shot from an user and whether the user got.
	* @return void
	*/
	void fill_tiles();

	bool end_game;
	u32 bird_positin;
	Bird bird;
	Sign sign;
	Goal goal;
	std::vector<Tile> tiles;
	std::vector<Actor *> drawn_tiles;

	std::condition_variable cv;
	std::mutex cv_mutex;
};
#endif