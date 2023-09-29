#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <list>
#include <cmath>
/////////////////////////////////////////////////////////////
//
// Programmer: Joey Brady
// Date: 11/30/22
// Name: Hw7 Asteroids
// Description: Program allows you to play a game of Asteroids
//
/////////////////////////////////////////////////////////////

//EXTRA THINGS
//Did not add too much in terms of extra work, but added explosion sounds for everything
//added a firing noise along with thrust noise, and tried my very best on indetation with curly braces


using namespace sf;

const int W = 1200;           //Width of the game screen
const int H = 800;            //Height of the game screen

float DEGTORAD = 0.017453f;  //Constant to convert degrees to radians

class Animation     //The Animation class that allows a change in texture
{
public:
	float Frame, speed;           //Initiazation of the Frame & speed float
	Sprite sprite;                //Initialization of the sprite from the Sprite class
    std::vector<IntRect> frames;  //Initialization of vector<IntRect> frames to store the frames

	Animation() {}                 //Constructor with no parameters

    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed)    //Constructor with parameters
	{
	    Frame = 0;      //Resets float Frame to 0
        speed = Speed;  //Updates the float speed variable

		for (int i=0;i<count;i++)   //Loop that repeats until its limit of count from the parameters
         frames.push_back( IntRect(x+i*w, y, w, h)  ); //Adds a new element to the frames vector

		sprite.setTexture(t);  //Sets the texture of the object
		sprite.setOrigin(w/2,h/2); //Sets the starting point to 1/2 of the parameters
        sprite.setTextureRect(frames[0]); //Sets the size of region of the texture to display
	}

	void update() //Void Function that updates the animation to make it animated
	{
    	Frame += speed;  //Progresses the Frame in respect to speed
		int n = frames.size();  //Initializes a variable to store the size of the frames vector
		if (Frame >= n) {Frame -= n;} //If Frame is greater than the size of the vector then it returns Frame to a previous state based on the size of the vector
		if (n>0) {sprite.setTextureRect( frames[int(Frame)] );} //Changes the size of region of the texture to display based on current Frame
	}

	bool isEnd() //End Function
	{
	  return Frame+speed>=frames.size(); //Outputs true if the speed in addiction to Frame is larger than the frames vector
	}
};

class Entity
{
public:
    float x,y,dx,dy,R,angle; //Initialization of the floats for the class
    bool life;               //Initialization of the boolean to determine if the object exists at the time
    std::string name;        //Initialization of the string for name
    Animation anim;          //Initialization of an Animation class called anim with no parameters

    Entity() { life=1;}      //Constructor setting life to true

/////////////////////////////////////////////////////////////
//
// Description: A large setter class for itself and all of it's derived classes
// Inputs: Animation class pass by reference, int x and int y for position, float Angle for rotation and radius for size
// No Outputs
//
/////////////////////////////////////////////////////////////
void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)  //Settings function to change the info of an Entity Class Object
{
  anim = a;       //Sets anim to the parameter &a
  x=X; y=Y;       //Sets float x,y to parameter int X,Y
  angle = Angle;  //Sets angle to parameter Angle
  R = radius;     //Sets R to parameter radius
}

virtual void update(){};     //Derived version of update from base class Animation

/////////////////////////////////////////////////////////////
//
// Description: Draws the sprites given to it onto the screen
// Inputs: the Render Window in which the function draws on
// No Outputs
//
/////////////////////////////////////////////////////////////
void draw(RenderWindow &app) //Draw function
{
  anim.sprite.setPosition(x,y);            //Sets position of the sprite
  anim.sprite.setRotation(angle+90); //Sets rotation of the sprite
  app.draw(anim.sprite);           //Draws the sprite onto the scree

  CircleShape circle(R);            //Initialization of a circle shape to the same size as sprite
  circle.setFillColor(Color(255,0,0,170)); //Sets color to a translucent red for testing purposes
  circle.setPosition(x,y);                //Sets position of circle over sprite
  circle.setOrigin(R,R);            //Sets origin onto the radius
  //app.draw(circle);              //Draws the hitboxes of the objects for testing purposes
}

