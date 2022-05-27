#include "game.h"


static bool update_game;

Game::Game(u32 tiles_count, u32 bird_update_time, u32 sign_update_time) :
	tiles_count(tiles_count), bird(bird_update_time, static_cast<IGameUpdater *>(this)), 
	sign(sign_update_time, static_cast<IGameUpdater *>(this)), end_game(false), bird_position(0), sign_position(0)
{
	update_game = true;
}

void Game::init()
{
	generate_random_places_for_bird_and_sign();
	bird.run_thread();
	sign.run_thread();

	tiles.resize(tiles_count);
	drawn_tiles.resize(tiles_count);
	
	for (u32 i = 0; i < drawn_tiles.size(); i++) {
		drawn_tiles[i] = tiles.data()[i].get_symbol();
	}
	
	drawn_tiles[bird_position] = bird.get_symbol();
	drawn_tiles[sign_position] = sign.get_symbol();
}

void Game::run()
{
	std::unique_lock<std::mutex> lk(cv_mutex);
	while (!end_game) {
		cv.wait(lk, []{ return update_game; });
		view.diplay(drawn_tiles);
		update_game = false;
	}
	shutdown();
	view.diplay_win();
	std::getchar();

}

void Game::update(DynamicActor *actor, int tiles_offset)
{
	update_mutex.lock();

	for (u32 i = 0; i < drawn_tiles.size(); i++) {
		drawn_tiles[i] = tiles.data()[i].get_symbol();
	}

	u32 *ptr = nullptr;

	if (dynamic_cast<Bird *>(actor)) {
		ptr = &bird_position;
	} else if (dynamic_cast<Sign *>(actor)) {
		ptr = &sign_position;
	}

	int result = *ptr + tiles_offset;
	if ((result >= 0) && (result < tiles_count)) {
		*ptr = result;
	}

	Bird *temp_bird = dynamic_cast<Bird *>(actor);
	if (temp_bird) {
		if (bird_position == sign_position) {
			if (sign.was_shot()) {
				end_game = true;
			} else {
				drawn_tiles[sign_position] = goal.get_symbol();
			}
		} else {
			drawn_tiles[bird_position] = bird.get_symbol();
			drawn_tiles[sign_position] = sign.get_symbol();
		}
	}

	Sign *temp_sign = dynamic_cast<Sign *>(actor);
	if (temp_sign) {
		if (sign_position == bird_position) {
			if (temp_sign->was_shot()) {
				end_game = true;
			} else {
				drawn_tiles[bird_position] = goal.get_symbol();
			}
		} else {
			drawn_tiles[bird_position] = bird.get_symbol();
			drawn_tiles[sign_position] = sign.get_symbol();
		}
	}

	update_game = true;
	cv.notify_one();

	update_mutex.unlock();
}

void Game::shutdown()
{
	bird.shutdown();
	sign.shutdown();
}

void Game::generate_random_places_for_bird_and_sign()
{
	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, tiles_count - 1);

	u32 random_bird_position = static_cast<u32>(dist6(rng));

	bird_position = random_bird_position;
	
	u32 random_sign_position = static_cast<u32>(dist6(rng));

	while (random_sign_position == bird_position) {
		random_sign_position = static_cast<u32>(dist6(rng));
	}

	sign_position = random_bird_position;
}
