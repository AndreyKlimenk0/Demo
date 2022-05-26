#include "game.h"


static bool update_game;
static std::mutex global_mutex;


void Actor::set_symbol(u8 new_symbol)
{
	symbol = new_symbol;
}

void Actor::set_position(u32 new_position)
{
	position = new_position;
}

u8 Actor::get_symbol() const
{
	return symbol;
}

u32 Actor::get_position() const
{
	return position;
}


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

	while((cv.wait_for(lck, get_update_time()) == std::cv_status::timeout) && run_tread) {
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

Bird::Bird() : DynamicActor('~'), random_bird_direction(0, 1)
{
}

void Bird::update_position()
{
	u8 direction = static_cast<u8>(random_bird_direction(rng));
	if (direction == 0) {
		if (get_position() > 0) {
			set_position(get_position() - 1);
		}
	}
	else if (direction == 1) {
		if (get_position() < (tiles_count - 1)) {
			set_position(get_position() + 1);
		}
	}
}

Sign::Sign() : DynamicActor('x'), shot(false)
{
}

bool Sign::was_shot() const
{
	return shot;
}

void Sign::update_position()
{
	if (GetAsyncKeyState(VK_LEFT)) {
		if (get_position() > 0) {
			set_position(get_position() - 1);
		}
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		if (get_position() < (tiles_count - 1)) {
			set_position(get_position() + 1);
		}
	}

	if (GetAsyncKeyState(VK_SPACE)) {
		shot = true;
	} else {
		shot = false;
	}
}

void Game::init(u32 tiles_count, u32 bird_update_time, u32 sign_update_time)
{
	end_game = false;
	update_game = true;

	bird.init(tiles_count, bird_update_time, nullptr, static_cast<IGameUpdater *>(this));
	sign.init(tiles_count, sign_update_time, &bird, static_cast<IGameUpdater *>(this));

	tiles.resize(tiles_count);
	drawn_tiles.resize(tiles_count);
	
	for (u32 i = 0; i < drawn_tiles.size(); i++) {
		drawn_tiles[i] = dynamic_cast<Actor *>(&tiles.data()[i]);
	}
	
	drawn_tiles[bird.get_position()] = &bird;
	drawn_tiles[sign.get_position()] = &sign;
}

void Game::run()
{
	std::unique_lock<std::mutex> lk(cv_mutex);
	while (!end_game) {
		cv.wait(lk, []{ return update_game; });
		output_tiles();
		update_game = false;
	}
	shutdown();
	std::cout << "You Win!!!" << std::endl;
	std::getchar();

}

void Game::update(DynamicActor *actor)
{
	for (u32 i = 0; i < drawn_tiles.size(); i++) {
		drawn_tiles[i] = dynamic_cast<Actor *>(&tiles.data()[i]);
	}

	Bird *temp_bird = dynamic_cast<Bird *>(actor);
	if (temp_bird) {
		if (temp_bird->get_position() == sign.get_position()) {
			if (sign.was_shot()) {
				end_game = true;
			} else {
				drawn_tiles[sign.get_position()] = &goal;
			}
		} else {
			drawn_tiles[bird.get_position()] = &bird;
			drawn_tiles[sign.get_position()] = &sign;
		}
	}

	Sign *temp_sign = dynamic_cast<Sign *>(actor);
	if (temp_sign) {
		if (temp_sign->get_position() == bird.get_position()) {
			if (temp_sign->was_shot()) {
				end_game = true;
			} else {
				drawn_tiles[bird.get_position()] = &goal;
			}
		} else {
			drawn_tiles[bird.get_position()] = &bird;
			drawn_tiles[sign.get_position()] = &sign;
		}
	}

	update_game = true;
	cv.notify_one();
}

void Game::shutdown()
{
	bird.shutdown();
	sign.shutdown();
}

void Game::fill_tiles()
{
}

void Game::output_tiles()
{
	system("cls");
	for (u32 i = 0; i < drawn_tiles.size(); i++) {
		std::cout << drawn_tiles[i]->get_symbol();
	}
	std::cout << std::endl;
}

int main()
{
	Game game;
	game.init(20, 100, 100);
	game.run();
	return 0;
}