#ifndef SIGN_H
#define SIGN_H

#include "dynamic_actor.h"

/**
* @brief class realizes sign mechnies for a user. it uses user input in order to toka aim on bird tile and make a shot.
*/
class Sign : public DynamicActor {
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

#endif