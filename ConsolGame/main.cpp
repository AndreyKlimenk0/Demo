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

/**
* @brief Tile class is a base class. the class helps to make interactive tile objects with own game mechenics.
		 when you inherite from this class you must overload update_position method and describe own game mechenics.
*/
class Tile {
public:
	Tile(u8 symbol);

	/**
	* @brief init member fileds, run update method in a new thread and generate random place for tile in tiles line.
	*
	* @param tiles_count is additional information is used in inherited classed.
	* @param update_time teakes milliseconds and tells in how many time to call method update_position.
	* @param tile when it is nullputr the function simple generate new position
			 if it is not nullptr the function generate position which does not intersect with tile->position
	* @return void
	*/
	void init(u32 tiles_count, u32 update_time, const Tile *tile);
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
	/**
	* @brief set a new position for tile in tiles line;
	* @return void
	*/
	void set_position(u32 new_position);

	/**
	* @return symbol which is used for drawing tiles line.
	*/
	u8  get_symbol() const;
	/**
	* @return tile position in tiles line.
	*/
	u32 get_position() const;
	/**
	* @return time which tells how soon to call update_position_method
	*/
	std::chrono::milliseconds get_update_time() const;

protected:
	u32 tiles_count;
	std::random_device dev;
	std::mt19937 rng;

private:
	/**
	* @brief a derived class must overload the method and realize own game logic for updating tile position,
	* the method is called by update method automatically through some time.
	* @return void
	*/
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

void Tile::init(u32 tiles_count, u32 update_time, const Tile *tile)
{
	this->tile_thread = std::thread(&Tile::update, this);
	this->tiles_count = tiles_count;
	this->update_time = std::chrono::milliseconds(update_time);

	std::uniform_int_distribution<std::mt19937::result_type> dist6(0, tiles_count - 1);

	u8 random_position = static_cast<u8>(dist6(rng));

	if (tile) {
		while (random_position == tile->get_position()) {
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

/**
* @brief class realize bird game mechenics. on a tiles line there is a bird tile with symbol '~'. the bird moves in random direction per set time.
*/
class Bird : public Tile {
public:
	Bird();
private:
	/**
	* @brief updates a position in one tile in random direction (left or right side).
	* @return void
	*/
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

/**
* @brief class realizes sign mechnies for a user. it uses user input in order to toka aim on bird tile and make a shot.
*/
class Sign : public Tile {
public:
	Sign();
	/**
	* @return true if user pressed space key, false if user did not press space key.
	*/
	bool was_shot() const;
private:
	/**
	* @brief updates a position on one tile based on user input from keyboard (left or right arrow keys).
	* @return void
	*/
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

/**
* @brief class initialize and run a game it take number tiles in the game and time per which will update bird and sign game entiry.
*/
class Game {
public:
	Game() {};
	/**
	* @brief init bird and sign classed, fills tiles line.
	* @param tells how many tiles will be in game.
	* @param through this time update bird position
	* @param through this time update sign position
	* @return void
	*/
	void init(u32 tiles_count, u32 bird_update_time, u32 sign_update_time);
	/**
	* @brief run main loop where update bird and sign position on tiles line
	* @return void
	*/
	void run();
private:
	/**
	* @brief calls shotdown methods in bird and sign classes and exit the program
	* @return void
	*/
	void shutdown();
	/**
	* @brief outputs tiles in a windows console
	* @return void
	*/
	void output_tiles();
	/**
	* @brief updates bird and sign position in tiles line and checks if was shot from an user and whether the user got.
	* @return void
	*/
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