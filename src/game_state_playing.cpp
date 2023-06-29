#include "game.h"
#include "util.h"

#pragma warning(push, 0)
#include <cassert>

#include <hash.h>
#include <queue.h>
#include <string_stream.h>
#include <temp_allocator.h>

#include <engine/action_binds.h>
#include <engine/canvas.h>
#include <engine/input.h>
#include <engine/log.h>

#include <imgui.h>
#pragma warning(pop)

namespace game {
using namespace foundation;

void game_state_playing_enter(engine::Engine &engine, Game &game) {
    engine::init_canvas(engine, *game.canvas, game.config);

    game.player = Player();
    game.player.pos = {24, 24};

    game.enemy = Enemy();
    game.enemy.pos = {game.canvas->width / 2.0f - game.enemy.bounds.size.x / 2.0f, game.canvas->height / 2.0f - game.enemy.bounds.size.y / 2.0f};
}

void game_state_playing_leave(engine::Engine &engine, Game &game) {
    (void)engine;
    (void)game;
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
        case ActionHash::UP: {
            if (pressed) {
                game.player.button_up = true;
            } else if (released) {
                game.player.button_up = false;
            }
            break;
        }
        case ActionHash::LEFT: {
            if (pressed) {
                game.player.button_left = true;
            } else if (released) {
                game.player.button_left = false;
            }
            break;
        }
        case ActionHash::RIGHT: {
            if (pressed) {
                game.player.button_right = true;
            } else if (released) {
                game.player.button_right = false;
            }
            break;
        }
        case ActionHash::DOWN: {
            if (pressed) {
                game.player.button_down = true;
            } else if (released) {
                game.player.button_down = false;
            }
            break;
        }
        case ActionHash::ACTION: {
            if (pressed) {
                game.player.button_action = true;
            } else if (released) {
                game.player.button_action = false;
            }
            break;
        }
        case ActionHash::DEBUG: {
            if (pressed) {
                game.imgui_debug = !game.imgui_debug;
            }
            break;
        }
        default:
            break;
        }
    }
}

void game_state_playing_update(engine::Engine &engine, Game &game, float t, float dt) {
    (void)engine;
    
    using namespace foundation;
    
    // Update player
    {
        float steer_x = 0.0f;
        float steer_y = 0.0f;

        if (game.player.button_up) {
            steer_y -= game.player.speed_incr * dt;
        }
        if (game.player.button_down) {
            steer_y += game.player.speed_incr * dt;
        }
        if (game.player.button_left) {
            steer_x -= game.player.speed_incr * dt;
        }
        if (game.player.button_right) {
            steer_x += game.player.speed_incr * dt;
        }

        game.player.vel.x += steer_x;
        game.player.vel.y += steer_y;

        // velocity magnitude
        float vel_mag = sqrtf(game.player.vel.x * game.player.vel.x + game.player.vel.y * game.player.vel.y);

        // check if faster than max
        if (vel_mag > game.player.max_speed) {
            float norm_vel_x = game.player.vel.x / vel_mag;
            float norm_vel_y = game.player.vel.y / vel_mag;
            game.player.vel.x = norm_vel_x * game.player.max_speed;
            game.player.vel.y = norm_vel_y * game.player.max_speed;
            vel_mag = game.player.max_speed;
        } else if (vel_mag < 0.02f) {
            // check if almost stopped
            game.player.vel.x = 0.0f;
            game.player.vel.y = 0.0f;
        } else {
            // apply a little bit of drag
            game.player.vel.x = game.player.vel.x * (1.0f - game.player.drag);
            game.player.vel.y = game.player.vel.y * (1.0f - game.player.drag);
        }

        // update position
        game.player.pos.x += game.player.vel.x;
        game.player.pos.y += game.player.vel.y;

        // check for out of bounds
        // account for player size
        {
            if (game.player.pos.x + game.player.bounds.origin.x + game.player.bounds.size.x > game.canvas->width - 1) {
                game.player.pos.x = (float)game.canvas->width - game.player.bounds.size.x - 1;
                game.player.vel.x = 0.0f;
            }

            if (game.player.pos.x + game.player.bounds.origin.x < 1) {
                game.player.pos.x = -(float)game.player.bounds.origin.x + 1;
                game.player.vel.x = 0.0f;
            }

            if (game.player.pos.y + game.player.bounds.origin.y + game.player.bounds.size.y > game.canvas->height - 1) {
                game.player.pos.y = (float)game.canvas->height - game.player.bounds.origin.y - game.player.bounds.size.y - 1;
                game.player.vel.y = 0.0f;
            }

            if (game.player.pos.y + game.player.bounds.origin.y < 12) {
                game.player.pos.y = -(float)game.player.bounds.origin.y + 12;
                game.player.vel.y = 0.0f;
            }
        }
    }

    // update enemy
    {
        // rotate bullet spawner
        game.enemy.rot += game.enemy.rot_speed * dt;
        
        // update enemy position
        float tt = t * game.enemy.speed;
        float scale = 2.0f / (3.0f - cosf(2.0f * tt));
        float x = scale * cosf(tt);
        float y = scale * sinf(2.0f * tt) / 2.0f;
        float x2 = game.canvas->width / 2.0f + x * 72.0f;
        float y2 = game.canvas->height / 2.0f + y * 80.0f;

        game.enemy.pos.x = x2;
        game.enemy.pos.y = y2;
        
        // spawn 4 bullets every few frames
        if (game.enemy.bullet_cooldown >= game.enemy.bullet_rate) {
            for (int i = 0; i < 4; ++i) {
                Bullet b;
                b.pos.x = game.enemy.pos.x + game.enemy.bounds.origin.x + game.enemy.bounds.size.x / 2.0f;
                b.pos.y = game.enemy.pos.y + game.enemy.bounds.origin.y + game.enemy.bounds.size.y / 2.0f;
                
                float rot = game.enemy.rot + 0.25f * i;
                float vel_x = 0.0f;
                float vel_y = game.bullet_speed;
                
                b.vel.x = vel_x * cosf(rot) - vel_y * sinf(rot);
                b.vel.y = vel_x * sinf(rot) + vel_y * cosf(rot);
                
                array::push_back(game.bullets, b);
            }
            
            game.enemy.bullet_cooldown = dt;
        } else {
            game.enemy.bullet_cooldown += dt;
        }
    }
    
    // update bullets
    {
        for (Bullet *bullet_iter = array::begin(game.bullets); bullet_iter != array::end(game.bullets); ++bullet_iter) {
            bullet_iter->pos.x += bullet_iter->vel.x * dt;
            bullet_iter->pos.y += bullet_iter->vel.y * dt;
        }

        // check for out of bounds bullets
        const math::Rect game_rect = { { 0, 12 }, { game.canvas->width, game.canvas->height - 12 } };
        for (uint32_t i = 0; i < array::size(game.bullets); ++i) {
            if (!math::is_inside(game_rect, game.bullets[i].pos)) {
                swap_pop(game.bullets, i);
            }
        }
    }
}

