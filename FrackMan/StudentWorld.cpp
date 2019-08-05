#include "StudentWorld.h"
#include <string>


StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir), m_player(nullptr), L(0), G(0), B(0)
{
	incLives();					
	// occupy room to breath
	for (int r = 0; r != 64; r++)
		for (int c = 0; c != 64; c++)
			m_dirts[r][c] = nullptr;
}

// Deconstructor, deletes all dirt and actors
StudentWorld::~StudentWorld()
{
	// delete all dirt
	for (int r = 0; r != 64; r++)
		for (int c = 0; c != 64; c++)
		{
			if (m_dirts[r][c] != nullptr)
				delete m_dirts[r][c];
		}

	// delete all actors that may have not been deleted, e.g. when game ends prematurely
	if (actors.size() != 0 )
	{
		int i = 0;
		while (i != actors.size())
		{
			// deleteActor shifts all elements to front, so only delete 0th element
			deleteActor(actors[i]);
		}
	}
	cout << "Deleted the Student World Successfully. Thank you for playing!\n";
}

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Print the dirt world for debug purposes
void StudentWorld::printDirt()
{
	for (int y = 0; y < 64; y++)
	{
		for (int x = 0; x < 64; x++)
		{

			if (m_dirts[y][x] == nullptr)
			{
				cout << '.';
			}
			else if (m_dirts[y][x]->isDirt())
			{
				cout << 'o';
			}
			else
			{
				cout << '?';
			}
		}
		cout << '\n';
	}
}

// Initializes the world: player dirt, barrel, gold, boulder etc
int StudentWorld::init()
{
	// occupy dirts
	
	for (int r = 0; r != 60; r++)
		for (int c = 0; c != 64; c++)
			m_dirts[r][c] = new Dirt(this, c, r);
	
	// occupy mineshaft
	for (int r = 0; r != 64; r++)
		for (int c = 30; c != 34; c++)
		{
			delete m_dirts[r][c];
			m_dirts[r][c] = nullptr;
		}

	// occupy FrackMan
	m_player = new FrackMan(this);
	actors.push_back(m_player);


	// Boulder, Gold, and Barrel numbers
	B = min(getLevel() / 2 + 2, (unsigned int) 6);
	G = min(5 - getLevel() / 2, (unsigned int) 2);
	L = min(2 + getLevel(), (unsigned int) 20);

	// Populate
	populateCollectables(B, 'B');
	populateCollectables(G, 'G');
	populateCollectables(L, 'L');

	// Add a regular protester
	addRegularP();

	// Create maze for protester to use for exit
	for (int i = 0; i < 64; i++)
		for (int j = 0; j < 64; j++)
			m_maze[i][j] = 999;

	return GWSTATUS_CONTINUE_GAME;
}

// Populate barrel, bold, boulder with given numbers
void StudentWorld::populateCollectables(int num, char which)
{
	for (int k = 0; k != num; k++)
	{
		bool clrToInsert = true;
		int x, y;
		// if boulder or gold
		if (which == 'L' || which == 'G')
		{
			x = randInt(0, 56); y = randInt(0, 56);
		}
		else // if boulder
		{
			x = randInt(0, 56); y = randInt(20, 56);
		}
		// check if it is within radius 6
		for (std::vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++)
			if (disBetween((*i)->getX(), (*i)->getY(), x, y) <= 6)
			{
				k--;
				clrToInsert = false;
				break;
			}
		if (clrToInsert)
		{
			if (which == 'L')
				actors.push_back(new Barrel(this, x, y, getLevel()));
			else if (which == 'G')
				actors.push_back(new GoldNugget(this, x, y, -1));
			else
			{
				actors.push_back(new Boulder(this, x, y));
				delDirtat(x, y, 4);
			}
				
		}
	}
}

