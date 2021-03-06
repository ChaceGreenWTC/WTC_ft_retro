#include "Arena.hpp"
#include "ObjectManager.hpp"

Arena::Arena( void ) {;

}

Arena::Arena ( const Arena & srcObj ) {
	this->operator=( srcObj );
}

Arena & Arena::operator=( const Arena & srcObj ) {
	this->maxX = srcObj.maxX;
	return (*this);
}

Arena::~Arena( void ) {

}

int	Arena::initialise( void ){
	initscr();
	srand((int)time(0));
	getmaxyx(stdscr, this->maxY, this->maxX); // move inside loop to make dynamically sized window
	if(this->maxX < 30|| this->maxY < 15)
	{
		endwin();
		return(1);
	}
	keypad(stdscr, TRUE);
	noecho();
	nodelay(stdscr, TRUE);
	curs_set(FALSE);
	raw();
	this->died = 0;
	if(has_colors() == FALSE)
	{	
		endwin();
		printf("Your terminal does not support color\n");
		exit();
	}
	start_color();	
	init_pair(1, COLOR_GREEN, COLOR_BLACK);
	init_pair(2, COLOR_RED, COLOR_BLACK);
	init_pair(3, COLOR_YELLOW, COLOR_BLACK);
	init_pair(4, COLOR_MAGENTA, COLOR_BLACK);

	this->OM = new  ObjectManager(this->maxX, this->maxY); // ObjectManager instance
	return (0);
}

void	Arena::gameLoop( void ){

	int ch = 0;
	int quit = 0;
	int score = 0;
	clock_t cl = clock();

	int lc = 0; // character map for scrolling top and bottom
	int display = 0; // loop counter for scrolling top and bottom
	while (!quit){
		clear();
		cl = clock();
		printLines(&lc, score, cl);
		if (display == 3){ score += 2; lc += ((lc >= 2) ? -2 : 1 ); display = 0; }
		else { display++; }

		
		this->OM->collisionManager();
		this->OM->moveManager();
		this->OM->objectCleaner();
		this->OM->drawManager();
		this->OM->enemySpawner(); //meaningless values right now
		this->OM->gameTickCounter();

		refresh();
		ch = getch();
		if (this->OM->player->isActive() == 0) {
			this->died = 1;
			quit = 1;
		}
		switch (ch) {
			case KEY_LEFT:
				this->OM->player->updatePos(-1, 0);
				break;
			case KEY_RIGHT:
				this->OM->player->updatePos(1, 0);
				break;
			case KEY_UP:
				this->OM->player->updatePos(0, -1);
				break;
			case KEY_DOWN:
				this->OM->player->updatePos(0, 1);
				break;
			case 'q':
				quit = 1;
				break; //DEBUG
			case ' ':
				this->OM->playerFire();
				break;
		}
		usleep(20000);
	}
}

void	Arena::exit( void ){
	endwin();
	if (this->died == 1){
		std::cout << "\n\033[1;31m __     ______  _    _     _____ _____ ______ _____  \033[0m" << std::endl;
		std::cout << "\033[1;31m \\ \\   / / __ \\| |  | |   |  __ \\_   _|  ____|  __ \\ \033[0m" << std::endl;
		std::cout << "\033[1;31m  \\ \\_/ / |  | | |  | |   | |  | || | | |__  | |  | |\033[0m" << std::endl;
		std::cout << "\033[1;31m   \\   /| |  | | |  | |   | |  | || | |  __| | |  | |\033[0m" << std::endl;
		std::cout << "\033[1;31m    | | | |__| | |__| |   | |__| || |_| |____| |__| |\033[0m" << std::endl;
		std::cout << "\033[1;31m    |_|  \\____/ \\____/    |_____/_____|______|_____/ \033[0m\n\n" << std::endl ;
	}
}

void	Arena::printLines(int *lc, int score, clock_t cl)
{
	char char1;
	char char2;
	char char3;
	if (*lc == 0)
	{
		char1 = '\\';
		char2 = '_';
		char3 = '/';
	}
	else if (*lc == 1)
	{
		char1 = '_';
		char2 = '/';;
		char3 = '\\';
	}
	else if (*lc == 2)
	{
		char1 = '/';
		char2 = '\\';
		char3 = '_';
	}
	attron(COLOR_PAIR(4));
	for (int i = 1; i < this->maxX-3; i++)
	{
		mvaddch(1, i, char1);
		mvaddch(this->maxY-4, i++, char1);
		mvaddch(1, i, char2);
		mvaddch(this->maxY-4, i++, char2);
		mvaddch(1, i, char3);
		mvaddch(this->maxY-4, i, char3);
	}
attroff(COLOR_PAIR(4));
	mvprintw(this->maxY-2, 1, "SCORE: %i	TIME: %.2f", score, (float)cl/(60 * 60 * 2));
}