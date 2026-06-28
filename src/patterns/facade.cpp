// facade.cpp - Facade Pattern (MediaPlayer)
// Interview Relevance: Simplifies complex subsystem interactions behind a unified
// interface. Tests understanding of API design, encapsulation of complexity, and
// when to introduce an abstraction layer. Real use: SDL, SFML, database ORMs.
// Compile: g++ -std=c++20 -Wall facade.cpp -o facade

#include <iostream>
#include <string>
#include <cassert>

// Complex subsystems
class AudioDecoder {
public:
    void loadCodec(const std::string& file) { std::cout << "  [Audio] Loading codec for: " << file << "\n"; }
    void decode() { std::cout << "  [Audio] Decoding audio stream\n"; }
    void play() { std::cout << "  [Audio] Playing audio\n"; }
};

class VideoDecoder {
public:
    void loadCodec(const std::string& file) { std::cout << "  [Video] Loading codec for: " << file << "\n"; }
    void decode() { std::cout << "  [Video] Decoding video frames\n"; }
    void render() { std::cout << "  [Video] Rendering video\n"; }
};

class SubtitleParser {
public:
    void load(const std::string& file) { std::cout << "  [Subs] Loading subtitles for: " << file << "\n"; }
    void sync() { std::cout << "  [Subs] Synchronizing timestamps\n"; }
};

class Display {
public:
    void setResolution(int w, int h) { std::cout << "  [Display] Resolution: " << w << "x" << h << "\n"; }
    void show() { std::cout << "  [Display] Showing on screen\n"; }
};

// Facade - simple interface wrapping complex subsystem interactions
class MediaPlayer {
    AudioDecoder audio_;
    VideoDecoder video_;
    SubtitleParser subs_;
    Display display_;
    bool playing_ = false;
public:
    void play(const std::string& file) {
        std::cout << "MediaPlayer::play(\"" << file << "\")\n";
        display_.setResolution(1920, 1080);
        audio_.loadCodec(file);
        video_.loadCodec(file);
        subs_.load(file);
        audio_.decode();
        video_.decode();
        subs_.sync();
        audio_.play();
        video_.render();
        display_.show();
        playing_ = true;
    }

    void stop() {
        std::cout << "MediaPlayer::stop()\n";
        playing_ = false;
    }

    bool isPlaying() const { return playing_; }
};

int main() {
    MediaPlayer player;

    // Client only knows the simple interface
    player.play("movie.mp4");
    assert(player.isPlaying());

    std::cout << "\n";
    player.stop();
    assert(!player.isPlaying());

    std::cout << "\nKey insight: Client calls one method instead of coordinating\n";
    std::cout << "4 subsystems with 10+ calls. Facade doesn't prevent direct\n";
    std::cout << "subsystem access if needed - it's a convenience, not a cage.\n";
    std::cout << "\nAll assertions passed!\n";
    return 0;
}
