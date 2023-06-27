#include "game.h"

namespace game {
using namespace foundation;

void game_state_playing_enter(engine::Engine &engine, Game &game) {
}

void game_state_playing_leave(engine::Engine &engine, Game &game) {
}

void game_state_playing_on_input(engine::Engine &engine, Game &game, engine::InputCommand &input_command) {
}

void game_state_playing_update(engine::Engine &engine, Game &game, float t, float dt) {
    (void)t;
    (void)dt;
}

void game_state_playing_render(engine::Engine &engine, Game &game) {
    (void)engine;
    (void)game;
}

} // namespace game
