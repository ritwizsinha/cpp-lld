/*
Below is the final modified question along with detailed reasoning behind each design decision.

---

### **Modified Low-Level Design Question: Online Music Streaming Service**

#### **Problem Statement:**

You are given:
- A list of songs in the format:  
  ```
  {song_id, title, artist, album, duration_in_seconds}
  ```
- A list of users in the format:  
  ```
  {user_id, name, subscription_type}
  ```
- A list of playlists in the format:  
  ```
  {playlist_id, user_id, [song_ids]}
  ```

Design a **music streaming service** that supports the following functionalities:
1. **Search API:**  
   - **Input:** A search query string (which could be a song title, artist name, or album name).  
   - **Output:** A list of matching songs with details (song_id, title, artist, album, duration).
   
2. **Playlist Management:**  
   - **APIs to create, update, and delete playlists** for a user.
   
3. **Playback Control:**  
   - Support operations: **play, pause, skip, and seek** on a song.  
   - Track and update the current playback position accurately.
   
4. **Concurrency Handling:**  
   - Ensure that multiple users can stream, search, and update playlists simultaneously without conflicts.
   
5. **Recommendation System:**  
   - Based on a user's listening history (stream events in the format: `{user_id, song_id, timestamp, action}` where `action` could be "play", "pause", "skip"), generate a list of recommended songs.
   
6. **Analytics:**  
   - Given a time window, provide analytics such as the **number of plays per song**, **most active users**, and **average session duration**.

---


11:19 Am

SongStore
- vector<Song>

Song
- id
- title
- artist
- duration_in_seconds

Search(Condition) -> cursor

Users
- user_id
- name
- sub_type

Playlists
- id
- user_id
- vector<song>

PlaylistManager
- vector<playlist>
- create(vector<Song>, user_id) -> Playlist
- delete(playlist id, user_id) 

SongState
- time
- state enum
- id
ActivityManager
- vector<Song> Played
- vector<Song> comingUp
- SongState
- next
- previous


Recommender
    SongStore
    feed(user_id, song_id, timestamp, action) -> Cursor

Analytics
    - For time period [s, e] number of plays per song
    - most active users [s, e]
    - average session duration [s, e] // assuming user finishes all songs that they start listening to.
*/

/*
Song
- id
- title
- artist
- duration_in_seconds
*/
#include <vector>
#include <string>
#include <sstream>
#include <unordered_map>

using namespace std;

struct Song {
    int id;
    string title;
    string artist;
    int duration;
    Song(const string &title, const string &artist, int duration) : title(title), artist(artist), duration(duration) {
        id = rand();
    };
};


struct SongStore {
    static vector<shared_ptr<Song>> songs;
    static void addSong(shared_ptr<Song> song) {
        songs.push_back(move(song));
    }

    static void removeSong(const int id) {
        auto it = find(begin(songs), end(songs), [id](const shared_ptr<Song>& song) {song->id == id;});
        if (it != songs.end()) songs.erase(it);
    }

    
};

struct User {
    int id;
    string name;
    int sub_type;
};

/*

Playlists
- id
- user_id
- vector<song>

*/
struct Playlist {
    int id;
    int user_id;
    vector<shared_ptr<Song>> songs;
    Playlist(int id, int user_id, vector<shared_ptr<Song>> songs): id(id), user_id(user_id), songs(move(songs)) {

    }
};


class PlaylistManager {
    private:
    static unordered_map<int, shared_ptr<Playlist>> playlists;
    PlaylistManager() {};
    public:
    int addPlaylistToUser(vector<shared_ptr<Song>> songs, int user_id) {
        auto newPlaylist = make_shared<Playlist>(user_id, songs);
        auto id = rand();
        playlists[id] = move(newPlaylist);

        return id;
    }

    void removePlaylist(const int id) {
        if (playlists.find(id) == playlists.end()) return;

        playlists.erase(id);
    };
};

/*


SongState
- time
- state enum
- id
ActivityManager
- vector<Song> Played
- vector<Song> comingUp
- SongState
- next
- previous

*/

enum class SongState {
    STARTED,
    ENDED,
    PAUSED,
    PLAYING,
    SKIP,
};

class InteractedSong {
    int time;
    SongState state;
    int song_id;
    long int started_at;
    public:
    InteractedSong(const int id):time(0),state(SongState::STARTED), song_id(id), started_at(time(0)){}
    void play() {
        if (state == SongState::PAUSED) {
            state = SongState::PLAYING;
            auto current_time = time(0);
            time += current_time - started_at;
            started_at = current_time;
        }
    }

    void pause() {
        if ()
    }

};