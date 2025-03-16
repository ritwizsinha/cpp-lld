/*
Given a queue of songs.
Implement a state pattern for changing the song.

State Actions 
Paused  Play    Next    Pause Previous
Running Play    Next    Pause Previous



3:23 Pm
*/


#include <vector>
#include <string>
#include <list>
#include <thread>
#include <iostream>

using namespace std;

struct Song {
    string name;
    int duration;
    bool operator==(const Song &other) const {
        return other.duration == duration and other.name == name;
    }
};


class PlaylistState {
    public:
    virtual void next() = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void previous() = 0;
    virtual ~PlaylistState() {};
};


template<>
struct hash<Song> {
    size_t operator()(const Song& song) const {
        return hash<string>()(song.name);
    }
};

class Analytics {
    public:
    static unordered_map<Song, int, hash<Song>> songDuration;
    static void pushMessage(const Song& song, int duration) {
        songDuration[song] += duration;
    }
};



unordered_map<Song, int, hash<Song>> Analytics::songDuration;



struct Playlist {
    list<Song> songs;
    list<Song>::iterator currentSong;
    shared_ptr<PlaylistState> state;

    Playlist(vector<Song> &songs): songs(begin(songs), end(songs)), currentSong(this->songs.begin()) {
    }


    void addSong(const Song &song) {
        songs.push_back(song);
    }

    void moveToNextSong() {
        currentSong++;
        if (currentSong == songs.end()) currentSong = songs.begin();
    }

    void moveToPreviousSong() {
        if (currentSong != begin(songs)) currentSong--;
    }


    void setState(shared_ptr<PlaylistState> current_state) {
        state = std::move(current_state);
    }

    const Song getSong() 
    {
        return *currentSong;
    }

    void next() {
        state->next();
    }

    void previous() {
        state->previous();
    }

    void pause() {
        state->pause();
    }

    void play() {
        state->play();
    }



};

class PauseState : public PlaylistState {
    shared_ptr<Playlist> playlist;
    public:

    PauseState(shared_ptr<Playlist>  playlist);
    void next() override;

    void play() override;
    void previous() override;
    void pause() override;
};

class PlayState : public PlaylistState {
    using Subscriber = function<void(const Song&, int)>;
    shared_ptr<Playlist> playlist;
    vector<Subscriber> subscribers;
    int startTime;
    public:
    PlayState(Subscriber sub, shared_ptr<Playlist> playlist):startTime(time(0)), playlist(playlist), subscribers({ sub }) {

    }
    void next() override {
        playlist->moveToNextSong();
        cout << "PLAY: Moving next song " << playlist->getSong().name << endl; 
        auto state = make_shared<PauseState>(playlist);
        playlist->setState(state);
    }

    void previous() override {
        playlist->moveToPreviousSong();
        cout << "PLAY: Moving previous song " << playlist->getSong().name << endl; 
        auto state = make_shared<PauseState>(playlist);
        playlist->setState(state);
    }

    void play() override {
        cout << "PLAY: Playing song " << playlist->getSong().name << endl; 
        this->pause();
    }

    void pause() override {
        cout << "PLAY: Pausing song " << playlist->getSong().name << endl; 
        auto state = make_shared<PauseState>(playlist);
        playlist->setState(state);
    }
    ~PlayState() override {
        auto endTime = time(0);
        auto song = playlist->getSong();
        for(auto &sub : subscribers) {
            sub(song, endTime - startTime);
        }
    }
};

PauseState::PauseState(shared_ptr<Playlist>  playlist) : playlist(playlist) {}
void PauseState::next() {
    playlist->moveToNextSong();
    cout << "PAUSED: Moving next song " << playlist->getSong().name << endl; 
}

void PauseState::play(){
    cout << "PAUSED:  Playing song: " << playlist->getSong().name <<endl;
    auto playlistState = make_shared<PlayState>(Analytics::pushMessage, playlist);
    playlist->setState(playlistState);
}

void PauseState::previous() {
    playlist->moveToPreviousSong();
    cout << "PAUSED: Moving previous song " << playlist->getSong().name << endl; 
}

void PauseState::pause(){
    this->play();
}





int main() {
    srand(time(0));
    vector<Song> songs = {
        {"False Confidence", 1000},
        {"Demons", 200},
        {"I love you", 1000},
        {"Ikimonogakari", 200},
        {"Blue bird", 1000},
        {"Despacito", 1002}
    };

    auto playlist = make_shared<Playlist>(songs);
    playlist->setState(make_shared<PauseState>(playlist));
    for(int i = 0; i < 20; i++) {
        int sleepTime = rand() % 5000;
        this_thread::sleep_for(chrono::milliseconds(sleepTime));
        int action = rand() % 4;

        switch(action) {
            case 0 : {
                playlist->play();
                break;
            }
            case 1 : {
                playlist->pause();
                break;
            }

            case 2: {
                playlist->next();
                break;
            }
            case 3: {
                playlist->previous();
                break;
            }

        }
    }

    for(auto &[song, duration] : Analytics::songDuration) {
        cout << song.name <<" "<<duration << endl;
    }
}