// One tick. Makes actors move. Called 10-20 times every second 
int StudentWorld::move()
{
	// Set game status text
	setGameStatText("Scr: " + to_string(getScore()) + " LVL: " + to_string(getLevel()) + " Lives: " + to_string(getLives()) + " Hlth: " + 
			to_string(m_player->getHP() * 10) + "% Wtr: " + to_string(m_player->getWater()) + " Gld: " + to_string(m_player->getGold()) +
		" Sonar: " + to_string(m_player->getSonar()) + " Oil Left: " + to_string(countOil()) + "      ");
	
	// If player dies:
	if (!m_player->isAlive())
	{
		playSound(SOUND_PLAYER_GIVE_UP);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	// If the player found all barrels, finish level
	if (countOil() == 0)
	{
		playSound(SOUND_FINISHED_LEVEL);
		return GWSTATUS_FINISHED_LEVEL;
	}
		 
	// Player does something
	m_player->doSomething();

	// Rest of the Actors does something
	int i = 1;
	
	while (i != actors.size())
	{
		if (!actors[i]->isAlive())
		{
			deleteActor(actors[i]);
			continue;
		}
		else
			actors[i]->doSomething();
		i++;
	}
	return GWSTATUS_CONTINUE_GAME;
}

//  Free all actors when player loses a life, or completes the current level
void StudentWorld::cleanUp()
{
	// delete dirt
	for (int r = 0; r != 64; r++)
		for (int c = 0; c != 64; c++)
		{
			if (m_dirts[r][c] != nullptr)
			{
				delete m_dirts[r][c];
				m_dirts[r][c] = nullptr;
			}
		}
	while (actors.size() != 0)
	{
		Actor* tmp = actors.back();
		actors.pop_back();
		delete tmp;
		tmp = nullptr;
	}
}

// Check if there is any dirt at a 4x4 square anchored at (x,y)
bool StudentWorld::anyDirtWallAt(const GraphObject::Direction& dir, const int& x, const int& y) const
{
	// The square anchored at (x,y) is unique because 
	// the anchor is defined by the lower left corner of the square.
	int xBoundary = x + 4, yBoundary = y + 4;
	int count = 0;
	switch (dir)
	{
		case left: case right:  
			for (int r = y; r != yBoundary; r++)
			{
				if (m_dirts[r][x]->isPlacable(x, r) && m_dirts[r][x] != nullptr)
					count++;
			}
			break;
		case up: case down: 
			for (int c = x; c != xBoundary; c++)
			{
				if (m_dirts[y][c]->isPlacable(c, y) && m_dirts[y][c] != nullptr)
					count++;
			}
			break;
		default: break;
	}
	return count == 4;
}

// Check if there is full dirt at a 4x4 square anchored at (x,y)
bool StudentWorld::anyDirtAt(const int& x, const int& y) const
{
	// The square anchored at (x,y) is unique because 
	// the anchor is defined by the lower left corner of the square. 
	int xBoundary = x + 4, yBoundary = y + 4;
	
	for (int r = y; r != yBoundary; r++)
	{
		for (int c = x; c != xBoundary; c++)
		{
			if (m_dirts[r][c] != nullptr)
			{
				return true;
			}
		}
	}
	return false;
}

// Deletes a square dirt at (x,y) with specified size 
void StudentWorld::delDirtat(const int& x, const int& y, const int& size)
{
	int xBoundary = x + size, yBoundary = y + size;
	for (int c = x; c != xBoundary; c++)
		for (int r = y; r != yBoundary; r++)
		{
			if (m_dirts[r][c] != nullptr)
			{
				delete m_dirts[r][c];
				m_dirts[r][c] = nullptr;
			}
		}
}

// Dig dirt at (x,y) according to Frackman's direction 
void StudentWorld::getDug(const int& x, const int& y)
{
	int xBoundary = x + 4, yBoundary = y + 4;
	int dir = m_player->getDirection();
	switch (dir)
	{
	case left:
		for (int k = y; k != yBoundary; k++)
		{
			if (m_dirts[k][x] != nullptr)
			{
				delete m_dirts[k][x];
				m_dirts[k][x] = nullptr;
			}
		}break;
	case right:
		for (int k = y; k != yBoundary; k++)
		{
			if (m_dirts[k][x + 3] != nullptr)
			{
				delete m_dirts[k][x + 3];
				m_dirts[k][x + 3] = nullptr;
			}
		}break;
	case up:
		for (int k = x; k != xBoundary; k++)
		{
			if (m_dirts[y + 3][k] != nullptr)
			{
				delete m_dirts[y + 3][k];
				m_dirts[y + 3][k] = nullptr;
			}
		}break;
	case down:
		for (int k = x; k != xBoundary; k++)
		{
			if (m_dirts[y][k] != nullptr)
			{
				delete m_dirts[y][k];
				m_dirts[y][k] = nullptr;
			}
		}break;
	default: break;

	}
	playSound(SOUND_DIG);
	return;
}

// Add Squirt actor at (x,y) to specified dir
void StudentWorld::addSquirt(int x, int y, GraphObject::Direction dir)
{
	int ch = dir;
	Squirt* m_squirt;
	switch (ch)
	{
		case up: m_squirt = new Squirt(this, x, y + 4, dir); break;
		case left: m_squirt = new Squirt(this, x - 4, y, dir); break;
		case down: m_squirt = new Squirt(this, x, y - 4, dir); break;
		case right: m_squirt = new Squirt(this, x + 4, y, dir); break;
		default: break;
	}
	actors.push_back(m_squirt);
	// Should annoy a Frackman when it is summoned
	if (annoyAllNearbyProtesters(m_squirt->getX(), m_squirt->getY(), 2) > 0)
	{
		m_squirt->setDead();
		return;
	}

	if (!canActorMoveTo(m_squirt, m_squirt->getX(), m_squirt->getY()))
		m_squirt->setDead();
}

// Check if there is protester at (x,y)
bool StudentWorld::isProtesterAt(const int& x, const int& y)
{
	for (std::vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) 
	{
		if ((*i)->isProtester() && (*i)->getX() == x && (*i)->getY() == y)
			return true;
	}
	return false;
}

// Delete the Actor with a given ptr
const void StudentWorld::deleteActor(Actor* ptr)
{
	Actor* tmp = ptr;
	std::vector<Actor*>::iterator k = find(actors.begin(), actors.end(), ptr);
	if (k == prev(actors.end()))
		actors.pop_back();
	else 
		ptr = *(actors.erase(k));
	delete tmp;
	tmp = nullptr;
}

// Check if the Actor can move to (x,y)
bool StudentWorld::canActorMoveTo(const Actor* ptr, const int& x, const int& y)
{
	bool answer = true;
	if (x < 0 || x > 60 || y < 0 || y > 60)
		return false;
	else if (ptr != m_player && anyDirtAt(x, y))
		return false;
	else if (ptr == m_player && anyDirtWallAt(ptr->getDirection(), x, y))
		return true;
	else
	{
		for (vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++)
			if ((*i) != ptr && doFramesOverlap(x, y, (*i)->getX(), (*i)->getY()) && (*i)->canActorPassThroughMe() == false)
			{
				answer = false;
			}
	}
	return answer;
}

// Check if Protester can move to (x,y) used for BFS
bool StudentWorld::canMoveTo(const int& x, const int& y)
{
	bool answer = true;
	if (x < 0 || x > 60 || y < 0 || y > 60)
		return false;
	else if (anyDirtAt(x,y))
		return false;
	else
	{
		for (vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++)
			if ((*i)->isObstacle() && doFramesOverlap(x, y, (*i)->getX(), (*i)->getY()))
			{
				answer = false;
			}
	}
	return answer;
}

// Calculate euclidean distance between (x1,y1) & (x2,y2)
float StudentWorld::disBetween(int x1, int y1, int x2, int y2)
{
	return pow((pow((float)(x1 - x2), 2) + pow((float)(y1 - y2), 2)),(float)1/2);
}

// Check if 4x4 dimensions of actors overlap
bool StudentWorld::doFramesOverlap(const Actor* a1, const Actor* a2) const
{
	int a1_x = a1->getX(), a1_y = a1->getY();
	int a2_x = a2->getX(), a2_y = a2->getY();
	
	// call the overloaded function that takes in coord of actors
	return doFramesOverlap(a1_x, a1_y, a2_x, a2_y);
}

// Check if 4x4 dimensions of actors anchored at (x1,y1) (x2,y2) overlap, overload func
bool StudentWorld::doFramesOverlap(const int a1_x, const int a1_y, const int a2_x, const int a2_y) const
{
	int a1_bx = a1_x + 3, a1_by = a1_y + 3;
	int a2_bx = a2_x + 3, a2_by = a2_y + 3;
	// With current logic, A1 overlapping w A2 is not same as A2 overlapping with A1, so
	// Does actor 1 overlap with actor 2
	bool left_to_right = (((a1_bx <= a2_bx) && (a1_bx  >= a2_x)) || ((a1_x >= a2_x) && (a1_x <= a2_bx ))) &&
		(((a1_y >= a2_y) && (a1_y <= a2_by )) || (((a1_by  >= a2_y)) && (a1_by <= a2_by)));
	if (left_to_right) return true;
	// Does actor 2 overlap with actor 1
	bool right_to_left = (((a2_bx <= a1_bx) && (a2_bx  >= a1_x)) || ((a2_x >= a1_x) && (a2_x <= a1_bx ))) &&
		(((a2_y >= a1_y) && (a2_y <= a1_by )) || (((a2_by-1 >= a1_y)) && (a2_by <= a1_by)));
	if (right_to_left) return true;
	return false;
}

// Use sonar, revealling all hidden items within 12 radius
void StudentWorld::revealAllNearby(const int& x, const int& y)
{
	int radius = 12;
	for (vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++)
	{
		if (disBetween((*i)->getX(), (*i)->getY(), m_player->getX(), m_player->getY()) <= 12)
			(*i)->setVisible(true);
	}
}

// Add gold to the world, timer is gold's lifespan, t=-1 before it is picked up by Frackman
void StudentWorld::addGold(const int& x, const int& y, int timer)
{
	GoldNugget* g = new GoldNugget(this, x, y, timer);
	actors.push_back(g);
}

// Add new regular protester
void StudentWorld::addRegularP()
{
	RegularP* rp = new RegularP(this);
	actors.push_back(rp);
}

// Return true if there is any dirt beneath the boulder, false otherwise
bool StudentWorld::checkBoulderGround(const Actor* a) const
{
	int xBoundary = a->getX() + 4;
	for (int k = a->getX(); k != xBoundary; k++)
	{
		if (m_dirts[a->getY() - 1][k] != nullptr)
			return true;
	}
	return false;
}

// Return true if the given actor is within specified radius from Frackman
bool StudentWorld::isNearbyFrackMan(const Actor* a, const int& radius)
{
	float distance = disBetween(a->getX(), a->getY(), m_player->getX(), m_player->getY());

	if (disBetween(a->getX(), a->getY(), m_player->getX(), m_player->getY()) <= (float)radius)
	{
		
		//if (a->hasHP())
			//printf("Distance is close with A:(%d,%d) and F:(%d,%d)\n", a->getX(), a->getY(), m_player->getX(), m_player->getY());
		return true;
	}
	else
	{
		//debug
		//if (a->hasHP())
			//printf("Distance is not close with A:(%d,%d) and F:(%d,%d)\n", a->getX(), a->getY(), m_player->getX(), m_player->getY());
		return false;
	}
		
}

// Return number of oils/barrels
int StudentWorld::countOil()
{
	return L;
}

// Decrease number of oils
void StudentWorld::decOil()
{
	L--;
}

// True if Actor sees Frackman
bool StudentWorld::facingTowardFrackMan(Actor* a)
{
	return a->getDirection() == lineOfSightToFrackMan(a);
	// Debug
}

// If any frame of frackman is in horizontal or vertical line of sight without any obstacles, return that direction. Otherwise none object.
GraphObject::Direction StudentWorld::lineOfSightToFrackMan(Actor* a)
{
	const int ax = a->getX(), ay = a->getY();
	const int fx = m_player->getX(), fy = m_player->getY();

	const int max_x = fx + 4;
	const int min_x = fx - 4;
	const int max_y = fy + 4;
	const int min_y = fy - 4;

	// Return no direction if line of sight is blocked
	if (min_x < ax && ax < max_x)
	{
		// Whether above or bottom, heck if any boulder or dirt is along LOS
		for (int y = ay; y > fy; y--)
			if (anyBoulderAt(ax, y) || anyDirtAt(ax, y))
				return GraphObject::none;
		for (int y = ay; y < fy; y++)
			if (anyBoulderAt(ax, y) || anyDirtAt(ax, y))
				return GraphObject::none;
		// If not blocked, return direction from actor to frackman 
		if (ay < fy)
			return GraphObject::up;
		else
			return GraphObject::down;
	}
	else if (min_y < ay && ay < max_y)
	{
		// Whether left or right, check any boulder or dirt along LOS
		for (int x = ax; x > fx; x--)
			if (anyBoulderAt(x, ay) || anyDirtAt(x, ay))
				return GraphObject::none;
		for (int x = ax; x < fx; x++)
			if (anyBoulderAt(x, ay) || anyDirtAt(x, ay))
				return GraphObject::none;
		// If not blocked, return direction from actor to frackman 
		if (ax > fx)
			return GraphObject::left;
		else
			return GraphObject::right;
	}
	else
		return GraphObject::none;
}

// Check if there is any boulder frame overlapped at 4x4 square anchored at (x,y)
bool StudentWorld::anyBoulderAt(const int& x, const int& y)
{
	int k = 1;
	while (k != actors.size())
	{
		if (actors[k]->isBoulder() && doFramesOverlap(actors[k]->getX(),  actors[k]->getY(), x, y))
			return true;
		k++;
	}
	return false;
}

// Annoy protesters by squirting, return number of protesters annoyed 
int StudentWorld::annoyAllNearbyProtesters(int x, int y, int points)
{
	int annoyed = 0;
	int k = 0;
	while (k != actors.size())
	{
		if (actors[k]->isProtester() && doFramesOverlap(x,y, actors[k]->getX(), actors[k]->getY()))
		{
			actors[k]->getAnnoyed(points);
			annoyed++;
		}
		k++;
	}
	return annoyed;
}

// Return nearby protesters within radius of that actor
Actor* StudentWorld::anyNearbyP(const Actor* a, const int radius)
{
	int k = 0;
	while (k != actors.size())
	{
		if (actors[k]->isProtester())
		{
			float distance = disBetween(a->getX(), a->getY(), actors[k]->getX(), actors[k]->getY());
			if (disBetween(a->getX(), a->getY(), actors[k]->getX(), actors[k]->getY()) <= (float)radius)
			{
				return actors[k];
			}
		}
		k++;
	}	
	return nullptr;
}

// Get direction for Protester at given location
GraphObject::Direction StudentWorld::getDir(Protester* actor, const int x, const int y) //todo
{
	// Figure out optimal order of directions
	int steps[4] = { m_maze[x][y + 1], m_maze[x][y - 1], m_maze[x - 1][y], m_maze[x + 1][y] };
	int min_step = 999;
	int stepOrder[4] = { 0 };
	// N^2 sort, there is a better way, but this will do for now (todo)
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (steps[j] < min_step)
			{
				stepOrder[i] = j;
				min_step = steps[j];
			}
		}
		// Mark this step/direction so that it is not accounted for in the next iteration
		steps[stepOrder[i]] = 999;

		// Check if the Protester can move in that direction
		int dirX, dirY = 0;
		GraphObject::Direction dir;
		switch (stepOrder[i])
		{
			// up
			case 0: dirX = x; dirY = y + 1; dir = GraphObject::Direction(up);
				break;
			// down
			case 1: dirX = x; dirY = y - 1; dir = GraphObject::Direction(down);
				break;
			// left
			case 2: dirX = x - 1; dirY = y; dir = GraphObject::Direction(left);
				break;
			// right
			case 3: dirX = x + 1; dirY = y; dir = GraphObject::Direction(right);
				break;
		}

		if (canActorMoveTo(actor, dirX, dirY))
			return dir;

		min_step = 999;
	}
	try
	{
		// Should never come here though
		throw ERROR;
	}
	catch (int e)
	{
		cout << "No possible direction is found for the protester" << e << '\n';
	}
}

