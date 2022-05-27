#include "game.h"


static bool update_game;

Game::Game(u32 tiles_count, u32 bird_update_time, u32 sign_update_time) :
	tiles_count(tiles_count), bird(tiles_count, bird_update_time, static_cast<IGameUpdater *>(this)), 
	sign(tiles_count, sign_update_time, static_cast<IGameUpdater *>(this)), end_game(false)
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
	update_mutex.lock();

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

	update_mutex.unlock();
}

void Game::shutdown()
{
	bird.shutdown();
	sign.shutdown();
}

void Game::output_tiles()
{
	system("cls");
	for (u32 i = 0; i < drawn_tiles.size(); i++) {
		std::cout << drawn_tiles[i]->get_symbol();
	}
	std::cout << std::endl;
}

void Game::generate_random_places_for_bird_and_sign()
{
	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, tiles_count - 1);

	u32 bird_position = static_cast<u32>(dist6(rng));

	bird.set_position(bird_position);
	
	u32 sign_position = static_cast<u32>(dist6(rng));

	while (sign_position == bird_position) {
		sign_position = static_cast<u32>(dist6(rng));
	}

	sign.set_position(sign_position);
}
