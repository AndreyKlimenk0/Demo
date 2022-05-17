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


class Tile {
public:
	Tile(u8 symbol);
	void init(u32 tiles_count, u32 update_time, const Tile *entity);
	void shutdown();
	void update();
	void set_position(u32 new_position);
	
	u8  get_symbol() const;
	u32 get_position() const;
	std::chrono::milliseconds get_update_time() const;

protected:
	u32 tiles_count;
	std::random_device dev;
	std::mt19937 rng;

private:
	virtual void update_position() = 0;

	u8 symbol;
	u32 position;
	
	bool run_tread;
	std::thread tile_thread;
	std::condition_variable cv;
	std::chrono::milliseconds update_time;
};

Tile::Tile(u8 symbol) : symbol(symbol), run_tread(true)
{
}

void Tile::init(u32 tiles_count, u32 update_time, const Tile *entity)
{
	this->tile_thread = std::thread(&Tile::update, this);
	this->tiles_count = tiles_count;
	this->update_time = std::chrono::milliseconds(update_time);

	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, tiles_count - 1);

	u8 random_position = static_cast<u8>(dist6(rng));

	if (entity) {
		while (random_position == entity->get_position()) {
			random_position = static_cast<u8>(dist6(rng));
		}
	}
	position = random_position;
}

void Tile::shutdown()
{
	run_tread = false;
	tile_thread.join();
}

void Tile::update()
{
	std::mutex mtx;
	std::unique_lock<std::mutex> lck(mtx);

	while((cv.wait_for(lck, get_update_time()) == std::cv_status::timeout) && run_tread) {
		update_position();
	}
}

void Tile::set_position(u32 new_position)
{
	position = new_position;
}

u8 Tile::get_symbol() const
{
	return symbol;
}

u32 Tile::get_position() const
{
	return position;
}

std::chrono::milliseconds Tile::get_update_time() const
{
	return update_time;
}

class Bird : public Tile {
public:
	Bird();
private:
	void update_position();
	std::uniform_int_distribution<std::mt19937::result_type> random_bird_direction;
};

Bird::Bird() : Tile('~'), random_bird_direction(0, 1)
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

class Sign : public Tile {
public:
	Sign();
	bool was_shot() const;
private:
	void update_position();
	bool shot;
};

Sign::Sign() : Tile('x'), shot(false)
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

class Game {
public:
	Game() {};
	void init(u32 tiles_count, u32 bird_update_time, u32 sign_update_time);
	void run();
	void shutdown();
private:
	void output_tiles();
	void fill_tiles();

	u32 bird_positin;
	Bird bird;
	Sign sign;
	std::vector<u8> tiles;
};

void Game::init(u32 tiles_count, u32 bird_update_time, u32 sign_update_time)
{
	bird.init(tiles_count, bird_update_time, nullptr);
	sign.init(tiles_count, sign_update_time, &bird);

	tiles.resize(tiles_count);
	fill_tiles();
}

void Game::run()
{
	while (1) {
		fill_tiles();
		output_tiles();
	}
}

void Game::shutdown()
{
	bird.shutdown();
	sign.shutdown();
	std::exit(1);
}

void Game::fill_tiles()
{
	for (auto &item : tiles) {
		item = '_';
	}

	if (bird.get_position() == sign.get_position()) {
		tiles[sign.get_position()] = '$';
		if (sign.was_shot()) {
			system("cls");
			std::cout << "YOU WIN" << std::endl;
			std::getchar();
			shutdown();
		}
	}
	else {
		tiles[bird.get_position()] = bird.get_symbol();
		tiles[sign.get_position()] = sign.get_symbol();
	}
}

void Game::output_tiles()
{
	system("cls");
	for (auto item : tiles) {
		std::cout << item;
	}
	std::cout << std::endl;
}

int main()
{
	Game game;
	game.init(20, 1000, 100);
	game.run();
}