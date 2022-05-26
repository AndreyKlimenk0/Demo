#include "dynamic_actor.h"
#include "game.h"


static std::mutex global_mutex;


DynamicActor::DynamicActor(u8 symbol, u32 tiles_count, u32 update_time, IGameUpdater *game_updater) : Actor(symbol), tiles_count(tiles_count),
	update_time(update_time), game_updater(game_updater)
{
}

void DynamicActor::run_thread()
{
	this->tile_thread = std::thread(&DynamicActor::update, this);
}

void DynamicActor::shutdown()
{
	cv.notify_all();
	if (tile_thread.joinable()) {
		tile_thread.join();
	}
}

void DynamicActor::update()
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lck(mtx);

	while ((cv.wait_for(lck, update_time) == std::cv_status::timeout)) {
		update_position();
		global_mutex.lock();
		game_updater->update(this);
		global_mutex.unlock();
	}
}