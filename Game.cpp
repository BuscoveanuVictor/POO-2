#include "Game.h"
#include "iostream"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include "SFML/Graphics.hpp"

#define	MDIM			7		//Dim matrice
#define WIDTH_CANDY		133
#define HEIGHT_CANDY    138
#define SCALE			0.5f
#define SECUNDA			1000	//Milisec
#define TIME_EVENT		10		//Milisec -> repezinta timpul cu care se intampla un event

bool operator == (POINT v1, POINT v2)
{
	return (v1.x - v2.x) + (v1.y - v2.y) == 0;
}

/*
	Se initializeaza structurile de date candy cu numarul
	bomboanei si textura(poza) corespunzatoare
*/
void Game::initializareMatrice(void) 
{
	srand(time(0));
	for (int i = 0; i < MDIM; i++)
		for (int j = 0; j < MDIM; j++)
		{
			M[i][j].noCandy = rand() % 6;
			M[i][j].sprite.setTexture(imgCandy[M[i][j].noCandy]); // Spriteul primeste poza coresp bomboanei
			M[i][j].sprite.setScale(SCALE, SCALE); // se modifica dim imaginii(sa fie mai mica)

			// Coord mijlocului bomboanei
			M[i][j].coord.x = (i * (WIDTH_CANDY  * SCALE)) + (WIDTH_CANDY  * SCALE) / 2;
			M[i][j].coord.y = (j * (HEIGHT_CANDY * SCALE)) + (HEIGHT_CANDY * SCALE) / 2;

			M[i][j].sprite.setScale({ SCALE,SCALE });

			// se stabilesc coord fiecarui sprite relativ la coord bomboanei
		/*	M[i][j].sprite.setPosition
			(M[i][j].coord.x - (WIDTH_CANDY * SCALE) / 2, 
				M[i][j].coord.y - (HEIGHT_CANDY * SCALE) / 2);*/
		}
}

void Game::incarcareImagini(void) 
{	

	sf::Image candies;
	if (!candies.loadFromFile("images/candies.png")) 
	{
		std::cout << "ERROR 404";
		return;
	}
	for (int i = 0; i < 6; i++) 
		imgCandy[i].loadFromImage
			(candies, sf::IntRect(i * WIDTH_CANDY, 0, WIDTH_CANDY, HEIGHT_CANDY));
}

void Game::handleEvents(void) 
{
	sf::Event event;

	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			switch (event.type) 
			{
				case sf::Event::Closed:
					window.close();
					break;

				case sf::Event::MouseButtonPressed:
					mouse.pressed = true;
					mouse.coordM = parseCoord({ event.mouseButton.x, event.mouseButton.y });
					break;

				case sf::Event::MouseButtonReleased:
					mouse.pressed = false;
					break;

				case sf::Event::MouseMoved:
					swapCandies(event);
					break;
			}
		}
	}
}

void Game::draw()
{
	window.clear(sf::Color::Cyan);

	for (int i = 0; i < MDIM; i++)
		for (int j = 0; j < MDIM; j++)
		{
			M[i][j].sprite.setPosition
			(M[i][j].coord.x - (WIDTH_CANDY *  M[i][j].sprite.getScale().x) / 2,
				M[i][j].coord.y - (HEIGHT_CANDY * M[i][j].sprite.getScale().y) / 2);

			window.draw(M[i][j].sprite);
		}

	window.display();
}


POINT Game::parseCoord(POINT coord)
{
	POINT res;

	res.x = (coord.x + 1) / (WIDTH_CANDY * SCALE);
	res.y = (coord.y + 1) / (HEIGHT_CANDY * SCALE);

	return res;

}

