// This #include statement was automatically added by the Particle IDE.
#include "musicLib.h"

// This #include statement was automatically added by the Particle IDE.
//#include "HC_SR04/HC_SR04.h"
#include "Spark_HC_SR04/firmware/HC_SR04.h"

// Original code taken from the internet.
// Not sure where.
//   Perhaps here http://superqk.tumblr.com/
//    or here https://github.com/mjs513/Sainsmart_4WD_Obstacle_Avoid_TestBed/blob/master/Archive/Sainsmart_4wd_Test1_code1/Sainsmart_4wd_Test1_code1.ino
//   Not sure any more, just google a comment and you'll find lots of examples.
//
// Added some scripting and a website to control it for a Sinterklaas surprise :-)
// Also added a piezo buzzer to play some Sinterklaas songs: "De zak van Sinterklaas" and "Sinterklaas is jarig".
//
// mos.html contains a Sinterklaas poem with a single button that makes the mouse squeek 5 times. After the 5th it will automatically start moving. After the 6th and 7th it will play a nice song.
// mosap.html contains a little control panel to perform some functions separately. Start/Stop/Reset/play song/etc.

int trigPin = D1;
int echoPin = D2;
int servoPin = D0;
int enablePin = A0;
int speakerPin = TX;

int dirA1 = D3;
int dirA2 = D4;
int dirB1 = D5;
int dirB2 = D6;
int pwmA = A4;
int pwmB = A5;

const char forwardpos = 80;// 138;
const int distancelimit = 26; //Distance limit for obstacles in front
const int sidedistancelimit = 20; //Minimum distance in cm to obstacles at both sides (the robot will allow a shorter distance sideways)

HC_SR04 rangefinder = HC_SR04(trigPin, echoPin, 6, 400);

int leftscanval, centerscanval, rightscanval, ldiagonalscanval, rdiagonalscanval;
char choice;

int distance;
int numcycles = 0;
char turndirection; //Gets 'l', 'r' or 'f' depending on which direction is obstacle free
const int turntime = 350; //Time the robot spends turning (miliseconds)
int thereis;
int possibleCollision;
Servo head;

const int speed = 235;


/*
curl https://api.spark.io/v1/devices/<deviceid>/cm?access_token=<accesstoken>
curl https://api.spark.io/v1/devices/<deviceid>/cm?access_token=<accesstoken>
curl "https://api.particle.io/v1/devices/<deviceid>/squeek" -d "args=play" -d access_token=<accesstoken>
*/
Song song1 { "cdfda ag cdfdg gf ", "111111442111111442", 150 };
Song zak { "gggabceeagfe gggabCeeeagfdc ", "1111121121121111111111111121", 250 };
Song jarig { "ggggCgaaCbag defageddCbag ", "21213321111422121332111142", 150 };
Song taunt { "ggeage ", "11112211", 150 };
Song squalch { "CCb", "111", 80 };

bool autoMove = false;
bool lastEnableBit = false;
bool rndCommand = false;
int nrOfInterrupts = 0;

void setup()
{
    Particle.function("squeek", squeek);

    Serial.begin(115200);
    Serial.print("init");

    head.attach(servoPin);
    head.write(forwardpos);

    pinMode(enablePin, INPUT_PULLUP);
    setSpeakerPin(speakerPin);

    //motor
    pinMode(dirA1, OUTPUT);
    pinMode(dirA2, OUTPUT);
    pinMode(dirB1, OUTPUT);
    pinMode(dirB2, OUTPUT);
    pinMode(pwmA, OUTPUT);
    pinMode(pwmB, OUTPUT);

    lastEnableBit = (digitalRead(enablePin) == LOW);
}

void loop()
{
    bool enabled = (digitalRead(enablePin) == LOW);

    if (enabled != lastEnableBit)
    {
        lastEnableBit = enabled;
        autoMove = !autoMove;
    }

    processCommand();
    checkCommand();

    if (autoMove)
    {
        move();
        //watchsurrounding();
    }
    else
    {
        stop();
        delay(100);

        if (nrOfInterrupts > 2 && nrOfInterrupts < 6)
        {
            int cm = watch();
            if (cm > 30 && cm < 400)
            {
                rndCommand = true;
            }
        }
    }
}


void checkCommand()
{
    if (rndCommand == false) return;
    rndCommand = false;

    switch (++nrOfInterrupts)
    {
        case 1:
            play("*", 300);
            break;
        case 2:
            play("!", 300);
            break;
        case 3:
            play("#", 300);
            break;
        case 4:
            play("! *", 150);
            break;
        case 5:
            play("* ! #", 300);
            break;
        case 6:
            autoMove = true;
            forward(true);
            delay(1200);
            stop();
            playSong(taunt);
            break;
        case 7:
            stop();
            play("cdefgabC", 80);
            break;
        case 8:
            stop();
            turn(false);
            playSong(jarig);
            stop();
            break;
        case 9:
            stop();
            turn(true);
            playSong(zak);
            stop();
            break;
        case 10:
            stop();
            playSong(taunt);
            nrOfInterrupts = 7;
            break;
    }
}

String lastCommand = "";

int squeek(String command)
{
    if (command == "ping")
    {
        return 1234;
    }

    lastCommand = command;
    return 1;
}

