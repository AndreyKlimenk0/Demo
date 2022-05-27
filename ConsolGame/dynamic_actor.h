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
	DynamicActor(u8 symbol, u32 update_time, IGameUpdater *game_updater);

	void run_thread();
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

protected:
	IGameUpdater *game_updater;

private:
	/**
	* @brief a derived class must overload the method and realize own game logic for updating tile position,
	* the method is called by update method automatically through some time.
	* @return void
	*/
	virtual void update_position(Direction &direction, u32 &tiles_number) = 0;

	std::thread tile_thread;
	std::condition_variable cv;
	std::chrono::milliseconds update_time;
};

#endif