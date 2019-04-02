#include "StudentWorld.h"
#include <string>
using namespace std;

StudentWorld::StudentWorld(std::string assetDir)
	: GameWorld(assetDir), m_player(nullptr), L(0), G(0), B(0)
{
	incLives();					//warning
								// occupy room to breath
	for (int r = 0; r != 64; r++)
		for (int c = 0; c != 60; c++)
			m_dirts[r][c] = nullptr;
} /*filled*/
GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Initializes the world: player dirt, barrel, gold, boulder etc
int StudentWorld::init()
{
	// occupy FrackMan
	m_player = new FrackMan(this);
	actors.push_back(m_player);

	// occupy dirts
	for (int r = 0; r != 60; r++)
		for (int c = 0; c != 60; c++)
			m_dirts[r][c] = new Dirt(this, c, r);

	// occupy mineshaft
	for (int r = 4; r != 64; r++)
		for (int c = 30; c != 34; c++)
		{
			delete m_dirts[r][c];
			m_dirts[r][c] = nullptr;
		}

	// Boulder, Gold, and Barrel numbers
	B = min(getLevel() / 2 + 2, (unsigned int) 6);
	G = min(5 - getLevel() / 2, (unsigned int) 2);
	L = min(2 + getLevel(), (unsigned int) 20);

	// Populate
	populateCollectables(B, 'B');
	populateCollectables(G, 'G');
	populateCollectables(L, 'L');

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
			x = randInt(0, 60); y = randInt(0, 56);
		}
		else // if boulder
		{
			x = randInt(0, 60); y = randInt(20, 56);
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
		to_string(m_player->healthPoint()/10 * 100) + "% Wtr: " + to_string(m_player->getWater()) + " Gld: " + to_string(m_player->getGold()) +
		" Sonar: " + to_string(m_player->getSonar()) + " Oil Left: " + to_string(countOil()) + "      ");
	
	// If player dies:
	if (!m_player->isAlive())
	{
		deleteActor(m_player);
		decLives();
		return GWSTATUS_PLAYER_DIED;
	}
	// If the player found all barrels, finish level
	if (countOil() == 0)
		 return GWSTATUS_FINISHED_LEVEL;
	
	// Player does something
	m_player->doSomething();
	isSafe(m_player);

	// Rest of the Actors does something
	std::vector<Actor*>::iterator i = actors.begin() + 1;
	while (i != actors.end())
	{
		if (!(*i)->isAlive())
		{
			deleteActor(*i);
			break;
		}
		else
		{
			(*i)->doSomething();
			isSafe((*i));
		}
		i++;
	}
	return GWSTATUS_CONTINUE_GAME;
}

// When player loses a life, or completes the current level
// free all actors
void StudentWorld::cleanUp()
{
	// delete dirt
	for (int r = 0; r != 64; r++)
		for (int c = 0; c != 60; c++)
		{
			if (m_dirts[r][c] != nullptr)
			{
				delete m_dirts[r][c];
				m_dirts[r][c] = nullptr;
			}
		}/*filled*/

	std::vector<Actor*>::iterator i = actors.begin();
	while (i != actors.end())
	{
		deleteActor(*i); 
	}
}

