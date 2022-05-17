#include <iostream>
#include <cstdint>
#include <cstdio>
#include <random>
#include <string>
#include <chrono>
#include <Windows.h>
#include <thread>

typedef std::uint8_t u8;
typedef std::uint32_t u32;

const u32 MAX_GRID_SIZE = 255;

class GameEntity {
public:
	GameEntity(u8 char_symbol);
	void init(u8 tiles_count, u32 update_time, const GameEntity *entity = nullptr);
	void set_position(u8 new_position);
	void update();
	
	u8 get_symbol() const;
	u8 get_position() const;
	std::chrono::milliseconds get_update_time() const;
private:
	virtual void update_position() = 0;

	u8 char_symbol;
	u8 position;
	std::chrono::milliseconds update_time;
protected:
	u8 tiles_count;
	std::random_device dev;
	std::mt19937 rng;
};

GameEntity::GameEntity(u8 char_symbol) : char_symbol(char_symbol)
{
}

void GameEntity::init(u8 tiles_count, u32 update_time, const GameEntity *entity)
{
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

void GameEntity::update()
{
	static std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	static std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	static std::chrono::nanoseconds accumulater(0);
	static std::chrono::nanoseconds zero(0);

	begin = std::chrono::steady_clock::now();

	auto b = std::chrono::time_point_cast<std::chrono::nanoseconds>(begin);
	auto e = std::chrono::time_point_cast<std::chrono::nanoseconds>(end);

	std::chrono::nanoseconds elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(begin - end);

	accumulater += elapsed_time;

	if (accumulater >= get_update_time()) {
		update_position();
		accumulater = zero;
	}

	end = std::chrono::steady_clock::now();
}

void GameEntity::set_position(u8 new_position)
{
	position = new_position;
}

u8 GameEntity::get_symbol() const
{
	return char_symbol;
}

u8 GameEntity::get_position() const
{
	return position;
}

std::chrono::milliseconds GameEntity::get_update_time() const
{
	return update_time;
}

class Bird : public GameEntity {
public:
	Bird();
private:
	void update_position();
	std::uniform_int_distribution<std::mt19937::result_type> random_bird_direction;
};

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

Bird::Bird() : GameEntity('~'), random_bird_direction(0, 1)
{
}

class Sign : public GameEntity {
public:
	Sign();
	bool was_shot() const;
private:
	bool shot;
	void update_position();
};

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

Sign::Sign() : GameEntity('x')
{
}

class Tile : virtual public Bird, virtual public Sign {
public:
	void init(u8 tiles_count, u32 bird_update_time, u32 sign_update_time);
	void run();
private:
	void fill_tiles();
	void output_tiles();

	u8 tiles_count;
	u8 tiles[MAX_GRID_SIZE];
};

void Tile::init(u8 tiles_count, u32 bird_update_time, u32 sign_update_time)
{
	this->tiles_count = tiles_count;

	Bird::init(tiles_count, bird_update_time);
	Bird *bird = dynamic_cast<Bird *>(this);
	Sign::init(tiles_count, sign_update_time, bird);

	fill_tiles();
}

void Tile::run()
{
	while (1) {

		std::thread t1(&GameEntity::update, dynamic_cast<Bird *>(this));
		std::thread t2(&GameEntity::update, dynamic_cast<Sign *>(this));
		t1.join();
		t2.join();

		fill_tiles();
		output_tiles();
	}
}
void Tile::fill_tiles()
{
	u8 tile_symbol = '_';

	for (u8 i = 0; i < tiles_count; i++) {
		tiles[i] = tile_symbol;
	}
	
	if (Bird::get_position() == Sign::get_position()) {
		tiles[Bird::get_position()] = '$';
		if (Sign::was_shot()) {
			system("cls");
			std::cout << "YOU WIN" << std::endl;
			std::getchar();
			std::exit(1);
			
		}
	} else {
		tiles[Bird::get_position()] = Bird::get_symbol();
		tiles[Sign::get_position()] = Sign::get_symbol();
	}
}

void Tile::output_tiles()
{
	system("cls");
	for (u8 i = 0; i < tiles_count; i++) {
		std::cout << tiles[i];
	}
	std::cout << std::endl;

	std::cout << "Bird position = " << (u32)Bird::get_position() << std::endl;
	std::cout << "Sing position = " << (u32)Sign::get_position() << std::endl;
}


//int main()
//{
//	u32 tiles_count;
//	std::cout << "Enter number of tiles." << std::endl;
//	std::cin >> tiles_count;
//
//	u32 bird_update_time;
//	std::cout << "Enter bird update time" << std::endl;
//	std::cin >> bird_update_time;
//
//	u32 sign_update_time;
//	std::cout << "Enter sign update time" << std::endl;
//	std::cin >> sign_update_time;
//
//	Tile tile;
//	tile.init(tiles_count, bird_update_time, sign_update_time);
//	tile.run();
//}
