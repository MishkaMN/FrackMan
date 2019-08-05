#include "Actor.h"
#include "StudentWorld.h"
// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp
int randInt(int min, int max)
{
	if (max < min)
		swap(max, min);
	static random_device rd;
	static mt19937 generator(rd());
	uniform_int_distribution<> distro(min, max);
	return distro(generator);
}

Protester::Protester(StudentWorld *sw, int imageID, int startX, int startY,  Direction dir , double size , unsigned int depth , int hp)
	:HPholder(sw, startX, startY, imageID, dir, size, depth, hp)
{
	numSquaresToMoveInCurrentDirection = randInt(8, 60);
	rest_ticks = max((unsigned int) 0, 3 - getWorld()->getLevel() / 4);
	tickPeriod = rest_ticks;
	m_state = 1;
	hasShouted = false;
	shouldMovePerp = false;
}

ValueHolder::ValueHolder(StudentWorld *sw, int startX, int startY, int imageID, Direction dir, double size, unsigned int depth)
	:Actor(sw, startX, startY, imageID, dir, size, depth)
{
	if (getWorld()->anyDirtAt(getX(), getY()))
		setVisible(false);
	else setVisible(true);
}

// Is the actor placable in 4x4 actor's operational range?
bool Actor::isPlacable(const int& x, const int& y) const
{
	if (x < 0 || x > 60 || y < 0 || y > 60)
		return false;
	else return true;
}

// Set dead. It will be deleted on the next tick.
void Actor::setDead()
{
	m_val = 0;
}

// Sonar's move on each tick, get picked up by FM
void SonarKit::doSomething()
{
	if (!isAlive())
		return;
	if (m_timer >= 0)
		setDead();
	if (getWorld()->isNearbyFrackMan(this, 3))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->giveFMSonar();
		getWorld()->increaseScore(75);
	}
}

// Gold's move on each tick. Get picked by FM or P and increase score or annoy P.
void GoldNugget::doSomething()
{
	setBool();
	if (!isAlive())
		return;
	Actor* nearbyP = getWorld()->anyNearbyP(this, 3);
	if (!isVisible() && getWorld()->isNearbyFrackMan(this, 4))
	{
		setVisible(true);
		return;
	}
	else if (getWorld()->isNearbyFrackMan(this, 3) && canPickedbyFM())
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->increaseScore(10);
		getWorld()->giveFMGold();
		return;
	}
	else if (nearbyP != nullptr && canPickedbyP())
	{
		setDead();
		getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD);
		getWorld()->increaseScore(25);
		nearbyP->addGold();
		return;
	}
	m_timer--;
	if (m_timer == 0)
		setDead();
}

// Barrel's move on each tick. Picked by FM and finish game level. 
void Barrel::doSomething()
{
	if (!isAlive())
		return;
	else if (!isVisible() && getWorld()->isNearbyFrackMan(this, 4))
	{
		setVisible(true);
		return;
	}
	else if (getWorld()->isNearbyFrackMan(this, 3))
	{
		setDead();
		getWorld()->decOil();
		getWorld()->playSound(SOUND_FOUND_OIL);
		getWorld()->increaseScore(1000);
	}
	return;

}

// Water Pool's move on each tick. Increase squirt of FM.
void WaterPool::doSomething()
{
	if (!isAlive())
		return;
	if (m_timer >= 0)
		setDead();
	if (getWorld()->isNearbyFrackMan(this, 3))
	{
		setDead();
		getWorld()->playSound(SOUND_GOT_GOODIE);
		getWorld()->giveFMWater();
		getWorld()->increaseScore(100);
	}

}

// Turn to told direction, or move to that dir if already facing it.
void MovingActor::turnOrMove(const Direction& dir, const int& x, const int& y)
{
	int desX, desY;
	switch (dir)
	{
	case left: desX = x - 1; desY = y; break;
	case right: desX = x + 1; desY = y; break;
	case up: desX = x; desY = y + 1; break;
	case down: desX = x; desY = y - 1; break;
	default: break;
	}
	if (getDirection() != dir)
		setDirection(dir);
	else if (getDirection() == dir && getWorld()->canActorMoveTo(this, desX, desY))
		moveTo(desX, desY);			//move
	else return;

}

