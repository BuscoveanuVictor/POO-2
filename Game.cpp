#include "Game.hpp"
#include "iostream"
#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <vector>

#define	MDIM				7		//Dim matrice
#define WIDTH_CANDY			133
#define HEIGHT_CANDY    	138
#define SCALE				0.5f
#define SECUNDA				1000	//Milisec
#define TIME_EVENT_SWAP		10		//Milisec -> repezinta timpul cu care se intampla un event de tip swap
#define TIME_EVENT_DESTROY	5		


///// Functii ajutatoare //////

std::ostream& operator<<(std::ostream& out, const POINT& P){
	out << '(' << P.x << ' ' << P.y << ')' << '\n';
	return out;
}

bool operator == (POINT v1, POINT v2)
{
	return (v1.x - v2.x) + (v1.y - v2.y) == 0;
}

POINT Game::parseCoord(POINT coord)
{
	POINT res;

	res.x = (coord.x + 1) / (WIDTH_CANDY * SCALE);
	res.y = (coord.y + 1) / (HEIGHT_CANDY * SCALE);

	return res;

}

///////////////////////////////

void Game::AfisareMatrice(void)
{
	std::cout << '\n';
	for(int j=0; j<MDIM; j++)
	{
		for(int i=0; i<MDIM; i++){
			
			if(M[i][j].noCandy ==-1){
				std::cout << ' ';
			}else std::cout << "  ";
			std::cout << M[i][j].noCandy;
		}
		std::cout << '\n';
	}
	std::cout << '\n';
}

void Game::generateCandy(POINT coordM){
	
	int i=coordM.x, j=coordM.y;

	M[i][j].noCandy = rand() % 6;
	M[i][j].sprite.setColor(sf::Color::White);
	M[i][j].sprite.setTexture(imgCandy[M[i][j].noCandy]); // Spriteul primeste poza coresp bomboanei
	M[i][j].sprite.setScale(SCALE, SCALE); // se modifica dim imaginii(sa fie mai mica)

	// Coord mijlocului bomboanei
	M[i][j].coord.x = (i * (WIDTH_CANDY  * SCALE)) + (WIDTH_CANDY  * SCALE) / 2;
	M[i][j].coord.y = (j * (HEIGHT_CANDY * SCALE)) + (HEIGHT_CANDY * SCALE) / 2;

	M[i][j].sprite.setScale({ SCALE,SCALE });
}