// Update maze for protester's optimal paths using queue based DFS algorithm
void StudentWorld::updateMaze() //todo
{
	// create an empty queue
	queue<Node> nodeQueue;    

	// reinitialize the map

	for (int x = 0; x < 64; x++) 
	{
		for (int y = 0; y < 64; y++)
		{
			m_maze[x][y] = 999;
		}
	}
	
	nodeQueue.push(Node(60, 60));
	
	m_maze[60][60] = 0;

	while (!nodeQueue.empty())
	{
		Node current = nodeQueue.front();    // get the value of the top item before popping
		nodeQueue.pop();

		int x = current.getX();
		int y = current.getY();

		if (x <= 60 && y + 1 <= 60 && x >= 0 && y + 1 >= 0)   // can move NORTH
		{
			if (canMoveTo(x, y + 1) && (m_maze[x][y + 1] == 999 || m_maze[x][y] + 1 < m_maze[x][y + 1]))
			{
				m_maze[x][y + 1] = m_maze[x][y] + 1;
				nodeQueue.push(Node(x, y + 1));
			}
		}

		if (x <= 60 && y - 1 <= 60 && x >= 0 && y - 1 >= 0)   // can move SOUTH
		{
			if (canMoveTo(x, y - 1) && (m_maze[x][y - 1] == 999 || m_maze[x][y] + 1 < m_maze[x][y - 1]))
			{
				m_maze[x][y - 1] = m_maze[x][y] + 1;
				nodeQueue.push(Node(x, y - 1));
			}
		}

		if (x + 1 <= 60 && y <= 60 && x + 1 >= 0 && y >= 0)   // can move RIGHT
		{
			if (canMoveTo(x + 1, y) && (m_maze[x + 1][y] == 999 || m_maze[x][y] + 1 < m_maze[x + 1][y]))
			{
				m_maze[x + 1][y] = m_maze[x][y] + 1;
				nodeQueue.push(Node(x + 1, y));
			}
		}
		

		if (x - 1 <= 60 && y <= 60 && x - 1 >= 0 && y >= 0)   // can move LEFT
		{
			if (canMoveTo(x - 1, y) && (m_maze[x - 1][y] == 999 || m_maze[x][y] + 1 < m_maze[x - 1][y]))
			{
				m_maze[x - 1][y] = m_maze[x][y] + 1;
				nodeQueue.push(Node(x - 1, y));
			}
		}
	}

}