// Boulder's move on each tick. Handle, falling, annoying FM or P etc.
void Boulder::doSomething()
{
	if (!isAlive())
		return;
	if (isStable() && !getWorld()->checkBoulderGround(this))	// stable but doesnt have ground
		m_state = 1; // enter waiting state
	if (m_state == 1 && m_timer <= 29)
		m_timer++;
	if (m_state == 1 && m_timer > 29)
	{
		getWorld()->playSound(SOUND_FALLING_ROCK);
		m_state = 2;
	}
	if (m_state == 2)
	{
		bool hitAnyone = false;
		// A. Check if it overlaps with any other actors, i.e. hit anyone?
		for (vector<Actor*>::const_iterator i = getWorld()->getActors().begin(); i != getWorld()->getActors().end(); i++)
		{
			if ((*i) != this && (*i)->hasHP() && getWorld()->doFramesOverlap(this->getX(), this->getY() - 1, (*i)->getX(), (*i)->getY()))
			{
				if ((*i)->isProtester())
					getWorld()->increaseScore(500);
				
				(*i)->setDead();
				hitAnyone = true;
			}
		}
		if (hitAnyone)
		{
			setDead();
			return;
		}

		// B. If didn't hit anyone, it should hit dirt or go out of screen
		// caActorMoveTo should return after executing only anyDirtAt part so that it is not redundant with A.

		if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
			turnOrMove(down, getX(), getY());
		else
			setDead();
	}
	return;
}

// Squirt's move on each tick. Either keep moving or specialmove - annoy P
void Squirt::doSomething()
{
	specialMove(getX(), getY());
	turnOrMove(getDirection(), getX(), getY());
}

// Squirt annoy protesters
void Squirt::specialMove(const int& x, const int& y)
{
	int desX, desY;
	switch (getDirection())
	{
	case left: desX = x - 4; desY = y; break;
	case right: desX = x + 4; desY = y; break;
	case up: desX = x; desY = y + 4; break;
	case down: desX = x; desY = y - 4; break;
	default: break;
	}
	if (getWorld()->annoyAllNearbyProtesters(desX,desY, 2) > 0)
	{
		setDead();
		getWorld()->increaseScore(100);
		return; 
	}
	else if (!getWorld()->canActorMoveTo(this, desX, desY))
		setDead();
	else if (getDistanceTraveled() >= 4)
		setDead();
	else continueTravel();
}

// Set dead, a different version, for Health Point holders.
void HPholder::setDead() { is_alive = false; Actor::setDead(); }

// Frackman's move on each tick. Moving, digging, reduce usable's resources
void FrackMan::doSomething()
{
	if (!isAlive())
		return;
	int ch;
	if (getWorld()->getKey(ch) == true)
	{
		// user hit a key this tick!
		switch (ch)
		{
		case KEY_PRESS_LEFT:
			turnOrMove(left, getX(), getY());
			break;
		case KEY_PRESS_RIGHT:
			turnOrMove(right, getX(), getY());
			break;
		case KEY_PRESS_UP:
			turnOrMove(up, getX(), getY());
			break;
		case KEY_PRESS_DOWN:
			turnOrMove(down, getX(), getY());
			break;
		case KEY_PRESS_ESCAPE:
			setDead();
			break;
		case KEY_PRESS_SPACE:
			if (getWater() > 0)
			{
				getWorld()->playSound(SOUND_PLAYER_SQUIRT);
				m_water--;
				getWorld()->addSquirt(getX(),getY(), getDirection());
			}
			break;
		case 'Z': case 'z':
			if (getSonar() > 0)
			{
				getWorld()->playSound(SOUND_SONAR);
				m_charge--;
				getWorld()->revealAllNearby(getX(), getY());
			}
			break;
		case KEY_PRESS_TAB:
			if (getGold() > 0)
			{
				m_gold--;
				getWorld()->addGold(getX(), getY(), 100);
			}
		default: break;
		}
	}
	// Dig the moved position.
	specialMove(getX(), getY());
}

// Set dead Frackman
void FrackMan::setDead() 
{
	getWorld()->playSound(SOUND_PLAYER_GIVE_UP);
	HPholder::setDead();
}

// Get annoyed frackman. Reduce health point.
bool FrackMan::getAnnoyed(unsigned int amt)
{
	bool completelyAnnoyed = HPholder::getAnnoyed(amt);
	if (getHP() <= 0)
	{
		setDead();
		return completelyAnnoyed;	// died
	}
	return false; // didn't die
}

// Digging: Frackman Special Move
void FrackMan::specialMove(const int& digX, const int& digY)
{
	if (getWorld()->anyDirtAt(digX,digY))
		getWorld()->getDug(digX, digY);
	return;
}

// Logic function: Returns the appropriate direction when the protester should move in perpendicular direction
GraphObject::Direction Protester::getDirPerp()
{
	int canMoveToBoth = 0;
	GraphObject::Direction dir = none;
	// whatever happens, if non_rest_tick reaches another set of 200, protester should move perpendicular
	if (non_rest_ticks % 200 == 0)
	{
		// this boolean should be changed only when protester actually moves perp
		shouldMovePerp = true;
	}
	if (shouldMovePerp)
	{
		if (getDirection() == left || getDirection() == right)
		{
			if (getWorld()->canActorMoveTo(this, getX(), getY() + 1))
			{
				canMoveToBoth++;
				dir = up;
			}
			if (getWorld()->canActorMoveTo(this, getX(), getY() - 1))
			{
				canMoveToBoth++;
				dir = down;
			}
			if (canMoveToBoth == 2)
			{
				int rnd = randInt(1, 2);
				dir = Direction(rnd);
			}
		}
		else if (getDirection() == up || getDirection() == down)
		{
			if (getWorld()->canActorMoveTo(this, getX() - 1, getY()))
			{
				canMoveToBoth++;
				dir = left;
			}
			if (getWorld()->canActorMoveTo(this, getX() + 1, getY()))
			{
				canMoveToBoth++;
				dir = right;
			}
			if (canMoveToBoth == 2)
			{
				int rnd = randInt(3, 4);
				dir = Direction(rnd);
			}
		}
	}
	return dir;
}