void Game::swapCandies(sf::Event e)
{
	if (mouse.pressed)
	{
		POINT currentPos = parseCoord({e.mouseMove.x,e.mouseMove.y});
		
		if (!(currentPos == mouse.coordM))
		{

			POINT previousPos = mouse.coordM; // coord obtinute la eventimentul click(mouse down)		

			std::cout << "Pe asta sunt cu mouse ul ->" << '(' << currentPos.x << ',' << currentPos.y << ')'
				<< " fac swap cu ala pe care am fost " << "  (" << previousPos.x << ',' << previousPos.y << ')' << '\n';
			
			// Pana aici am obtinut info despre coord in M ale bomboanelor care trebuie sa fie schimbate(swap) //


			// Mai departe trebuie sa se realizeze animatia de swap indifierent daca se distrung //
			// sau nu niste bomboane, mai apoi vom face aceasta verificare //

			
			if (swapAnimation(previousPos, currentPos)) 
			{
				mouse.pressed = false;

				std::swap(M[currentPos.x][currentPos.y],
					M[mouse.coordM.x][mouse.coordM.y]);
			}
		

			// Aici se va face algoritmul de destroy 

			/*


			
			POINT counter;
			counter = CountCandies(currentPos);
			
			std::cout << counter.x << ' ' << counter.y << '\n';
			mouse.pressed = false;
			draw();*/
		}
	}
}


bool Game::swapAnimation(POINT coordMSelectedCandy, POINT coordMToSwapCandy)
{
	sf::Clock clock = sf::Clock::Clock();
	sf::Time previousTime;							// secunda 0 
	sf::Time currentTime = clock.getElapsedTime();	// timpul trecut 

	bool isFinishedMoveAbove = false;
	bool isFinishedMoveBelow = false;

	int x = coordMSelectedCandy.x - coordMToSwapCandy.x;
	int y = coordMSelectedCandy.y - coordMToSwapCandy.y;

	POINT direction = { x, y };

	while (1) {
		currentTime = clock.getElapsedTime();
		
		// la fiecare TIME_EVENT(10 milisecunde) se intampla ce e mai jos
		if ( currentTime.asMilliseconds() - previousTime.asMilliseconds() == TIME_EVENT)
		{
			
			// Bomboana care trebuie sa treaca pe deasupra
			if (!isFinishedMoveAbove)
			{
				isFinishedMoveAbove = moveAboveAnimation(coordMSelectedCandy, direction, SCALE, 0.7f);
			}
			// Bomboana care trebuie sa treaca pe dedesupt
			if (!isFinishedMoveBelow)
			{
				isFinishedMoveBelow = moveOnLineAnimation(coordMToSwapCandy, direction);
			}
		
			draw();
			previousTime = currentTime;

			if (isFinishedMoveAbove && isFinishedMoveBelow)return true;
		}
	}
}

bool Game::moveAboveAnimation(POINT coordM,POINT direction, float current,float goal)
{
	// se comporta ca niste variabile globale
	// acestea sunt initializate cu parametrii fct o ~singura data~
	static float currentScale = current;	
	static float goalScale = goal;
	static float inc = ((goal-current)*2)/((SECUNDA*.5f)/TIME_EVENT);

	// se mareste img
	currentScale += inc;
	// cand img a ajus la goalScale se revine la dim initialza schimband inc 
	if (currentScale >= goalScale)inc = -inc; 

	// se updateaza scale-ul sprite ului
	M[coordM.x][coordM.y].sprite.setScale(currentScale, currentScale);

	// se muta bomboana pe axele de coord
	M[coordM.x][coordM.y].coord.x -= direction.x * (WIDTH_CANDY * SCALE) / ((SECUNDA * .5f) / TIME_EVENT);
	M[coordM.x][coordM.y].coord.y -= direction.y * (HEIGHT_CANDY * SCALE) / ((SECUNDA * .5f) / TIME_EVENT);
	
	// dupa ce am scazut inc si am ajus inapoi la SCALE-ul normal 
	// se returneaza sugerand faptul ca eventul s-a terminat
	if (currentScale <= SCALE) {
		// intrucat inc se comporta ca o var globala el daca
		// nu revenea la forma init(+) cand se reintra in fct era neg
		inc = -inc; 
		return true;// true sugereaza faptul ca s-a terminat animatia
	}

	return false;
}


