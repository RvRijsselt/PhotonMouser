
typedef struct Song {
    String notes;
    String beats;
    int tempo;
} Song;

void setSpeakerPin(int pin);

void play(const String &notes, int tempo);
void playSong(const struct Song &song);