virtual ~Entity(){};         //Destructor for the Entity class
};

//My Created Class
class ufo: public Entity{
public:
    static int ufoCount;  //Static Int counter for the ufo to ensure there is only one
    ufo()
    {
        if(rand()%2==1){ //Randomizes if the ufo flies right or left
            dx=3;  //Flies to the right
        }
        else{
            dx=-3;  //Flies to the left
        }
        name="UFO"; //Names the object as a Ufo
        ++ufoCount; //Increase to static into to account for the creation of a UFO
    }
    ~ufo()   //Destructor for the UFO class
    {
        --ufoCount; //Subtracts from the static count to account for the removal of an UFO
    }
    void update()  //Update function
    {
        x+=dx;     //Increases x based on dx
        if (x>W) x=0;  if (x<0) x=W; //If the x-coordinate reaches the edge of the screen, restart at the other side of the screen
    }
};
int ufo::ufoCount = 0; //Sets UFO count to 0

class asteroid: public Entity    //Initialization of asteroid, an Inherited Class from the Entity Class
{
public:
    static int meteorCount; //Static int counter to count the asteroids

  asteroid()              //Constructor for the Asteroid Class
  {
    dx=rand()%8-4;       //initializes dx value
    dy=rand()%8-4;       //initializes dy value
    name="asteroid";     //initializes name as asteroid
    ++ meteorCount;     //Increases the static count to account for the addition of an asteroid
  }
    ~asteroid(){        //Destructor for the asteroid class
      --meteorCount;    //Subtracts from the static count to account for the removal of an asteroid
      }

void  update()          //update function
  {
   x+=dx;              //Increases x based on dx
   y+=dy;              //Increases y based on dy

   if (x>W) x=0;  if (x<0) x=W;  //If the x-coordinate reaches the edge of the screen, restart at the other side of the screen
   if (y>H) y=0;  if (y<0) y=H;  //If the y-coordinate reaches the edge of the screen, restart at the other side of the screen
  }
};

int asteroid::meteorCount=0; //Sets the meteorCount to 0

class bullet: public Entity //Initialization of bullet, an Inherited Class from the Entity Class
{
public:
  bullet()            //bullet constructor with no parameters
  {
    name="bullet"; //Initialization of the name as bullet
  }

void  update()   //Update function
  {
   dx=cos(angle*DEGTORAD)*6;  //Sets dx based on the angle
   dy=sin(angle*DEGTORAD)*6;  //Sets dy based on the angle
  // angle+=rand()%6-3;
   x+=dx;                       //increases x based on dx
   y+=dy;                       //increases y based on dy

   if (x>W || x<0 || y>H || y<0){life=0;} //Once they hit the edge of the screen, bullets life is set to false and disappear from screen
  }

};


class player: public Entity //Initialization of player, an Inherited Class from the Entity Class
{
public:
   bool thrust;           //Initialization of the boolean thrust

   player()    //player constructor with no parameters
   {
     name="player";  //Initialization of the name player
   }

   void update()  //Update function
   {
     if (thrust)   //If there is thrust
      { dx+=cos(angle*DEGTORAD)*0.2;  //Increase dx based on the angle
        dy+=sin(angle*DEGTORAD)*0.2; } //Increase dy based on the ang;e
     else
      { dx*=0.99;   //If not multiply dx by 0.99 for a glide
        dy*=0.99; } //If not multiply dy by 0.99 for a glide

    int maxSpeed=15;                   //Initialization of the max speed
    float speed = sqrt(dx*dx+dy*dy); //Calculates speed based on pythagoreas theorem
    if (speed>maxSpeed)         //If speed is over the maxSpeed
     { dx *= maxSpeed/speed;    //Multiply dx by maxSpeed/speed
       dy *= maxSpeed/speed;    //Multiply dy by maxSpeed/speed
     }

    x+=dx;                  //Increase x by dx
    y+=dy;                  //Increase y by dy

    if (x>W) {x=0;} if (x<0) {x=W;} //If the x-coordinate reaches the edge of the screen, restart at the other side of the screen
    if (y>H) {y=0;} if (y<0) {y=H;} //If the y-coordinate reaches the edge of the screen, restart at the other side of the screen
   }
};

