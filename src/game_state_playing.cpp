#include "game.h"

#pragma warning(push, 0)
#include <cassert>

#include <hash.h>

#include <engine/input.h>
#include <engine/action_binds.h>
#include <engine/log.h>
#include <engine/canvas.h>
#pragma warning(pop)

namespace game {
using namespace foundation;

void game_state_playing_enter(engine::Engine &engine, Game &game) {
    engine::init_canvas(engine, *game.canvas, game.config);
}

void game_state_playing_leave(engine::Engine &engine, Game &game) {
}

void game_state_playing_on_input(engine::Engine &engine, Game &game, engine::InputCommand &input_command) {
    assert(game.action_binds != nullptr);

    if (input_command.input_type == engine::InputType::Key) {
        bool pressed = input_command.key_state.trigger_state == engine::TriggerState::Pressed;
        bool released = input_command.key_state.trigger_state == engine::TriggerState::Released;

        engine::ActionBindsBind bind = engine::bind_for_keycode(input_command.key_state.keycode);
        if (bind == engine::ActionBindsBind::NOT_FOUND) {
            log_error("ActionBind not found for keycode %d", input_command.key_state.keycode);
            return;
        }

        uint64_t bind_key = static_cast<uint64_t>(bind);
        ActionHash action_hash = ActionHash(hash::get(game.action_binds->bind_actions, bind_key, (uint64_t)0));

        switch (action_hash) {
        case ActionHash::QUIT: {
            if (pressed) {
                transition(engine, game, GameState::Quitting);
            }
            break;
        }
        default:
            break;
        }
    }
}

void game_state_playing_update(engine::Engine &engine, Game &game, float t, float dt) {
    (void)t;
    (void)dt;

    assert(game.canvas != nullptr);
    using namespace engine::canvas;

    engine::Canvas &c = *game.canvas;
    clear(c, engine::color::black);

    sprite(c, 812, 8, 8, engine::color::pico8::peach);
}

void game_state_playing_render(engine::Engine &engine, Game &game) {
    engine::render_canvas(engine, *game.canvas);
}

} // namespace game
