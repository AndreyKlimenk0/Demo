#include "game.h"


static bool update_game;


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

void Game::output_tiles()
{
	system("cls");
	for (u32 i = 0; i < drawn_tiles.size(); i++) {
		std::cout << drawn_tiles[i]->get_symbol();
	}
	std::cout << std::endl;
}