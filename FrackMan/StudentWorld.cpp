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

// Check dirt
bool StudentWorld::isDirtAt(const int& x, const int& y) const
{
	int xBoundary = x + 4, yBoundary = y + 4;
	for (int c = x; c != xBoundary; c++)
		for (int r = y; r != yBoundary; r++)
			if (m_dirts[r][c]->isPlacable(r, c) && m_dirts[r][c] != nullptr)
				return true;
	return false;
	/*filled*/
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
bool StudentWorld::isProtesterAt(const int& x, const int& y)
{
	int k = 0;
	std::vector<Actor*>::iterator i = actors.begin();
	while (k != actors.size())
	{
		if ((*i)->isProtester() && (*i)->getX() == x && (*i)->getY() == y)
			return true;
		k++;
		i++;
	}
	return false;
}
void StudentWorld::deleteActor(Actor* ptr)
{
	std::vector<Actor*>::iterator i = find(actors.begin(), actors.end(), ptr);
	std::vector<Actor*>::iterator p = i;
	p++;
	if (p == actors.end())
		actors.pop_back();
	else
		i = actors.erase(i);
	delete ptr;
	ptr = nullptr;
}
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
			if ((*i) != ptr && doesRadiusOverlap(ptr,(*i)) && (*i)->canActorPassThroughMe() == false )
				answer = false;
	}
	return answer;
}
StudentWorld::~StudentWorld()
{
	for (int r = 0; r != 60; r++)
		for (int c = 0; c != 60; c++)
		{
			delete m_dirts[r][c];
		}/*filled*/
	int k = 0;

	while (k != actors.size())
	{
		std::vector<Actor*>::iterator i = actors.begin();
		deleteActor(*i);
	}
}
float StudentWorld::disBetween(int x1, int y1, int x2, int y2)
{
	return ((x1 - x2) ^ 2 + (y1 - y2) ^ 2) ^ (1 / 2);
}
bool StudentWorld::doesRadiusOverlap(Actor* a1, Actor* a2) const
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
void StudentWorld::revealAllNearby(const int& x, const int& y)
{
	int radius = 12;
	vector<Actor*>::iterator i = actors.begin();
	for (int k = 0; k != actors.size(); k++)
	{
		if (isNearbyFrackMan((*i), radius))
		(*i)->setVisible(true);
	}
}
bool StudentWorld::isNearbyFrackMan(Actor* a, const int& radius) const
{
	int aBoundary_x = a->getX() + radius, aBoundary_y = a->getY() + radius;
	if (m_player->getX() >= a->getX() - radius && m_player->getX() <= aBoundary_x && 
		m_player->getY() >= a->getY() - radius && m_player->getY() <= aBoundary_y)
		return true;
	else return false;
}
void StudentWorld::addGold(const int& x, const int& y, int timer)
{
	GoldNugget* g = new GoldNugget(this, x, y, timer);
	actors.push_back(g);
}

// Check if the Boulder has a ground beneath it or not
bool StudentWorld::checkBoulderGround(Actor* a) const
{
	int xBoundary = a->getX() + 4;
	for (int k = a->getX(); k != xBoundary; k++)
	{
		if (m_dirts[a->getY()][k] != nullptr)
			return false;
	}
	return true;
}
bool StudentWorld::isSafe(Actor* a)
{
	if (a->hasHP())
	{
		int k = 0;
		while (k != actors.size())
		{
			std::vector<Actor*>::iterator i = actors.begin();
			if ((*i)->isBoulder() && doesRadiusOverlap(a, *i))
			{
				a->setDead();
				return false;
			}
			k++;
		}
	}
	return true;
}
int StudentWorld::countOil()
{
	int k = 0;
	while (k != actors.size())
	{
		std::vector<Actor*>::iterator i = actors.begin();
		if ((*i)->isOil())
			L++;
		k++;
	}
	return L;
}
bool StudentWorld::facingTowardFrackMan(Actor* a)
{
	return a->getDirection() == lineOfSightToFrackMan(a);
}
GraphObject::Direction StudentWorld::lineOfSightToFrackMan(Actor* a)
{
	Actor* f = actors[0];
	const int max_y = a->getY() > f->getY() ? a->getY() : f->getY();
	const int min_y = a->getY() <= f->getY() ? a->getY() : f->getY();
	const int max_x = a->getX() > f->getX() ? a->getX() : f->getX();
	const int min_x = a->getX() <= f->getX() ? a->getX() : f->getX();
	if (a->getX() == f->getX())
	{
		for (int i = min_y; i != max_y; i++)
			if (isBoulderAt(a->getX(), i) || isDirtAt(a->getX(), i))
				return GraphObject::none;
	}
	else if (a->getY() == f->getY())
	{
		for (int i = min_x; i != max_x; i++)
			if (isBoulderAt(i, a->getY()) || isDirtAt(i, a->getY()))
				return GraphObject::none;
	}
	if (a->getY() == min_y)
		return GraphObject::up;
	else if (a->getX() == min_x)
		return GraphObject::right;
	else if (a->getY() == max_y)
		return GraphObject::down;
	else if (a->getX() == max_x)
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
		if ((*i)->isProtester() && doesRadiusOverlap(annoyer, *i))
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