/////////////////////////////////////////////////////////////
//
// Description: Function That Checks if two objects collided
// Inputs: Entity *a and Entity *b, two pointers from the entity class
//         that can be any of the Entity Class's Derived Classes
// Outputs: Returns True or False
//
/////////////////////////////////////////////////////////////
bool isCollide(Entity *a,Entity *b) //Checks to see if two entity class collide
{
  return (b->x - a->x)*(b->x - a->x)+    //If they do this bool returns true, if not returns false
         (b->y - a->y)*(b->y - a->y)<
         (a->R + b->R)*(a->R + b->R);
}

int main()
{
    srand(time(0));   //Initialization of srand to give a truly random rand

    RenderWindow app(VideoMode(W, H), "Asteroids!"); //Initialization of the game window
    app.setFramerateLimit(60);    //Sets the frame limit of the game

    Texture t1,t2,t3,t4,t5,t6,t7,saucer;     //Initialization of the textures
    t1.loadFromFile("images/spaceship.png");      //Loads all the textures in
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");

    saucer.loadFromFile("images/ufo.png");  //Loads in the added ufo texture

    SoundBuffer ufoAlarm; //Initialization of the sound buffer
    if (!ufoAlarm.loadFromFile("audio/alienAlarm.ogg")) { return EXIT_FAILURE; } //Loads the sound into the buffer
    Sound ufoSound; //Initialization of the sound
    ufoSound.setBuffer(ufoAlarm); //Sets the sound to the buffer
    ufoSound.setLoop(true); //Sets the ufoSound to loop as long as the UFO exists

    SoundBuffer shootBuffer; //Initialization of the sound buffer
    if (!shootBuffer.loadFromFile("audio/obxjohn__pew-pew.ogg")) { return EXIT_FAILURE; } //Loads the sound into the buffer
    Sound shootSound ; //Initialization of the sound
    shootSound.setBuffer(shootBuffer); //Sets the sound to the buffer

    SoundBuffer exp1Buffer; //Initialization of the sound buffer
    if (!exp1Buffer.loadFromFile("audio/448226__inspectorj__explosion-8-bit-01.ogg")) { return EXIT_FAILURE; } //Loads the sound into the buffer
    Sound exp1Sound ; //Initialization of the sound
    exp1Sound.setBuffer(exp1Buffer); //Sets the sound to the buffer

    SoundBuffer exp2Buffer; //Initialization of the sound buffer
    if (!exp2Buffer.loadFromFile("audio/derplayer__explosion-07.ogg")) { return EXIT_FAILURE; } //Loads the sound into the buffer
    Sound exp2Sound ; //Initialization of the sound
    exp2Sound.setBuffer(exp2Buffer); //Sets the sound to the buffer

    SoundBuffer exp3Buffer; //Initialization of the sound buffer
    if (!exp3Buffer.loadFromFile("audio/derplayer__explosion-near.ogg")) { return EXIT_FAILURE; } //Loads the sound into the buffer
    Sound exp3Sound ; //Initialization of the sound
    exp3Sound.setBuffer(exp3Buffer); //Sets the sound to the buffer

    SoundBuffer thrustBuffer; //Initialization of the sound buffer
    if (!thrustBuffer.loadFromFile("audio/mango777__loopingthrust.ogg")) { return EXIT_FAILURE; } //Loads the sound into the buffer
    Sound thrustSound ; //Initialization of the sound
    thrustSound.setBuffer(thrustBuffer); //Sets the sound to the buffer

    t1.setSmooth(true); //makes the spaceship texture smooth
    t2.setSmooth(true); //makes the background texture smooth
    saucer.setSmooth(true); //makes the added ufo smooth

    Sprite background(t2); //initialization of the background sprite use the t2/background texture

    Animation sExplosion(t3, 0,0,256,256, 48, 0.5); //initialization of all the animations of the textures
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);

    Animation sUfo(saucer,0,0,32,52,1,0); //Initialization of the added ufo animations

    std::list<Entity*> entities;               //Initialization of a list for the entities


    player *p = new player();  //Initialization of the player object
    p->settings(sPlayer,200,200,0,20); //Using the setting function to set the palyer's values
    entities.push_back(p);  //Adds the player to the end of the list

    /////main loop/////
    while (app.isOpen()) //Runs for the entirety that the application is open
    {
        Event event;  //Initialization of the event class
        while (app.pollEvent(event)) //Runs as long as the event is open
        {
            if (event.type == Event::Closed) { //Checks if the event is closed
                app.close();  //If true closes the app along with it
            }

            if (event.type == Event::KeyPressed) { //Checks to see if a key was pressed
                if (event.key.code == Keyboard::Space) { //If spacebar is pressed
                    bullet *b = new bullet(); //Produces a new bullet class object
                    shootSound.play(); //Gives the bullet a sound when spacebar is released
                    b->settings(sBullet, p->x, p->y, p->angle, 10); //using the setting function to set the bullet's value based on player position
                    entities.push_back(b); //Adds the bullet to the end of the entities list
                }
            }
        }

        if(asteroid::meteorCount==0) { //Checks to see if the original 15 asteroids have been destoryed
            for (int i = 0; i < 15; i++) //For loop that repeats 15 times
            {
                asteroid *a = new asteroid(); //Initialization of a new asteroid object
                a->settings(sRock, rand() % W, rand() % H, rand() % 360, 25); //Using the setting function to set its values
                entities.push_back(a); //Adds the new asteroid to the entities list
            }
        }

    if (Keyboard::isKeyPressed(Keyboard::Right)){ p->angle+=3;} //Checks to see if the right arrow was pressed to change angle of spaceship
    if (Keyboard::isKeyPressed(Keyboard::Left)) { p->angle-=3;} //Checks to see if the left arrow was pressed to change angle of spaceship
    if (Keyboard::isKeyPressed(Keyboard::Up)) {
        p->thrust = true; //Checks to see if the up arrow was pressed to make thrust true for the player
        thrustSound.play();
    }
    else {
        p->thrust=false; //If up arrow is not pressed the thrust is false for the player
        thrustSound.stop();
    }

    for(auto a:entities){ //For all entities in the entities list
     for(auto b:entities) { //For another look through of the entities in the entities list
         if (a->name == "asteroid" && b->name == "bullet") { //If the a entity is an asteroid and the b entity is a bullet then..
             if (isCollide(a, b)) {  //Using the isCollide function checks if the two objects hit

                 a->life = false; //If true sets life to false for both objects and they disappear from game screen
                 b->life = false;

                 Entity *e = new Entity(); //Creates a new Entity
                 e->settings(sExplosion, a->x, a->y); //Sets the explosion ontop of the asteroid
                 exp3Sound.play(); //Plays Asteroid Explosion Sound
                 e->name = "explosion"; //Sets the name of the entity
                 entities.push_back(e); //Adds the explosion to the end of the entities list


                 for (int i = 0; i < 2; i++) { //Loop that runs twice
                     if (a->R == 15) { continue; } //If the radius of the destroyed asteroid is 15 then the code continues to the next iteration
                     Entity *e = new asteroid();  //Creates a new entity
                     e->settings(sRock_small, a->x, a->y, rand() % 360, 15); //Sets the new asteroid to a smaller asteroid
                     entities.push_back(e); //Adds the entity to the end of the list
                 }
             }
         }

         if (a->name == "UFO" && b->name == "bullet") { //If the a entity is an UFO and the b entity is a bullet then..
             if (isCollide(a, b)) {  //Using the isCollide function checks if the two objects hit

                 a->life = false; //If true sets life to false for both objects and they disappear from game screen
                 b->life = false;
                 ufoSound.stop();

                 Entity *e = new Entity(); //Creates a new Entity
                 e->settings(sExplosion_ship, a->x, a->y); //Sets the explosion ontop of the UFO
                 exp2Sound.play();//Plays UFo Explosion Sound
                 e->name = "explosion"; //Sets the name of the entity
                 entities.push_back(e); //Adds the explosion to the end of the entities list
             }
         }

         if (a->name == "player" && b->name == "UFO") { //If the a entity is the player and the b entity is an UFO then..
             if (isCollide(a, b)) {
                 b->life = false; //The UF0's life is set to 0 and therefore removed from the game
                 ufoSound.stop();

                 Entity *e = new Entity(); //Creates a new Entity
                 e->settings(sExplosion_ship, a->x, a->y); //Sets the ship explosion ontop of the ship
                 exp1Sound.play();//Plays SpaceShip Explosion Sound
                 e->name = "explosion"; //Sets the name of the entity
                 entities.push_back(e); //Adds the entity to the end of the list

                 Entity *f = new Entity(); //Creates a new Entity
                 f->settings(sExplosion_ship, b->x, b->y); //Sets the ship explosion ontop of the UFO
                 f->name = "explosion"; //Sets the name of the entity
                 entities.push_back(f); //Adds the entity to the end of the list
                 ufoSound.stop();

                 p->settings(sPlayer, W / 2, H / 2, 0, 20); //Resets the settings of the ship/player
                 p->dx = 0; //Sets the player's dx to 0
                 p->dy = 0; //Sets the player's dy to 0
             }
         }

         if (a->name == "player" && b->name == "asteroid") { //If the a entity is the player and the b entity is a bullet then..
             if (isCollide(a, b)) {
                 b->life = false; //The asteroid's life is set to 0 and therefore removed from the game

                 Entity *e = new Entity(); //Creates a new Entity
                 e->settings(sExplosion_ship, a->x, a->y); //Sets the ship explosion ontop of the ship
                 exp1Sound.play();//Plays SpaceShip Explosion Sound
                 e->name = "explosion"; //Sets the name of the entity
                 entities.push_back(e); //Adds the entity to the end of the list

                 p->settings(sPlayer, W / 2, H / 2, 0, 20); //Resets the settings of the ship/player
                 p->dx = 0; //Sets the player's dx to 0
                 p->dy = 0; //Sets the player's dy to 0
             }
         }
     }
    }

    if (p->thrust) { //Checks to see if there is thrust
        p->anim = sPlayer_go; //If thrust is active changes the player animation to the moving player animation
    }
    else {
        p->anim = sPlayer; //If no thrust then Splayer has it's normal animation
    }

    for(auto e:entities) {      //For every variable in the list entities
        if (e->name == "explosion") {  //If they have the name explosion
            if (e->anim.isEnd()) { //If their animation is finished
                e->life = 0;  //Then set their life to zero removing them from the game
            }
        }
    }

    /*if (rand()%150==0){     //If the random generator produces a value less than 150
       asteroid *a = new asteroid(); //A new asteroid class object is created
       a->settings(sRock, 0,rand()%H, rand()%360, 25); //Sets the settings of the new asteroid
       entities.push_back(a); //Adds the asteroid to the end of the entities list
     }
    */
    if(ufo::ufoCount==0) {         //Checks to see that there is no other UFO
        if (rand() % 800 == 0) {   //Random generator to determine if a UFO should spawn
            ufo *u = new ufo();   //Initializes New UFO
            ufoSound.play();       //Plays UFO sound
            u->settings(sUfo, 0, rand() % H, 270, 20); //Sets the settings for the UFO
            entities.push_back(u);  //Puts the UFO at the end of the entities list
        }
    }


    for(auto i=entities.begin();i!=entities.end();){ // i traverses the entirety of the entities list
      Entity *e = *i; //Sets an Entity object to the entity that the i represents

      e->update(); //Updates the entity based on it's class
      e->anim.update(); //Updates its animation using the update function from the animation class

      if (e->life==false) {i=entities.erase(i); delete e;} //If the entity's life is false then the entity is erased from the game
      else i++; //If it's true then i can go to the next entity in the list
    }

   //////draw//////
   app.draw(background); //Draws the background for the game window

   for(auto i:entities) { //For loop that draws all of the entities in the list
       i->draw(app);
   }

   app.display(); //Displays everything drawn onto the app
    }

    return 0;
}
