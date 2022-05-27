#ifndef VIEW_H
#define VIEW_H

#include <vector>

class Game;
class View {
public:
	void diplay_win() const;
	void diplay(const std::vector<char> &data) const;
};

#endif