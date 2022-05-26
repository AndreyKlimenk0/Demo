#include "dynamic_actor.h"
#include "game.h"


static std::mutex global_mutex;


DynamicActor::DynamicActor(u8 symbol) : run_tread(true), game_updater(nullptr)
{
	set_symbol(symbol);
}

void DynamicActor::init(u32 tiles_count, u32 update_time, const DynamicActor *tile, IGameUpdater *game_updater)
{
	this->tiles_count = tiles_count;
	this->update_time = std::chrono::milliseconds(update_time);
	this->game_updater = game_updater;

	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, tiles_count - 1);

	u8 random_position = static_cast<u8>(dist6(rng));

	if (tile) {
		while (random_position == tile->get_position()) {
			random_position = static_cast<u8>(dist6(rng));
		}
	}
	set_position(random_position);

	this->tile_thread = std::thread(&DynamicActor::update, this);
}

void DynamicActor::shutdown()
{
	run_tread = false;
	tile_thread.join();
}

void DynamicActor::update()
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lck(mtx);

	while ((cv.wait_for(lck, get_update_time()) == std::cv_status::timeout) && run_tread) {
		update_position();
		global_mutex.lock();
		game_updater->update(this);
		global_mutex.unlock();
	}
}

std::chrono::milliseconds DynamicActor::get_update_time() const
{
	return update_time;
}
