
#include "Particle.h"
#include "musicLib.h"

int speakerPin1;

void setSpeakerPin(int pin)
{
    speakerPin1 = pin;
    pinMode(speakerPin1, OUTPUT);
}

int frequency(char note)
{
  int i;
  const int numNotes = 8;

  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  int frequencies[] = {262, 294, 330, 349, 392, 440, 494, 523};

  for (i = 0; i < numNotes; i++)
  {
    if (names[i] == note)
    {
      return(frequencies[i]);
    }
  }
  return(0);
}

void playSong(const struct Song &song)
{
    unsigned int i, duration;
    for (i = 0; i < song.notes.length(); i++)
    {
        duration = (song.beats[i] - '0') * song.tempo;

        if (song.notes[i] == ' ')
        {
            delay(duration);
        }
        else
        {
            tone(speakerPin1, frequency(song.notes[i]), duration);
            delay(duration);
        }
        delay(song.tempo/10);
    }
}
