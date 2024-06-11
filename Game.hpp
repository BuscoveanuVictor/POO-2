#pragma once
#include "SFML/Graphics.hpp"
#include <stdlib.h>
#include <iostream>
#include <vector>


#define	MDIM				7		//Dim matrice
#define WIDTH_CANDY			133
#define HEIGHT_CANDY    	138
#define SCALE				0.5f
#define SECUNDA				1000	//Milisec
#define TIME_EVENT_SWAP		10		//Milisec -> repezinta timpul cu care se intampla un event de tip swap
#define TIME_EVENT_DESTROY	5		


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

	}M[MDIM][MDIM];

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

