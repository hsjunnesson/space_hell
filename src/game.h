#pragma once

#pragma warning(push, 0)
#include <collection_types.h>
#include <memory_types.h>
#pragma warning(pop)

namespace engine {
struct Engine;
struct InputCommand;
struct ActionBinds;
struct Canvas;
}; // namespace engine

typedef struct ini_t ini_t;

namespace game {

/// Murmur hashed actions.
enum class ActionHash : uint64_t {
    NONE = 0x0ULL,
    QUIT = 0x387bbb994ac3551ULL,
};

/**
 * @brief An enum that describes a specific game state.
 * 
 */
enum class GameState {
    // No game state.
    None,

    // Game state is creating, or loading from a save.
    Initializing,

    // Playing the game.
    Playing,

    // Shutting down, saving and closing the game.
    Quitting,

    // Final state that signals the engine to terminate the application.
    Terminate,
};

struct Game {
    Game(foundation::Allocator &allocator, const char *config_path);
    ~Game();

    foundation::Allocator &allocator;
    ini_t *config;
    engine::ActionBinds *action_binds;
    engine::Canvas *canvas;
    GameState game_state;
};

/**
 * @brief Updates the game
 *
 * @param engine The engine which calls this function
 * @param game_object The game to update
 * @param t The current time
 * @param dt The delta time since last update
 */
void update(engine::Engine &engine, void *game_object, float t, float dt);

/**
 * @brief Callback to the game that an input has ocurred.
 * 
 * @param engine The engine which calls this function
 * @param game_object The game to signal.
 * @param input_command The input command.
 */
void on_input(engine::Engine &engine, void *game_object, engine::InputCommand &input_command);

/**
 * @brief Renders the game
 *
 * @param engine The engine which calls this function
 * @param game_object The game to render
 */
void render(engine::Engine &engine, void *game_object);

/**
 * @brief Renders the imgui
 * 
 * @param engine The engine which calls this function
 * @param game_object The game to render
 */
void render_imgui(engine::Engine &engine, void *game_object);

/**
 * @brief Asks the game to quit.
 * 
 * @param engine The engine which calls this function
 * @param game_object The game to render
 */
void on_shutdown(engine::Engine &engine, void *game_object);

/**
 * @brief Transition a Game to another game state.
 * 
 * @param engine The engine which calls this function
 * @param game The game to transition
 * @param game_state The GameState to transition to.
 */
void transition(engine::Engine &engine, Game &game, GameState game_state);

} // namespace game
