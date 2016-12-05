// This #include statement was automatically added by the Particle IDE.
#include "musicLib.h"

// This #include statement was automatically added by the Particle IDE.
#include "motorCtrl.h"

#include "Spark_HC_SR04/firmware/HC_SR04.h"

double cm = 0.0;

int trigPin = D1;
int echoPin = D2;
int servoPin = D0;
int enablePin = A0;
int speakerPin = TX;

const char Forward = 138;
const char Left = Forward + 45;
const char Right = Left - 90;

/*
curl https://api.spark.io/v1/devices/<deviceid>/cm?access_token=<accesstoken>
curl https://api.spark.io/v1/devices/<deviceid>/cm?access_token=<accesstoken>
curl "https://api.particle.io/v1/devices/<deviceid>/squeek" -d "args=play" -d access_token=<accesstoken>
*/

Servo myservo;
HC_SR04 rangefinder = HC_SR04(trigPin, echoPin, 2, 60);

void setup()
{
    Particle.function("squeek", squeek);
    Particle.variable("cm", &cm, DOUBLE);

    Serial.begin(115200);
    Serial.print("init");

    pinMode(enablePin, INPUT_PULLUP);

    setSpeakerPin(speakerPin);

    // Setup motor pins
    setupMotor();
    myservo.attach(servoPin);

    // Go to forward position
    myservo.write(Forward);
}


const int numValues = 20;
double area[numValues];

void lookSmart()
{
    myservo.write(Left);

    for (int i = 0; i < numValues; ++i)
    {
        myservo.write(Left - 90.0 * i / numValues);
        delay(20);
        area[numValues-i-1] = rangefinder.getDistanceCM();
    }

    for (int i = 0; i < numValues; ++i)
    {
        Serial.print(" ");
        Serial.print(area[i]);
    }
    Serial.println();

    myservo.write(Right);
    delay(1000);



    for (int i = 0; i < numValues; ++i)
    {
        myservo.write(Right + 90.0 * i / numValues);
        delay(20);
        area[i] = rangefinder.getDistanceCM();
    }

    for (int i = 0; i < numValues; ++i)
    {
        Serial.print(" ");
        Serial.print(area[i]);
    }
    Serial.println();
}

int sensorPos = 0;
float leftEngine = 0.0f;
float rightEngine = 0.0f;



void follow()
{
    cm = rangefinder.getDistanceCM();

    // Calc new pos
    if (cm < 0)
    {
        sensorPos = (sensorPos - 4);
    }
    else
    {
        sensorPos = (sensorPos + 4);
    }
    sensorPos = sensorPos > 30 ? 30 : sensorPos < -30 ? -30 : sensorPos;

    // Move to new pos
    myservo.write(sensorPos + Forward);
    delay(20);

    Serial.println("Sensor: ");
    Serial.println(sensorPos);
    Serial.println(cm);

    // forward/backwards
    if (cm > 20)
    {
        leftEngine += 0.2f;
        rightEngine += 0.2f;
    }
    else if (cm < 10)
    {
        leftEngine -= 0.2f;
        rightEngine -= 0.2f;
    }
    else if (cm > 10 && cm < 20)
    {
      leftEngine = rightEngine = 0.0f;
    }

    // Turn robot
    if (sensorPos < -20)
    {
        sensorPos = -18;
        leftEngine += -0.2f;
        rightEngine += 0.2f;
    }
    else if (sensorPos > 20)
    {
        sensorPos = 18;
        leftEngine += 0.2f;
        rightEngine += -0.2f;
    }
    else
    {
      //leftEngine = rightEngine = 0.0f;
    }

    leftEngine = constrain(leftEngine, -1.0f, 1.0f);
    rightEngine = constrain(rightEngine, -1.0f, 1.0f);

    engageEngines(leftEngine, rightEngine);

      //Serial.println("Engine: ");
      //Serial.println(leftEngine);
      //Serial.println(rightEngine);
}

void move()
{
  cm = rangefinder.getDistanceCM();

  if (cm == -1 || cm > 40)
  {
    leftEngine += 0.2f;
    rightEngine += 0.2f;
  }
  else if (cm < 15)
  {
    leftEngine -= 0.4f;
    rightEngine -= 0.4f;
  }
  else if (cm > 20 && cm < 30)
  {
    leftEngine = rightEngine = 0.0f;
  }

  leftEngine = constrain(leftEngine, -1.0f, 1.0f);
  rightEngine = constrain(rightEngine, -1.0f, 1.0f);
  engageEngines(leftEngine, rightEngine);

        //Serial.println("Engine: ");
        //Serial.println(leftEngine);
        //Serial.println(rightEngine);
}

void loop()
{
    int enabled = digitalRead(enablePin);

    if (enabled)
    {
        follow();
        //lookSmart();
        //move();
    }
    else
    {
        stop();
    }
}


Song song1 { "cdfda ag cdfdg gf ", "111111442111111442", 150 };
Song zak { "gggabceeagfe gggabCeeeagfdc ", "1111121121121111111111111121", 250 };
Song jarig { "ggggCgaaCbag defageddCbag ", "21213321111422121332111142", 150 };
Song squalch { "CCb", "111", 80 };

//curl "https://api.particle.io/v1/devices/3e0041000447343138333038/squeek"  -d "args=play" -d access_token=2acdc28d7946e796937d091a30fc3a36602deffe

int squeek(String command)
{
    if(command == "play")
    {
        playSong(zak);
        return 1;
    }
    else if(command == "jarig")
    {
        playSong(jarig);
        return 2;
    }
    else
    {
        Song test;
        test.notes = command;
        test.beats = "1111111111111111111111111111111111111111111";
        test.tempo = 150;
        playSong(test);
    }
    return 0;
}