bool Game::moveOnLineAnimation(POINT coordM, POINT direction)
{		
	//std::cout << direction.x << ',' << direction.y << '\n';
	M[coordM.x][coordM.y].coord.x += direction.x * (WIDTH_CANDY  * SCALE) /((SECUNDA * .5f) / TIME_EVENT);
	M[coordM.x][coordM.y].coord.y += direction.y * (HEIGHT_CANDY * SCALE) /((SECUNDA * .5f) / TIME_EVENT);

	//std::cout << M[coordM.x][coordM.y].coord.y << '\n';

	// am adunat (width/height_candy*scale)/2 pentru ca tr determinat mijlocul bomboanei
	const int i = ((WIDTH_CANDY  * SCALE) * (coordM.x + direction.x)) + (WIDTH_CANDY  * SCALE) / 2;
	const int j = ((HEIGHT_CANDY * SCALE) * (coordM.y + direction.y)) + (HEIGHT_CANDY * SCALE) / 2;


	if ((direction.y == 1 && M[coordM.x][coordM.y].coord.y >= j) || (direction.y == -1 && M[coordM.x][coordM.y].coord.y <= j)) 
		return true;
	
	if ((direction.x == 1 && M[coordM.x][coordM.y].coord.x >= i) || (direction.x == -1 && M[coordM.x][coordM.y].coord.x <= i))
		return true;

	/*if (((direction.y & 1)&&(M[coordM.x][coordM.y].coord.y >= j)) || ((direction.y & (-1)) && (M[coordM.x][coordM.y].coord.y <= j)))
		return true;*/

	return false; 
}


POINT Game::CountCandies(POINT coordM)
{
	int counterX = 1 , counterY = 1;
	counterX += CountCandiesOnLine({ coordM.x, coordM.y }, onHorizontal);
	counterY += CountCandiesOnLine({ coordM.x, coordM.y }, onVertical);
	return { counterX, counterY };
}

int Game::CountCandiesOnLine(POINT coordM,const int axes)
{
	int i = coordM.x, j = coordM.y;
	M[i][j].toggled = true;

	// Daca e onHorizontal => axes = 0 , un ex: i-1+axes = i-1, nu se schimba nimic
	// Daca e onVertical => axes = 1 , un ex: i-1+axes = i => i ul va fi constant si se va schimba j ul

	if (i>0	&& M[i - 1 + axes][j - axes].noCandy == M[i][j].noCandy && !M[i - 1 + axes][j - axes].toggled)
	{
		return 1 + CountCandiesOnLine({ i - 1 + axes, j - axes }, axes);
	}
	if (i<MDIM && M[i + 1 - axes][j + axes].noCandy == M[i][j].noCandy && !M[i + 1 - axes][j + axes].toggled)
	{
		return 1 + CountCandiesOnLine({ i + 1 - axes, j + axes }, axes);
	}
	return 0;
}

void Game::DestroyCandiesOnLine(POINT coordM,const int axes)
{
	int i = coordM.x, j = coordM.y;
	M[i][j].toggled = false;

	// Daca e onHorizontal => axes = 0 , un ex: i-1+axes = i-1, nu se schimba nimic
	// Daca e onVertical => axes = 1 , un ex: i-1+axes = i => i ul va fi constant si se va schimba j ul

	if (i > 0 && M[i - 1 + axes][j - axes].toggled == M[i][j].noCandy && !M[i - 1 + axes][j - axes].toggled)
	{
		DestroyCandiesOnLine({ i - 1 + axes, j - axes },  axes);
	}
	if (i < MDIM && M[i + 1 - axes][j + axes].noCandy == M[i][j].noCandy && !M[i + 1 - axes][j + axes].toggled)
	{
		DestroyCandiesOnLine({ i + 1 - axes, j + axes }, axes);
	}

}


Game::Game(unsigned int width, unsigned height):window(sf::VideoMode(width,height),"Game")
{
	incarcareImagini();
	initializareMatrice();
}

void Game::play(void)
{
	draw();
	handleEvents();
}
