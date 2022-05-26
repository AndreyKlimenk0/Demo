#ifndef DYNAMIC_ACTOR
#define DYNAMIC_ACTOR


#include <random>
#include <chrono>
#include <Windows.h>
#include <thread>
#include <condition_variable>

#include "actor.h"
#include "helper.h"

class IGameUpdater;

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

#endif