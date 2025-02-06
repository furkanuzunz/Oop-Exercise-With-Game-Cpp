#include <raylib.h>
#include <iostream>
#include <string.h>

using namespace std;

Color Green = Color{38, 185, 154, 255};
Color Dark_Green = Color{20, 160, 133, 255};
Color Light_Green = Color{129, 204, 184, 255};
Color Yellow = Color{243, 213, 91, 255};

int player_score = 0;
int cpu_score = 0;

class Ball
{
public:
    float x, y;
    int speed_x, speed_y;
    int radius;

    void Draw()
    {
        DrawCircle(x, y, radius, Yellow); // 2 ye böldük cunku tam merkeze yerlestirdik topu.20 de topun yarıcapı.
    }

    void Update() // 3.adimi tamamladik. topu haraket ettirme.
    {
        x += speed_x;
        y += speed_y;
        // alt kısımda iflerle 4 numarayı da tamamladık.
        if (y + radius >= GetScreenHeight() || y - radius <= 0) // yuksekliklere,yani alt üstü kontrol ettik.
        {
            speed_y *= -1; // topun dikeyde hız yonunu tersine cevirdik
        }
        if (x + radius >= GetScreenWidth()) // cpu wins
        {
            cpu_score++;
            ResetBall();
        }
        if (x - radius <= 0) // player wins
        {
            player_score++;
            ResetBall();
        }
    }

    void ResetBall()
    {
        x = GetScreenWidth() / 2;
        y = GetScreenHeight() / 2;

        int speed_choices[2] = {-1, 1};
        speed_x *= speed_choices[GetRandomValue(0, 1)];
        speed_y *= speed_choices[GetRandomValue(0, 1)];
    }
};

class Paddle // 5.adim  olan oyuncunun raketini oynattik.
{
protected:
    void limitMovement()
    {
        if (y <= 0)
        {
            y = 0;
        }
        if (y + height >= GetScreenHeight())
        {
            y = GetScreenHeight() - height;
        }
    }

public:
    float x, y;
    float width, height;
    int speed_y;

    void Draw()
    {
        DrawRectangleRounded(Rectangle{x, y, width, height}, 0.8, 0, WHITE);
    }
    void Update()
    {
        if (IsKeyDown(KEY_UP))
            y = y - speed_y; // kuregi yukarı gönderceksek cikartlarim ki 0,0 orijine yakınlaşsın.
        if (IsKeyDown(KEY_DOWN))
            y = y + speed_y;

        limitMovement();
    }
};

class CpuPaddle : public Paddle // 6.adimimizi da yaptik.
{
    // biz paddle sinifinin update fonksiyonunu override yapmamiz gerekiyor.cunku oyuncunun değil bilgisayarın oynatmasını istiyoruz.
public:
    void Update(int ball_y)
    { // yapacagimiz sey raketin ,topun  merkezi y konumunun, raketin y mkonumundan üstünde mi değil mi onu kontrol etmemiz lazim
        // eğer top yukardaysa raketi kaldırır.değilse aşağı indirir.
        if (y + height / 2 > ball_y)
        {
            y = y - speed_y;
        }
        if (y + height / 2 <= ball_y)
        {
            y = y + speed_y;
        }
        limitMovement();
    }
};
Ball ball;
Paddle playerPaddle;
CpuPaddle cpu;

int main()
{
    cout << "starting the game" << endl;

    const int screen_width = 1280;
    const int screen_height = 800;
    InitWindow(screen_width, screen_height, "My pong game");
    // oyun penceremiz hazır.
    SetTargetFPS(60); // oyunun ne hızla calısacagını belirledik.ASLIND bunun ile while dongumuzu saniyede 60 kez guncellemek istedigimiiz tanimladik

    ball.radius = 20;
    ball.x = screen_width / 2;
    ball.y = screen_height / 2;
    ball.speed_x = 7;
    ball.speed_y = 7;

    playerPaddle.width = 25;
    playerPaddle.height = 120;
    playerPaddle.x = screen_width - playerPaddle.width - 10;
    playerPaddle.y = screen_height / 2 - playerPaddle.height / 2;
    playerPaddle.speed_y = 6;

    cpu.width = 25;
    cpu.height = 120;
    cpu.x = 10;
    cpu.y = screen_height / 2 - cpu.height / 2;
    cpu.speed_y = 6;

    while (WindowShouldClose() == false) // esc tusunu ve carpı ikonunu kontrol eder.basılıysa true dönderir.kosulumuz false ise devam etti.eğer true olursa kapanicak dongu
    {
        BeginDrawing(); // oyun nesnelerini cizebileceğimiz bos bir tuval olusturur.
        // updating
        ball.Update(); // ekrana cizilenleri silmediği icin,ekranda iz bırakılıyor.top ondan cizgi gibi gozuktu en basta
                       // bunun cozumu her oyun dongusunde ekranı temzlemek.
                       // bir digeri ise top ekran dısına cıkıyor.
        playerPaddle.Update();
        cpu.Update(ball.y);

        // raketlerle topun carpısmalarını kontrol edeceğiz.//7.adimi da yaptik.
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{playerPaddle.x, playerPaddle.y, playerPaddle.width, playerPaddle.height}))
        {
            // true donerse carpismamiz var demektir.
            ball.speed_x *= -1;
        }
        if (CheckCollisionCircleRec(Vector2{ball.x, ball.y}, ball.radius, Rectangle{cpu.x, cpu.y, cpu.width, cpu.height}))
        {
            ball.speed_x *= -1;
        }

        // temizleme
        ClearBackground(Dark_Green); // bu sekilde pencere önc temizleniyor ondan sonra tekrar her sey ciziliyor.60 snde bir olacagından bu anlaşılmayacak.
        // drawing
        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, Green);
        DrawCircle(screen_width / 2, screen_height / 2, 150, Light_Green);

        DrawLine(screen_width / 2, 0, screen_width / 2, screen_height, WHITE);

        ball.Draw();
        cpu.Draw();
        playerPaddle.Draw();

        // skor yazdirma;
        DrawText(TextFormat("%i", cpu_score), screen_width / 4 - 20, 20, 80, WHITE);
        DrawText(TextFormat("%i", player_score), 3 * screen_width / 4 - 20, 20, 80, WHITE);

        EndDrawing(); // tuval cizimini sonlandirir
    }
    // 1.adimizi tamamladik
    // 2.adm kurekleri ve toplari cizmek
    // 2.adimi da draw fonksiyonlari ile tamalladik.
    // simdi ise onlari canlandırma var.

    CloseWindow(); // oyun icin her pencereyi actıgımızda kapatabilmemiz de gerekir.
    return 0;
}