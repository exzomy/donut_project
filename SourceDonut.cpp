#include <Windows.h>
#include <cmath>
#include <cstring>

// DISPLAY CONFIGURATION
constexpr int WIDTH = 140;
constexpr int HEIGHT = 44;
constexpr int PIXELS = WIDTH * HEIGHT;

constexpr float TORUS_RADIUS = 2.0f;
constexpr float CAMERA_DIST = 5.0f;

constexpr float PROJ_X = 50.0f;
constexpr float PROJ_Y = 25.0f;

constexpr float STEP_THETA = 0.025f;
constexpr float STEP_PHI = 0.008f;

static constexpr char GRAYSCALE[] = ".,-~:;=!*#$@";

// RENDER BUFFERS
static char  g_Buffer[PIXELS];
static float g_Depth[PIXELS];

static char g_Output[PIXELS + HEIGHT];

// CONSOLE SETUP
HANDLE InitConsole()
{
    HANDLE hOut = CreateConsoleScreenBuffer(
        GENERIC_READ | GENERIC_WRITE,
        0,
        nullptr,
        CONSOLE_TEXTMODE_BUFFER,
        nullptr);

    SetConsoleActiveScreenBuffer(hOut);

    CONSOLE_CURSOR_INFO cursor{};
    cursor.dwSize = 1;
    cursor.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &cursor);

    CONSOLE_FONT_INFOEX font{};
    font.cbSize = sizeof(font);
    font.dwFontSize.X = 8;
    font.dwFontSize.Y = 16;
    font.FontFamily = FF_DONTCARE;
    font.FontWeight = FW_NORMAL;
    wcscpy_s(font.FaceName, L"Consolas");
    SetCurrentConsoleFontEx(hOut, FALSE, &font);

    COORD size;
    size.X = WIDTH + 1;
    size.Y = HEIGHT;
    SetConsoleScreenBufferSize(hOut, size);

    SMALL_RECT rect;
    rect.Left = 0;
    rect.Top = 0;
    rect.Right = WIDTH - 1;
    rect.Bottom = HEIGHT - 1;
    SetConsoleWindowInfo(hOut, TRUE, &rect);

    return hOut;
}

// RENDERING
void DrawTorus(float angleA, float angleB)
{
    memset(g_Buffer, ' ', PIXELS);
    memset(g_Depth, 0, sizeof(g_Depth));

    float sinA = sinf(angleA);
    float cosA = cosf(angleA);
    float sinB = sinf(angleB);
    float cosB = cosf(angleB);

    int shadesCount = sizeof(GRAYSCALE) - 1;

    for (float theta = 0.0f; theta < 6.2831853f; theta += STEP_THETA)
    {
        float sinTheta = sinf(theta);
        float cosTheta = cosf(theta);

        for (float phi = 0.0f; phi < 6.2831853f; phi += STEP_PHI)
        {
            float sinPhi = sinf(phi);
            float cosPhi = cosf(phi);

            float circleX = TORUS_RADIUS + cosTheta;
            float circleY = sinTheta;

            float x = circleX * (cosB * cosPhi + sinA * sinB * sinPhi) - circleY * cosA * sinB;
            float y = circleX * (sinB * cosPhi - sinA * cosB * sinPhi) + circleY * cosA * cosB;
            float z = CAMERA_DIST + cosA * circleX * sinPhi + circleY * sinA;

            float invZ = 1.0f / z;

            int px = (int)(WIDTH * 0.5f + PROJ_X * invZ * x);
            int py = (int)(HEIGHT * 0.5f - PROJ_Y * invZ * y);

            if (px < 0 || px >= WIDTH || py < 0 || py >= HEIGHT)
                continue;

            float light = cosPhi * cosTheta * sinB
                - cosA * cosTheta * sinPhi
                - sinA * sinTheta
                + cosB * (cosA * sinTheta - cosTheta * sinA * sinPhi);

            int idx = px + py * WIDTH;

            if (invZ <= g_Depth[idx])
                continue;

            g_Depth[idx] = invZ;

            int shade = (int)((light + 1.0f) * 0.5f * (shadesCount - 1));
            shade = (shade < 0) ? 0 : (shade >= shadesCount) ? shadesCount - 1 : shade;

            g_Buffer[idx] = GRAYSCALE[shade];
        }
    }
}

// FRAME ASSEMBLY
int AssembleFrame()
{
    int cursor = 0;

    for (int row = 0; row < HEIGHT; ++row)
    {
        memcpy(g_Output + cursor, g_Buffer + row * WIDTH, WIDTH);
        cursor += WIDTH;
        g_Output[cursor++] = '\n';
    }

    return cursor;
}

int main()
{
    HANDLE hOut = InitConsole();
    DWORD written = 0;

    LARGE_INTEGER freq, last;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&last);

    float angleA = 0.0f;
    float angleB = 0.0f;

    while (true)
    {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);

        float delta = (float)(now.QuadPart - last.QuadPart) / (float)freq.QuadPart;
        last = now;

        DrawTorus(angleA, angleB);

        int frameLen = AssembleFrame();

        WriteConsoleOutputCharacterA(hOut, g_Output, frameLen, { 0, 0 }, &written);

        angleA += delta * 1.2f;
        angleB += delta * 0.6f;
    }

    return 0;
}