#include "server.h"
#include <thread>
#include "player.h"
#include <chrono>

//macos: g++ *.cpp -o main -levent -ljsoncpp -std=c++11 -I/opt/homebrew/include -L/opt/homebrew/lib -Wall
//ubuntu: g++ *.cpp -o main -levent -ljsoncpp -Wall
int main(){
    PlayerServer ps;
    return 0;
}