void processCommand()
{
    if (lastCommand == "") return;

    if (lastCommand == "go")
    {
        stop();
        autoMove = true;
        play("b d f", 150);
    }
    else if (lastCommand == "stop")
    {
        stop();
        autoMove = false;
        play("f d b", 300);
    }
    else if (lastCommand == "reset")
    {
        stop();
        nrOfInterrupts = 0;
        autoMove = false;
        play("*", 100);
    }
    else if (lastCommand == "taunt")
    {
        stop();
        playSong(taunt);
    }
    else if (lastCommand == "zak")
    {
        stop();
        playSong(zak);
    }
    else if(lastCommand == "jarig")
    {
        stop();
        playSong(jarig);
    }
    else if(lastCommand == "squeek")
    {
        rndCommand = true;
    }
    else
    {
        stop();
        Song test;
        test.notes = lastCommand;
        test.beats = "1111111111111111111111111111111111111111111111";
        test.tempo = 150;
        playSong(test);
    }
    lastCommand = "";
}

void forward(bool dir)
{
    digitalWrite(dirA1, dir ? HIGH : LOW);
    digitalWrite(dirA2, dir ? LOW : HIGH);
    digitalWrite(dirB1, dir ? HIGH : LOW);
    digitalWrite(dirB2, dir ? LOW : HIGH);
    analogWrite(pwmB, speed);
    analogWrite(pwmA, speed);
}

void turn(bool right)
{
    digitalWrite(dirA1, right ? LOW : HIGH);
    digitalWrite(dirA2, right ? HIGH : LOW);
    digitalWrite(dirB1, right ? HIGH : LOW);
    digitalWrite(dirB2, right ? LOW : HIGH);
    analogWrite(pwmB, speed);
    analogWrite(pwmA, speed);
}

void stop()
{
    analogWrite(pwmB, 0);
    analogWrite(pwmA, 0);
    digitalWrite(dirB1, LOW);
    digitalWrite(dirB2, LOW);
    digitalWrite(dirA1, LOW);
    digitalWrite(dirA2, LOW);
}

int factor = 1;

int watch()
{
    int val = rangefinder.getDistanceCM();

    return val > 0 ? val : 500;
}

const int watchRange = 70;

void watchsurrounding()
{
  centerscanval = watch();
  if(centerscanval<distancelimit){stop();}

  head.write(forwardpos+watchRange/2);
  delay(150 * factor);
  ldiagonalscanval = watch();
  if(ldiagonalscanval<distancelimit){stop();}

  head.write(forwardpos+watchRange);
  delay(300 * factor);
  leftscanval = watch();
  if(leftscanval<sidedistancelimit){stop();}

  head.write(forwardpos);
  delay(300 * factor);
  centerscanval = watch();
  if(centerscanval<distancelimit){stop();}

  head.write(forwardpos-watchRange/2);
  delay(150 * factor);
  rdiagonalscanval = watch();
  if(rdiagonalscanval<distancelimit){stop();}

  head.write(forwardpos-watchRange);
  delay(300 * factor);
  rightscanval = watch();
  if(rightscanval<sidedistancelimit){stop();}

  head.write(forwardpos); //Finish looking around (look forward again)
  delay(300 * factor);
}

char decide(){
  watchsurrounding();
  if (leftscanval>rightscanval && leftscanval>centerscanval){
    choice = 'l';
  }
  else if (rightscanval>leftscanval && rightscanval>centerscanval){
    choice = 'r';
  }
  else{
    choice = 'f';
  }
  return choice;
}

void move()
{
  forward(true);  // if nothing is wrong go forward using go() function above.
  ++numcycles;
  if(numcycles>130){ //Watch if something is around every 130 loops while moving forward
    watchsurrounding();
    if(leftscanval<sidedistancelimit || ldiagonalscanval<distancelimit){
        turn(true);
        delay(turntime);
    }
    if(rightscanval<sidedistancelimit || rdiagonalscanval<distancelimit){
        turn(false);
        delay(turntime);
    }
    numcycles=0; //Restart count of cycles
  }

  distance = watch();
  if (distance > 490) possibleCollision++;
  else possibleCollision = 0;
  if (possibleCollision > 10) {
      forward(false);
      delay(500);
      stop();
      play("f c", 150);
      possibleCollision = 0;
      goto retry;
  }

  if (distance<distancelimit){ // The robot will just stop if it is completely sure there's an obstacle ahead (must test 25 times) (needed to ignore ultrasonic sensor's false signals)
      ++thereis;}
  if (distance>distancelimit){
      thereis=0;} //Count is restarted
  if (thereis > 25){
    forward(false);
    delay(500);
    stop(); // Since something is ahead, stop moving.
    play("g d", 200);
retry:
    stop();
    turndirection = decide(); //Decide which direction to turn.
    switch (turndirection){
      case 'l':
        turn(false);
        delay(turntime);
        goto retry;
        break;
      case 'r':
        turn(true);
        delay(turntime);
        goto retry;
        break;
      case 'f':
        ; //Do not turn if there was actually nothing ahead
        break;
    }
    numcycles = 65;
    thereis=0;
  }
}
