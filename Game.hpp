#pragma once
#include "SFML/Graphics.hpp"
#include <stdlib.h>
#include <iostream>
#include <vector>

struct POINT
{
	int x,y;
};
struct COORD
{
	float x, y;
};

class Game
{
private:
	
	enum axes{
		onVertical,onHorizontal
	};
	enum positions{
		above=1,below,start_game
	};
	enum events{
		countCandies,destroyCandies
	};

	sf::RenderWindow window;
	sf::Texture imgCandy[6];
	
	struct Candy
	{
		sf::Sprite	sprite;
		COORD		coord;			//relative la mijlocul bomboanei
		int			noCandy;
		int			swaped = 0;		// 0-false  1-candy moving above 2-candy moving below 
		bool 		marked = false;

	}M[8][8];


	struct{
		bool pressed = false;
		POINT coordM;
	}mouse;


	void initializareMatrice(void);
	void incarcareImagini(void);
	void handleEvents(void);
	void setPositonOfSprites(void);
	void generateCandy(POINT coordM);

	void draw(POINT FirstCandy, POINT SecondCandy);
	void draw();
	POINT parseCoord(POINT coord);
	
	void swapCandies(sf::Event e);
	
	bool DestroyCandiesAnimation(std::vector<POINT> coordM);
	POINT CountAndMarkCandies(POINT coordM);
	void MarkedCandiesEvent(bool destroy=true);
	
	bool ReplaceDestroyedCandiesEvent(void);
	void AfisareMatrice(void);

	bool CautareDeFormatiiInMatrice();

	bool swapAnimation(POINT coordMSelectedCandy, POINT coordMToSwapCandy);
	bool moveAboveAnimation(POINT coordM, POINT direction, float current , float goal);
	bool moveOnLineAnimation(POINT coordM,POINT direction);


public:
	friend std::ostream& operator<<(std::ostream& out, const POINT& P);
	friend bool operator == (POINT v1, POINT v2);

	Game(unsigned int width, unsigned height);
	void play(void);
};