void game_state_playing_render(engine::Engine &engine, Game &game) {
    using namespace engine::canvas;
    namespace ss = foundation::string_stream;
    namespace color = engine::color::pico8;

    TempAllocator128 ta;

    engine::Canvas &c = *game.canvas;
    clear(c, engine::color::black);
    
    // draw bullets
    for (Bullet *bullet_iter = array::begin(game.bullets); bullet_iter != array::end(game.bullets); ++bullet_iter) {
        pset(c, (int32_t)bullet_iter->pos.x, (int32_t)bullet_iter->pos.y, color::yellow);
    }
    
    // draw player
    sprite(c, 405, (int32_t)game.player.pos.x, (int32_t)game.player.pos.y, color::peach);
    sprite(c, 206, (int32_t)game.player.pos.x, (int32_t)game.player.pos.y + c.sprite_size, color::peach);

    // draw enemy
    sprite(c, 514, (int32_t)game.enemy.pos.x, (int32_t)game.enemy.pos.y, color::light_gray, 2, 2);

    // draw ui
    rectangle(c, 0, 0, c.width - 1, c.height - 1, color::dark_blue);
    ss::Buffer score_buffer(ta);
    ss::printf(score_buffer, "score:%u", game.player.score);
    print(c, ss::c_str(score_buffer), 2, 2, color::white);
    line(c, 0, 11, c.width - 1, 11, color::dark_blue);

    engine::render_canvas(engine, *game.canvas);
}

void game_state_playing_render_imgui(engine::Engine &engine, Game &game) {
    (void)engine;

    if (game.imgui_debug) {
        ImGui::SetNextWindowSize(ImVec2(200, 200), ImGuiCond_Once);
        ImGui::SetNextWindowPos(ImVec2(8, 8), ImGuiCond_Once);
        if (!ImGui::Begin("Debug", &game.imgui_debug)) {
            ImGui::End();
            return;
        }

        ImGui::Text("Player");
        ImGui::Text("Position: %.1f, %.1f", game.player.pos.x, game.player.pos.y);
        ImGui::Text("Velocity: %.1f, %.1f", game.player.vel.x, game.player.vel.y);

        ImGui::Text("Enemy");
        ImGui::Text("Position: %.1f, %.1f", game.enemy.pos.x, game.enemy.pos.y);

        ImGui::Text("Bullets: %d", array::size(game.bullets));

        if (ImGui::Button("Clear Bullets")) {
            array::clear(game.bullets);
        }

        ImGui::End();
    }
}

} // namespace game