void Game::initializareMatrice(void) 
{
	// Se initializeaza structurile de date candy cu numarul
	// bomboanei, textura(poza) corespunzatoare si sprite ul cu scale-ul initial
	srand(time(0));
	for (int i = 0; i < MDIM; i++)
		for (int j = 0; j < MDIM; j++)
			generateCandy({i,j});
		
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

void Game::setPositonOfSprites(void)
{
	for (int i = 0; i < MDIM; i++)
		for (int j = 0; j < MDIM; j++)
		{
			M[i][j].sprite.setPosition
			(M[i][j].coord.x - (WIDTH_CANDY *  M[i][j].sprite.getScale().x) / 2,
				M[i][j].coord.y - (HEIGHT_CANDY * M[i][j].sprite.getScale().y) / 2);
		}
}

void Game::draw()
{
	window.clear(sf::Color::Cyan);

	setPositonOfSprites();

	for (int i = 0; i < MDIM; i++)
		for (int j = 0; j < MDIM; j++)
			if(M[i][j].noCandy==-1){
				M[i][j].sprite.setColor(sf::Color::Cyan);
			}
			else
				window.draw(M[i][j].sprite);
}

void Game::draw(POINT FirstCandy, POINT SecondCandy)
{
	draw();
	if(!(FirstCandy==SecondCandy)){
		window.draw(M[SecondCandy.x][SecondCandy.y].sprite);
		window.draw(M[FirstCandy.x][FirstCandy.y].sprite);
	}
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


void Game::swapCandies(sf::Event e)
{
	if (mouse.pressed)
	{
		POINT currentPos = parseCoord({e.mouseMove.x,e.mouseMove.y}); 

		// current pos celula pe care se afla mouse-ul in de fiecare data cand se misca
		// mouse.coodM este pozitia celulei cand s-a dat click
		
		// std::cout << "current :" <<currentPos;
		// std::cout << "mouse :" <<mouse.coordM;

		if (!(currentPos == mouse.coordM) && 
		((abs(currentPos.x-mouse.coordM.x) + abs(currentPos.y-mouse.coordM.y)) == 1))
		// o verificare, pt bomboana cu care vr sa fac swap sa se afle intr-o casuta alaturata
		{

			POINT previousPos = mouse.coordM; // coord obtinute la eventimentul click(mouse down)		
			
			// Mai departe trebuie sa se realizeze animatia de swap indifierent daca se distrug //
			// sau nu niste bomboane, mai apoi vom face aceasta verificare //
		
			// Se incepe animatia si se returneaza true atunci cand se termina
			if (swapAnimation(previousPos, currentPos)) 
			{
				// Dupa ce se termina se realizeaza urmatoarele
				
				// 1. se face swap-ul in matrice dintre bomboane
				std::swap(M[currentPos.x][currentPos.y], M[previousPos.x][previousPos.y]);
				
				// 2. se numara in matrice daca se indeplineste conditia de spargere de bomboane
				// CountAndMarkCandies numara si marcheaza cate bomboane de acelasi tip 
				// cu bomboanele mutate sunt pe linii
				POINT counterCandiesAbove, counterCandiesBelow;
				counterCandiesAbove = CountAndMarkCandies(currentPos);
				counterCandiesBelow = CountAndMarkCandies(previousPos);	
				// std::cout << counterCandiesAbove;
				// std::cout << counterCandiesBelow;

				bool swapBack = true;

				if(counterCandiesAbove.x >= 3 || counterCandiesAbove.y>=3)
				{	
					M[currentPos.x][currentPos.y].marked = true;
					swapBack = false;
				}
				if (counterCandiesBelow.x >=3 || counterCandiesBelow.y >=3)
				{
					M[previousPos.x][previousPos.y].marked = true;
					swapBack = false;
				}

				if (swapBack){
					MarkedCandiesEvent(false); 
					// 3. daca nu se indeplineste se face swap back(la pozitile initiale)
					if(swapAnimation(currentPos,previousPos))
						std::swap(M[currentPos.x][currentPos.y], M[mouse.coordM.x][mouse.coordM.y]);
				}
				else {
					bool bTag;
					do {
						MarkedCandiesEvent(true);
						bTag=CautareDeFormatiiInMatrice();
					}while(bTag);
				}
			}
			mouse.pressed = false;
		}
	}
}

bool Game::CautareDeFormatiiInMatrice(){

	POINT values;
	bool amGasitFormatie=false;
	for(int j=0; j<MDIM; j++)
		for(int i=0; i<MDIM-2; i++)
		{
			int contor = 1;
			while(M[i][j].noCandy==M[i+1][j].noCandy){
				contor++;
				i++;
			}

			if(contor>=3){
				i = i-contor+1;
				M[i][j].marked=true;
				while(M[i][j].noCandy==M[i+1][j].noCandy){
					M[i+1][j].marked=true;
					i++;
				}
				amGasitFormatie= true;
			}
		}

	for(int i=0; i<MDIM; i++)
		for(int j=0; j<MDIM-2; j++)
		{
			int contor = 1;
			while(M[i][j].noCandy==M[i][j+1].noCandy){
				contor++;
				j++;
			}
			if(contor>=3){
				j = j- contor+1;
				M[i][j].marked=true;
				while(M[i][j].noCandy==M[i][j+1].noCandy){
					M[i][j+1].marked=true;
					j++;
				}
				amGasitFormatie= true;
			}
		}

	return amGasitFormatie;
}

bool Game::swapAnimation(POINT coordMSelectedCandy, POINT coordMToSwapCandy)
{
	sf::Clock clock = sf::Clock();
	sf::Time previousTime;							// secunda 0 
	sf::Time currentTime = clock.getElapsedTime();	// timpul trecut 

	bool isFinishedMoveAbove = false;
	bool isFinishedMoveBelow = false;

	int x = coordMSelectedCandy.x - coordMToSwapCandy.x;
	int y = coordMSelectedCandy.y - coordMToSwapCandy.y;

	POINT direction = { x, y };

	//direction
	// (0,-1) sus
	// (0, 1) jos
	// (-1,0) stanga
	// ( 1,0) dreapta

	while (1) {
		currentTime = clock.getElapsedTime();
		
		// la fiecare TIME_EVENT(10 milisecunde) se intampla ce e mai jos
		if ( currentTime.asMilliseconds() - previousTime.asMilliseconds() == TIME_EVENT_SWAP)
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
		
			draw(coordMSelectedCandy,coordMToSwapCandy);
			window.display();
			previousTime = currentTime;

			if (isFinishedMoveAbove && isFinishedMoveBelow){
				return true;
			}
		}
	}
}

bool Game::moveAboveAnimation(POINT coordM,POINT direction, float current,float goal)
{
	// variabilele de mai jos fiind statice 
	// se comporta ca niste variabile globale
	// astfel acestea sunt initializate cu param. fct 
	// o ~singura data~ si NU de fiecare data
	// cand se intra in while-ul functie de swapAnimation
	static float currentScale = current;	
	static float goalScale = goal;
	static float inc = ((goal-current)*2)/((SECUNDA*.5f)/TIME_EVENT_SWAP);

	// se mareste img
	currentScale += inc;
	// cand img a ajus la goalScale se revine la dim initialza schimband inc 
	if (currentScale >= goalScale)inc = -inc; 

	// se updateaza scale-ul sprite ului
	M[coordM.x][coordM.y].sprite.setScale(currentScale, currentScale);

	// se muta bomboana pe axele de coord
	M[coordM.x][coordM.y].coord.x -= direction.x * (WIDTH_CANDY * SCALE) / ((SECUNDA * .5f) / TIME_EVENT_SWAP);
	M[coordM.x][coordM.y].coord.y -= direction.y * (HEIGHT_CANDY * SCALE) / ((SECUNDA * .5f) / TIME_EVENT_SWAP);
	
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
	M[coordM.x][coordM.y].coord.x += direction.x * (WIDTH_CANDY  * SCALE) /((SECUNDA * .5f) / TIME_EVENT_SWAP);
	M[coordM.x][coordM.y].coord.y += direction.y * (HEIGHT_CANDY * SCALE) /((SECUNDA * .5f) / TIME_EVENT_SWAP);

	//std::cout << M[coordM.x][coordM.y].coord.y << '\n';

	// am adunat (width/height_candy*scale)/2 pentru ca tr determinat mijlocul bomboanei
	const int i = ((WIDTH_CANDY  * SCALE) * (coordM.x + direction.x)) + (WIDTH_CANDY  * SCALE) / 2;
	const int j = ((HEIGHT_CANDY * SCALE) * (coordM.y + direction.y)) + (HEIGHT_CANDY * SCALE) / 2;

	if ((direction.y == 1 && M[coordM.x][coordM.y].coord.y >= j) || (direction.y == -1 && M[coordM.x][coordM.y].coord.y <= j)) 
		return true;
	
	if ((direction.x == 1 && M[coordM.x][coordM.y].coord.x >= i) || (direction.x == -1 && M[coordM.x][coordM.y].coord.x <= i))
		return true;

	return false; 
}


POINT Game::CountAndMarkCandies(POINT coordM)
{
	// int counterX = 1 , counterY = 1;
	// counterX += ParcurgereRecusiva({ coordM.x, coordM.y }, onHorizontal);
	// counterY += ParcurgereRecusiva({ coordM.x, coordM.y }, onVertical);
	// return { counterX, counterY };
	int counterVertical = 1;
	int counterHorizontal = 1;
	bool down(1), up(1), right(1), left(1);
	// var down, up, right, left sunt pentru atunci cand 
	// chiar daca o bomboana are urm bomb de acelasi tip
	// dar la un moment dat NU, sa nu se mai contorizeze
	// adica sunt necesare pentru a valida consecutivitatea
	// bomboanelor de acelasi tip

	bool quit = false;

	int i = coordM.x, j = coordM.y;

	M[i][j].marked = true;
	// inc = 1 inseamna ca eu analizez celula de +1 -> sus,jos,stanga dreapta
	// inc = 2 inseamna ca eu analizez celula de +2 -> sus,jos,stanga dreapta

	for (int inc = 1; !quit ;inc++)
	{
		quit = true;

		if (i+inc<MDIM  && down && M[i+inc][j].noCandy == M[i][j].noCandy)
		{
			counterVertical++;
			quit = false;
			M[i+inc][j].marked = true;
		}else down = false;

		if (i-inc>=0 && up && M[i-inc][j].noCandy == M[i][j].noCandy)
		{
			counterVertical++;
			quit = false;
			M[i-inc][j].marked = true;
		}else up = false;

		if (j+inc<MDIM && right && M[i][j+inc].noCandy == M[i][j].noCandy)
		{
			counterHorizontal++;
			quit = false;
			M[i][j+inc].marked = true;
		}else right = false;

		if (j-inc>=0 && left && M[i][j-inc].noCandy == M[i][j].noCandy)
		{
			counterHorizontal++;
			quit = false;
			M[i][j-inc].marked = true;
		}else left = false;
	}
	if(counterHorizontal<3)
	{	
		//face false bomboanele adicente marcate si de acelasi tip dar care nu formeaza o formatie 
		M[i][j-1].marked = (M[i][j-1].marked && !M[i][j-1].noCandy==M[i][j].noCandy);
		M[i][j+1].marked = (M[i][j+1].marked && !M[i][j+1].noCandy==M[i][j].noCandy);
	}
	if(counterVertical<3)
	{
		M[i-1][j].marked =  (M[i-1][j].marked && !M[i-1][j].noCandy==M[i][j].noCandy);
		M[i+1][j].marked =  (M[i+1][j].marked && !M[i+1][j].noCandy==M[i][j].noCandy);
	}
	M[i][j].marked = false;

	return	{counterHorizontal,counterVertical};

}

void Game::MarkedCandiesEvent(bool destroy)
{
	std::vector<POINT> coordCandiesToDestroy;
	sf::Image image;
	image.createMaskFromColor(sf::Color(0, 255, 0, 0), 0);
	sf::Texture texture;
	texture.loadFromImage(image);

	for(int i=0; i<MDIM; i++)
		for(int j=0; j<MDIM; j++){
			if(M[i][j].marked && destroy)
				coordCandiesToDestroy.push_back({i,j});
			M[i][j].marked = false;
		}
	if(destroy){
		if(DestroyCandiesAnimation(coordCandiesToDestroy))
		{
			std::cout << "S-a termiant animatia de distrugere";
			for(auto coord : coordCandiesToDestroy){
				M[coord.x][coord.y].noCandy = -1;  // -1 inseamna bomboana distrusa
				M[coord.x][coord.y].sprite.setTexture(texture);
			}
			ReplaceDestroyedCandiesEvent();
		}
	}
}

bool Game::DestroyCandiesAnimation(std::vector<POINT> coordM)
{
	sf::Clock clock = sf::Clock();
	sf::Time previousTime;							// secunda 0 
	sf::Time currentTime = clock.getElapsedTime();	// timpul trecut 

	float currentScale = SCALE;
	float goalScale = 0.01;
	float dec = ((goalScale-currentScale)*2)/((SECUNDA*.5f)/TIME_EVENT_DESTROY);

	while (1) {
		currentTime = clock.getElapsedTime();

		if (currentTime.asMilliseconds() - previousTime.asMilliseconds() == TIME_EVENT_DESTROY)
		{
			currentScale += dec;

			for(auto coord : coordM){
				M[coord.x][coord.y].sprite.setScale({currentScale,currentScale});
			}
		
			if(currentScale<=goalScale){
				return true;
			}
			draw();
			window.display();
			previousTime = currentTime;
		}
	}
}

bool Game::ReplaceDestroyedCandiesEvent(void)
{	
	bool bVef, AmDatReplace, bTag;
	sf::Clock clock;

	do{
		bVef = false;
		AmDatReplace = true;
		std::vector<POINT> coordM;

		for(int i=0; i<MDIM; i++) //merg pe coloana
			for(int j=0; j<MDIM; j++) // pe linie
				if(M[i][j].noCandy==-1)
				{
					bVef = true;
					if(j==0) 
						generateCandy({i,j});
					else 
					{
						AmDatReplace = false;
						coordM.push_back({i,j});
						break;
					}
				}

		while(!AmDatReplace)
		{
			if (clock.getElapsedTime().asMilliseconds() == 1)
			{
				bTag = false;
				for(auto coord : coordM)
				{
					if(moveOnLineAnimation(coord, {0,-1}) && 
						moveOnLineAnimation({coord.x,coord.y-1}, {0,1}))
					{
							std::swap(M[coord.x][coord.y],M[coord.x][coord.y-1]);
							draw();
							bTag =true;
					}
					
				}
				if(bTag)
					window.display();
				
				clock.restart();
				
				break;
			}

		}
		AfisareMatrice();
		coordM.clear();
	}
	while(bVef);

	draw();
	window.display();

	return true;
}

Game::Game(unsigned int width, unsigned height):window(sf::VideoMode(width,height),"Game")
{
	incarcareImagini();
	initializareMatrice();
}

void Game::play(void)
{
	draw();
	window.display();
	handleEvents();
}
