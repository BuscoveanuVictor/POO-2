#pragma once
#include "SFML/Graphics.hpp"
#include <stdlib.h>

struct POINT
{
	int x,y;
};
struct COORD
{
	float x, y;
};

enum {
	onHorizontal,onVertical
};
enum {
	above=1,below
};


class Game
{
private:

	sf::RenderWindow window;
	sf::Texture imgCandy[6];
	
	struct Candy
	{
		sf::Sprite	sprite;
		COORD		coord;		//relative la mijlocul bomboanei
		int			noCandy;
		int			swaped = 0;		// 0-false  1-candy moving above 2-candy moving below 
		bool		destroyed = false;
		bool		toggled = false;

	}
	M[8][8];


	struct{
		bool pressed = false;
		POINT coordM;
	}mouse;


	void initializareMatrice(void);
	void incarcareImagini(void);
	void handleEvents(void);
	void setPositonOfSprites(void);

	void draw();
	POINT parseCoord(POINT coord);
	void mouseDrag(int CoordX, int CoordY);
	void swapCandies(sf::Event e);
	int CountCandiesOnLine(POINT coordM,const int axes);
	void DestroyCandiesOnLine(POINT coordM, const int axes);
	POINT CountCandies(POINT coordM);

	bool swapAnimation(POINT coordMSelectedCandy, POINT coordMToSwapCandy);
	bool moveAboveAnimation(POINT coordM, POINT direction, float current , float goal);
	bool moveOnLineAnimation(POINT coordM,POINT direction);


public:
	Game(unsigned int width, unsigned height);
	void play(void);
};

