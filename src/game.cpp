#include "game.h"

#pragma warning(push, 0)
#include <memory.h>

#include <engine/engine.h>
#include <engine/input.h>
#include <engine/log.h>
#pragma warning(pop)

namespace game {
using namespace foundation;

void game_state_playing_enter(engine::Engine &engine, Game &game);
void game_state_playing_leave(engine::Engine &engine, Game &game);
void game_state_playing_on_input(engine::Engine &engine, Game &game, engine::InputCommand &input_command);
void game_state_playing_update(engine::Engine &engine, Game &game, float t, float dt);
void game_state_playing_render(engine::Engine &engine, Game &game);

Game::Game(Allocator &allocator)
: allocator(allocator)
, game_state(GameState::None) {
}

Game::~Game() {
}

void update(engine::Engine &engine, void *game_object, float t, float dt) {
    (void)engine;
    (void)t;
    (void)dt;

    if (!game_object) {
        return;
    }

    Game &game = (*(Game *)game_object);

    switch (game.game_state) {
    case GameState::None: {
        transition(engine, game, GameState::Initializing);
        break;
    }
    case GameState::Playing: {
        game_state_playing_update(engine, game, t, dt);
        break;
    }
    case GameState::Quitting: {
        transition(engine, game, GameState::Terminate);
        break;
    }
    }
}

void on_input(engine::Engine &engine, void *game_object, engine::InputCommand &input_command) {
    if (!game_object) {
        return;
    }

    Game &game = (*(Game *)game_object);

    switch (game.game_state) {
    case GameState::Playing: {
        game_state_playing_on_input(engine, game, input_command);
        break;
    }
    default: {
        break;
    }
    }
}

void render(engine::Engine &engine, void *game_object) {
    if (!game_object) {
        return;
    }

    Game &game = (*(Game *)game_object);

    switch (game.game_state) {
    case GameState::Playing: {
        game_state_playing_render(engine, game);
        break;
    }
    default: {
        break;
    }
    }
}

void render_imgui(engine::Engine &engine, void *game_object) {
    (void)engine;
    (void)game_object;
}

void on_shutdown(engine::Engine &engine, void *game_object) {
    (void)game_object;
    log_info("Quitting");
    engine::terminate(engine);
}

void transition(engine::Engine &engine, Game &game, GameState game_state) {
    if (game.game_state == game_state) {
        return;
    }

    // When leaving a game state
    switch (game.game_state) {
    case GameState::Playing: {
        game_state_playing_leave(engine, game);
        break;
    }
    case GameState::Terminate: {
        return;
    }
    default:
        break;
    }

    game.game_state = game_state;

    // When entering a new game state
    switch (game.game_state) {
    case GameState::None: {
        break;
    }
    case GameState::Initializing: {
        log_info("Initializing");
        // engine::init_sprites(*engine.sprites, game->params->game_atlas_filename().c_str());

        // game->room_templates->read(game->params->room_templates_filename().c_str());
        // game->mob_templates->read(game->params->mob_templates_filename().c_str());

        transition(engine, game, GameState::Playing);
        break;
    }
    case GameState::Playing: {
        log_info("Playing");
        game_state_playing_enter(engine, game);
        break;
    }
    case GameState::Quitting: {
        log_info("Quitting");
        break;
    }
    case GameState::Terminate: {
        log_info("Terminating");
        engine::terminate(engine);
        break;
    }
    }
}

} // namespace game
