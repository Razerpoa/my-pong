#include "raylib.h"
#include "icon.h"

const float SPEED = 450;

struct MoveableObject {
    Rectangle mesh;
    Vector2 velocity = Vector2 { 0.0f, 0.0f };
};

enum class OpponentType { Human, AI };

void reset(const float &window_width, const float &window_height,
               const int &paddle_edge_offset, MoveableObject &left_paddle,
               MoveableObject &right_paddle, MoveableObject &ball) {
    ball.mesh.x = window_width / 2.0f;
    ball.mesh.y = window_height / 2.0f;
    right_paddle.mesh.x = window_width - paddle_edge_offset;
    right_paddle.mesh.y = window_height / 2.0f - right_paddle.mesh.height / 2.0f;
    left_paddle.mesh.x = paddle_edge_offset;
    left_paddle.mesh.y = window_height / 2.0f - left_paddle.mesh.height / 2.0f;
}

int main() {
    const float window_width = 800;
    const float window_height = 600;
    
    const int paddle_edge_offset = 75;
    const float paddle_strength = 0.5f;
    int player_score = 0;
    int opponent_score = 0;
    
    bool show_settings = false;
    bool game_paused = false;
    OpponentType current_opponent = OpponentType::Human;
    
    Image icon = LoadImageFromMemory(".png", icon_data, ICON_DATA_SIZE);

    SetTraceLogLevel(LOG_NONE);
    InitWindow(window_width, window_height, "PING PONG");
    
    SetWindowIcon(icon);
    UnloadImage(icon);
    SetTargetFPS(60);
    SetExitKey(KEY_Q);

    MoveableObject left_paddle =  {
        .mesh = {
            .width = 10,
            .height = 100,
        }
    };
    MoveableObject right_paddle = {
        .mesh = {
            .width = 10,
            .height = 100,
        }
    };
    MoveableObject ball =  {
        .mesh = {
            .width = 10.0f,
            .height = 10.0f,
        }
    };

    Rectangle dotted_line = {
        .x = window_width / 2.0f - 10.0f / 2.0f,
        .y = 0.0f,
        .width = 10.0f,
        .height = 35.0f,
    };

    reset(window_width, window_height, paddle_edge_offset, left_paddle, right_paddle, ball);

    ball.velocity = {-1.0f, 0.0f};
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        left_paddle.velocity = {0.0f, 0.0f};
        right_paddle.velocity = {0.0f, 0.0f};

        if (IsKeyDown(KEY_W)) left_paddle.velocity.y = -1.0f;
        if (IsKeyDown(KEY_S)) left_paddle.velocity.y = 1.0f;

        if (current_opponent == OpponentType::Human) {
            if (IsKeyDown(KEY_UP)) right_paddle.velocity.y = -1.0f;
            if (IsKeyDown(KEY_DOWN)) right_paddle.velocity.y = 1.0f;
        }
        
        if (IsKeyPressed(KEY_ESCAPE)) {show_settings = !show_settings; game_paused = !game_paused;};
        
        if (left_paddle.mesh.y < 0.0f) left_paddle.mesh.y = 0.0f;
        if (left_paddle.mesh.y + left_paddle.mesh.height > window_height) {
            left_paddle.mesh.y = window_height - left_paddle.mesh.height;
        };
        if (right_paddle.mesh.y + right_paddle.mesh.height > window_height) {
            right_paddle.mesh.y = window_height - right_paddle.mesh.height;
        };
        if (right_paddle.mesh.y < 0.0f) right_paddle.mesh.y = 0.0f;

        if (CheckCollisionRecs(ball.mesh, right_paddle.mesh)) {
            ball.velocity.x = -ball.velocity.x;
            ball.velocity.y = ball.velocity.y + right_paddle.velocity.y * paddle_strength;
        }
        if (CheckCollisionRecs(ball.mesh, left_paddle.mesh)) {
            ball.velocity.x = -ball.velocity.x;
            ball.velocity.y = ball.velocity.y + left_paddle.velocity.y * paddle_strength;
        }

        if (ball.mesh.x < left_paddle.mesh.x) {
            opponent_score += 1;
            reset(window_width, window_height, paddle_edge_offset, left_paddle, right_paddle, ball);
            ball.velocity = {1.0f, 0.0f};
        }
        if (ball.mesh.x + ball.mesh.width > right_paddle.mesh.x + right_paddle.mesh.width) {
            player_score += 1;
            reset(window_width, window_height, paddle_edge_offset, left_paddle, right_paddle, ball);
            ball.velocity = {-1.0f, 0.0f};
        }
        if (ball.mesh.y < 0.0f || ball.mesh.y + ball.mesh.height > window_height) {
            ball.velocity.y = -ball.velocity.y;
        }

        if (ball.velocity.x > 1.0f) ball.velocity.x = 1.0f;
        if (ball.velocity.x < -1.0f) ball.velocity.x = -1.0f;

        if (ball.velocity.y > 1.0f) ball.velocity.y = 1.0f;
        if (ball.velocity.y < -1.0f) ball.velocity.y = -1.0f;

        if (!game_paused) {
            left_paddle.mesh.y += left_paddle.velocity.y * SPEED * dt;
            if (current_opponent == OpponentType::AI) {
                right_paddle.mesh.y = ball.mesh.y - right_paddle.mesh.height / 2;
            } else {
                right_paddle.mesh.y += right_paddle.velocity.y * SPEED * dt;
            }
            ball.mesh.y += ball.velocity.y * SPEED * dt;
            ball.mesh.x += ball.velocity.x * SPEED * dt;
        }

        BeginDrawing();
        if (!show_settings && !game_paused) {
            ClearBackground(BLACK);
    
            DrawRectangleRec(left_paddle.mesh, WHITE);
            DrawRectangleRec(right_paddle.mesh, WHITE);
            DrawRectangleRec(ball.mesh, WHITE);
            
            const char *player_score_text = TextFormat("%02i", player_score);
            const char *opponent_score_text = TextFormat("%02i", opponent_score);
            
            Vector2 player_score_size = MeasureTextEx(GetFontDefault(), player_score_text, 20, 12);
            Vector2 opponent_score_size = MeasureTextEx(GetFontDefault(), opponent_score_text, 20, 12);
            
            DrawText(player_score_text, (window_width / 3) - (player_score_size.x / 2), 20, 40, WHITE);
            DrawText(opponent_score_text, (window_width - window_width / 3) - (opponent_score_size.x / 2), 20, 40, WHITE);
    
            for (float y = 0; y < window_height; y += (dotted_line.height * 2)) {
                DrawRectangle(dotted_line.x, y, dotted_line.width, dotted_line.height, GRAY);
            }
        } else {
            ClearBackground(RAYWHITE);
            float opponent_settings_font_size = 30.0f;
            float spacing = 30.0f;

            const char* opponent_display = current_opponent == OpponentType::Human ? "Human" : "AI";
            
            Vector2 opponent_settings_size = MeasureTextEx(GetFontDefault(), "Opponent", opponent_settings_font_size, 0);
            Rectangle opponent_select_box = {
                .x = window_width / 2 + spacing,
                .y = window_height / 2,
                .width = opponent_settings_size.x,
                .height = opponent_settings_size.y
            };
            Vector2 current_opponent_size = MeasureTextEx(GetFontDefault(), opponent_display, 20.0f, 0);

            if (CheckCollisionPointRec(GetMousePosition(), opponent_select_box)) {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                    current_opponent = current_opponent == OpponentType::Human ? OpponentType::AI : OpponentType::Human;
                    reset(window_width, window_height, paddle_edge_offset, left_paddle, right_paddle, ball);
                }
            }
            
            DrawRectangleRec(opponent_select_box, BLACK);
            DrawText("Opponent", (window_width / 2) - opponent_settings_size.x - spacing, window_height / 2, opponent_settings_font_size, BLACK);
            DrawText(opponent_display, (opponent_select_box.x + opponent_select_box.width / 2) - current_opponent_size.x / 2, (opponent_select_box.y + opponent_select_box.height / 2) - current_opponent_size.y / 2, 20.0f, RAYWHITE);
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
