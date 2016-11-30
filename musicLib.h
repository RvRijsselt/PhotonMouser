
typedef struct Song {
    String notes;
    String beats;
    int tempo;
} Song;

void setSpeakerPin(int pin);

void playSong(const struct Song &song);