// Check dirt fix
// TODO something with boundary and stuff
// How is it actually used?
bool StudentWorld::isDirtAt(const int& x, const int& y) const
{
	int xBoundary = x + 4, yBoundary = y + 4;
	for (int c = x; c != xBoundary; c++)
		for (int r = y; r != yBoundary; r++)
		{
			if (m_dirts[r][c]->isPlacable(c, r) && m_dirts != nullptr)
				return true;
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
void StudentWorld::deleteActor(Actor* ptr)
{
	std::vector<Actor*>::iterator i = find(actors.begin(), actors.end(), ptr);
	actors.erase(i);
	delete ptr;
	ptr = nullptr;
}

// Check if the Actor can move to (x,y)
bool StudentWorld::canActorMoveTo(Actor* ptr, const int& x, const int& y)
{
	bool answer = true;
	if (x < 0 || x > 56 || y < 0 || y > 60)
		return false;
	else if (isDirtAt(x, y) && ptr != m_player)
		return false;
	else if (isDirtAt(x, y) && ptr == m_player)
		return true;
	else
	{
		for (vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++)
			if ((*i) != ptr && doesFrameOverlap(ptr,(*i)) && (*i)->canActorPassThroughMe() == false )
				answer = false;
	}
	return answer;
}

// Deconstructor, deletes all dirt and actors
StudentWorld::~StudentWorld()
{
	// delete all dirt
	for (int r = 0; r != 60; r++)
		for (int c = 0; c != 60; c++)
		{
			delete m_dirts[r][c];
		}/*filled*/

	// delete all actors
	for (vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++)
		deleteActor(*i);
}

// Calculate euclidean distance between (x1,y1) & (x2,y2)
float StudentWorld::disBetween(int x1, int y1, int x2, int y2)
{
	return ((x1 - x2) ^ 2 + (y1 - y2) ^ 2) ^ (1 / 2);
}

// Check if 4x4 dimensions of actors overlap
bool StudentWorld::doesFrameOverlap(Actor* a1, Actor* a2) const
{
	int a1Boundary_x = a1->getX() + 4, a1Boundary_y = a1->getY() + 4;
	int a2Boundary_x = a2->getX() + 4, a2Boundary_y = a2->getY() + 4;
	if (a1->getX() >= a2->getX() && a1->getX() < a2Boundary_x && 
		a1->getY() >= a2->getY() && a1->getY() < a2Boundary_y)
		return true;
	else if 
		(a2->getX() >= a1->getX() && a2->getX() < a1Boundary_x && 
		a2->getY() >= a1->getY() && a2->getY() < a1Boundary_y)
		return true;
	else return false;
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

// Return true if there is any dirt beneath the boulder, false otherwise
bool StudentWorld::checkBoulderGround(Actor* a) const
{
	int xBoundary = a->getX() + 4;

	for (int k = a->getX(); k != xBoundary; k++)
	{
		if (m_dirts[a->getY()][k] != nullptr)
			return true;
	}
	return false;

}

// Return true if the given actor is within specified radius from Frackman
bool StudentWorld::isNearbyFrackMan(Actor* a, const int& radius)
{
	if (disBetween(a->getX(), a->getY(), m_player->getX(), m_player->getY()) <= radius)
		return true;
	else
		return false;
}
// True if Actor is safe from any boulder, false otherwise, which makes the actor dead.
// TODO: Optimize by creating separate vector from actors for boulder
bool StudentWorld::isSafe(Actor* a)
{
	// Only HP holders can be injured by boulder
	if (a->hasHP())
	{
		for (std::vector<Actor*>::iterator i = actors.begin(); i != actors.end(); i++)
			if ((*i)->isBoulder() && doesFrameOverlap(a, *i))
			{
				a->setDead();
				return false;
			}
	}
	return true;
}

// Return number of oils/barrels
int StudentWorld::countOil()
{
	return L;
}

// True if Actor sees Frackman
bool StudentWorld::facingTowardFrackMan(Actor* a)
{
	return a->getDirection() == lineOfSightToFrackMan(a);
}

// 
GraphObject::Direction StudentWorld::lineOfSightToFrackMan(Actor* a)
{
	const int ax = a->getX(), ay = a->getY();
	const int fx = m_player->getX(), fy = m_player->getY();

	const int max_x = max(ax, fx);
	const int min_x = min(ax, fx);
	const int max_y = max(ay, fy);
	const int min_y = min(ay, fy);

	// Return no direction if line of sight is blocked
	if (ax == fx)
	{
		for (int y = min_y; y != max_y; y++)
			if (isBoulderAt(ax, y) || isDirtAt(ax, y))
				return GraphObject::none;
	}
	else if (ay == fy)
	{
		for (int x = min_x; x != max_x; x++)
			if (isBoulderAt(x, ay) || isDirtAt(x, ay))
				return GraphObject::none;
	}

	// If not blocked, return direction to frackman from actor
	if (ay == min_y)
		return GraphObject::up;
	else if (ax== min_x)
		return GraphObject::right;
	else if (ay == max_y)
		return GraphObject::down;
	else if (ax == max_x)
		return GraphObject::left;
}
bool StudentWorld::isBoulderAt(const int& x, const int& y)
{
	int k = 0;
	std::vector<Actor*>::iterator i = actors.begin();
	while (k != actors.size())
	{
		if ((*i)->isBoulder() && (*i)->getX() == x && (*i)->getY() == y)
			return true;
		k++;
		i++;
	}
	return false;
}
int StudentWorld::annoyAllNearbyProtesters(Actor* annoyer, int points)
{
	int annoyed = 0;
	int k = 0;
	while (k != actors.size())
	{
		std::vector<Actor*>::iterator i = actors.begin();
		if ((*i)->isProtester() && doesFrameOverlap(annoyer, *i))
		{
			(*i)->getAnnoyed(points);
			annoyed++;
		}
		k++;
		i++;
	}
	return annoyed;
}


// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp
