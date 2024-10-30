// Các bug hiện tại:
/*
- Màu bàn cờ tự thay đổi khi đổi lượt
- Hình tròn O chưa được đẹp
- Hình X còn mỏng
- Hướng dẫn hiện trên terminal + lỗi font chữ
*/
// Cần bổ sung:
/*
- Backrgound
- Sound effects
- Người chơi
- (Hiệu ứng khi di chuyển)
- (Hiệu ứng khi đổi lượt
- (Timer)
- Font chữ
*/




// Khai báo thư viện cần thiết
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#define BOARD_SIZE 12
#define LEFT 50
#define TOP 50

using namespace std;

struct _POINT { int x, y, c; };

_POINT _A[BOARD_SIZE][BOARD_SIZE];
bool _TURN; // true: X, false: O
int _X, _Y;

void DrawBoard(SDL_Renderer* renderer);
void DrawInstructions(SDL_Renderer* renderer);
void ResetData();
void StartGame();
void ExitGame();
int CheckBoard(int pX, int pY);
int ProcessFinish(int pWhoWin);
int TestBoard();
void MoveRight();
void MoveLeft();
void MoveDown();
void MoveUp();

int main(int argc, char* args[]) {
    // Tạo một cửa sổ Window
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        cout << "Can't init SDL: " << SDL_GetError() << endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Game Caro", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    if (!window) {
        cout << "Can't create window: " << SDL_GetError() << endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        cout << "Can't create renderer: " << SDL_GetError() << endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    ResetData();
    StartGame();

    SDL_Event event;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    ExitGame();
                    running = false;
                    break;
                case SDLK_a: MoveLeft(); break;
                case SDLK_w: MoveUp(); break;
                case SDLK_s: MoveDown(); break;
                case SDLK_d: MoveRight(); break;
                case SDLK_RETURN: {
                    int row = (_Y - TOP) / 40;
                    int col = (_X - LEFT) / 40;
                    if (CheckBoard(_X, _Y) == 0) {
                        _A[row][col].c = _TURN ? -1 : 1;

                        int result = TestBoard();
                        if (result != 0) {
                            ProcessFinish(result);
                            StartGame();
                        }
                        else {
                            bool isDraw = true;
                            for (int i = 0; i < BOARD_SIZE; i++) {
                                for (int j = 0; j < BOARD_SIZE; j++) {
                                    if (_A[i][j].c == 0) {
                                        isDraw = false;
                                        break;
                                    }
                                }
                                if (!isDraw) break;
                            }
                            if (isDraw) {
                                ProcessFinish(0);
                                StartGame();
                            }
                            else {
                                _TURN = !_TURN; // Đổi lượt
                            }
                        }
                    }
                    else {
                        cout << "Ô này đã được đánh dấu!" << endl;
                    }
                } break;
                }
            }
        }

        // Màu trắng cho nền(có thể thay đổi sau)
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer); // Clear màn hình

        DrawInstructions(renderer);
        DrawBoard(renderer);
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// Vẽ hình tròn
void DrawCircle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w; // Tính khoảng cách từ tâm
            int dy = radius - h;
            if ((dx * dx + dy * dy) <= (radius * radius)) { // Nếu nằm trong hình tròn
                if ((dx * dx + dy * dy) >= ((radius - 5) * (radius - 5))) { // Tạo viền
                    SDL_RenderDrawPoint(renderer, centerX + dx, centerY + dy);
                }
            }
        }
    }
}

// Vẽ bàn cờ
void DrawBoard(SDL_Renderer* renderer) {
    // Vẽ nền cho bàn cờ
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer); // Xóa màn hình với màu nền

    // Vẽ các ô với màu đen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Màu đen cho viền ô

    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            // Vẽ ô
            SDL_Rect rect = { LEFT + 40 * j, TOP + 40 * i, 40, 40 };
            SDL_RenderDrawRect(renderer, &rect); // Vẽ viền ô

            // Vẽ X và O
            if (_A[i][j].c == -1) {
                // Vẽ X
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Màu đỏ cho X
                SDL_RenderDrawLine(renderer, LEFT + 40 * j + 5, TOP + 40 * i + 5, LEFT + 40 * j + 35, TOP + 40 * i + 35); // Đường chéo từ trái trên đến phải dưới
                SDL_RenderDrawLine(renderer, LEFT + 40 * j + 35, TOP + 40 * i + 5, LEFT + 40 * j + 5, TOP + 40 * i + 35); // Đường chéo từ trái dưới đến phải trên
            }
            else if (_A[i][j].c == 1) {
                // Vẽ O
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Màu xanh cho O
                DrawCircle(renderer, LEFT + 40 * j + 20, TOP + 40 * i + 20, 15); // Vẽ hình tròn cho O
            }
        }
    }

    // Vẽ ô hiện tại
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Màu xanh cho ô hiện tại
    SDL_Rect highlightRect = { _X, _Y, 40, 40 };
    SDL_RenderDrawRect(renderer, &highlightRect); // Vẽ viền ô hiện tại
}

