#include "view.h"
#include <iostream>


void View::diplay_win() const
{
	system("cls");
	std::cout << "You Win!!!" << std::endl;
}

void View::diplay(const std::vector<char>& data) const
{
	system("cls");
	for (char c : data) {
		std::cout << c;
	}
	std::cout << std::endl;
}