// Protester: Move randomly by random number of squares
void Protester::specialMove(const int& digX, const int& digY)
{
	// Pick new direction
	Direction new_rand_dir = Direction(randInt(1, 4));
	int old_x = getX(), old_y = getY();
	// If the protestor did not move, pick a new dir
	while (old_x == getX() && old_y == getY())
	{
		old_x = getX();
		old_y = getY();
		turnOrMove(new_rand_dir, getX(), getY());
		new_rand_dir = Direction(randInt(1, 4));
	}
	numSquaresToMoveInCurrentDirection = randInt(8, 60);
	return;
}

// Protester's move on each tick. It's special move, following FM, annoying him etc.
void Protester::doSomething()
{
	if (!isAlive())
	{
		return;
	}
	else if (tick() > 0)
	{
		tickTock();
		return;
	}
	else if (tick() == 0)
		setTicksToNextMove();

	// Leave Oil State
	Direction perp_dir = getDirPerp();
	if (getState() == 0)
	{
		if (getX() == 60 && getY() == 60)
			setDead();
		else 
		{
			// perform one step according to the queue based maze-searching algorithm
			leaveOilField();
		}
		return;
	}
	else if (getWorld()->isNearbyFrackMan(this, 4) && getWorld()->facingTowardFrackMan(this) && !hasRecentlyShouted())
	{
		// Time to shout
		getWorld()->playSound(SOUND_PROTESTER_YELL);
		getWorld()->annoyFrackMan();
		setShouted();
		return;
	}
	else if (getWorld()->lineOfSightToFrackMan(this) != none && getWorld()->facingTowardFrackMan(this) && !getWorld()->isNearbyFrackMan(this, 4))
	{
		// LOS and is facing toward Frackman, so move toward him
		turnOrMove(getDirection(), getX(), getY());
		numSquaresToMoveInCurrentDirection = 0;
		return;
	}
	else if (getWorld()->lineOfSightToFrackMan(this) != none)
	{
		// LOS but not facing nor near, so turn and move toward Frackman
		setDirection(getWorld()->lineOfSightToFrackMan(this));
		turnOrMove(getDirection(), getX(), getY());
		numSquaresToMoveInCurrentDirection = 0;
		return;
	}
	else if (getWorld()->lineOfSightToFrackMan(this) == none && perp_dir == Direction(none))
	{
		// No LOS to Frackman
		if (numSquaresToMoveInCurrentDirection <= 0)
		{
			specialMove(getX(), getY());
		}
	}
	else if (perp_dir != Direction(none))
	{
		// Do the perpendicular logic
		setDirection(perp_dir);
		numSquaresToMoveInCurrentDirection = randInt(8, 60);
		shouldMovePerp = false;
	}
	int old_x = getX(), old_y = getY();
	turnOrMove(getDirection(), getX(), getY());
	// If the protestor did not move, pick a new dir
	if (old_x == getX() && old_y == getY())
	{
		numSquaresToMoveInCurrentDirection = 0;
	}
}

// Protester: get annoyed, turn to leave state and go to 60,60
bool Protester::getAnnoyed(unsigned int amt)
{
	if (getState() == 0)
	{
		// Can't annoy already annoyed Protester
		return false;
	}
	getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
	bool completelyAnnoyed = HPholder::getAnnoyed(amt);
	if (getHP() <= 0)
	{
		// Not setting to dead completely
		setMustLeaveOilField(); // leaving state;
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		return completelyAnnoyed;	// died
	}
	return false; // didn't die
}

// Protester: get appropriate direction from maze structure and leave oil field
void Protester::leaveOilField()
{
	turnOrMove(getWorld()->getDir(this, getX(),getY()), getX(), getY());
	return;
}

// Everytime new protester dies, update the maze structure again to account for removed dirts since last time
void Protester::setMustLeaveOilField()
{
	// Updates the maze only when new protester dies, otherwise it is computationally heavy
	m_state = 0; 
	getWorld()->updateMaze();
}

// Handle protester's state change (leaving the field) or actually setting them dead
void Protester::setDead() {
	getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
	if (getState() == 0)
		HPholder::setDead();
	else
		setMustLeaveOilField();
}