void DrawInstructions(SDL_Renderer* renderer) {
    // Hướng dẫn
    // Dùng SDL_ttf để vẽ font chữ sau
}

void ResetData() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            _A[i][j].x = LEFT + 40 * j;
            _A[i][j].y = TOP + 40 * i;
            _A[i][j].c = 0;
        }
    }
    _TURN = true; // Bắt đầu với người chơi X
    _X = LEFT;
    _Y = TOP;
}

void StartGame() {
    ResetData();
}

void ExitGame() {
    cout << "Cảm ơn bạn đã chơi trò chơi!" << endl;
}

int CheckBoard(int pX, int pY) {
    int row = (pY - TOP) / 40;
    int col = (pX - LEFT) / 40;

    // Kiểm tra chỉ số hợp lệ
    if (row < 0 || row >= BOARD_SIZE || col < 0 || col >= BOARD_SIZE) {
        return -1; // Chỉ số không hợp lệ
    }
    return _A[row][col].c;
}

int ProcessFinish(int pWhoWin) {
    if (pWhoWin == -1) {
        cout << "Người chơi X thắng!" << endl;
    }
    else if (pWhoWin == 1) {
        cout << "Người chơi O thắng!" << endl;
    }
    else if (pWhoWin == 0) {
        cout << "Hòa!" << endl;
    }
    return pWhoWin;
}

int TestBoard() {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (_A[i][j].c != 0) {
                // Kiểm tra hàng ngang
                if (j <= BOARD_SIZE - 5 &&
                    _A[i][j].c == _A[i][j + 1].c &&
                    _A[i][j].c == _A[i][j + 2].c &&
                    _A[i][j].c == _A[i][j + 3].c &&
                    _A[i][j].c == _A[i][j + 4].c)
                    return _A[i][j].c;

                // Kiểm tra cột
                if (i <= BOARD_SIZE - 5 &&
                    _A[i][j].c == _A[i + 1][j].c &&
                    _A[i][j].c == _A[i + 2][j].c &&
                    _A[i][j].c == _A[i + 3][j].c &&
                    _A[i][j].c == _A[i + 4][j].c)
                    return _A[i][j].c;

                // Kiểm tra đường chéo chính
                if (i <= BOARD_SIZE - 5 && j <= BOARD_SIZE - 5 &&
                    _A[i][j].c == _A[i + 1][j + 1].c &&
                    _A[i][j].c == _A[i + 2][j + 2].c &&
                    _A[i][j].c == _A[i + 3][j + 3].c &&
                    _A[i][j].c == _A[i + 4][j + 4].c)
                    return _A[i][j].c;

                // Kiểm tra đường chéo phụ
                if (i >= 4 && j <= BOARD_SIZE - 5 &&
                    _A[i][j].c == _A[i - 1][j + 1].c &&
                    _A[i][j].c == _A[i - 2][j + 2].c &&
                    _A[i][j].c == _A[i - 3][j + 3].c &&
                    _A[i][j].c == _A[i - 4][j + 4].c)
                    return _A[i][j].c;
            }
        }
    }
    return 0; // Chưa có ai thắng
}

void MoveRight() {
    if (_X < LEFT + (BOARD_SIZE - 1) * 40) {
        _X += 40;
    }
}

void MoveLeft() {
    if (_X > LEFT) {
        _X -= 40;
    }
}

void MoveDown() {
    if (_Y < TOP + (BOARD_SIZE - 1) * 40) {
        _Y += 40;
    }
}

void MoveUp() {
    if (_Y > TOP) {
        _Y -= 40;
    }
}
