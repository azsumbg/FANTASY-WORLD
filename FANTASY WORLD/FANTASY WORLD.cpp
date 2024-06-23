#include "framework.h"
#include "FANTASY WORLD.h"
#include <mmsystem.h>
#include <d2d1.h>
#include <dwrite.h>
#include "ErrH.h"
#include "FCheck.h"
#include "D2BMPLOADER.h"
#include "warengine.h"
#include <vector>
#include <fstream>
#include <chrono>

#pragma comment (lib,"winmm.lib")
#pragma comment (lib,"d2d1.lib")
#pragma comment (lib,"dwrite.lib")
#pragma comment (lib,"errh.lib")
#pragma comment (lib,"fcheck.lib")
#pragma comment (lib,"d2bmploader.lib")
#pragma comment (lib,"warengine.lib")

#define bWinClassName L"FantasyWars"

#define mNew 1001
#define mSpeed 1002
#define mExit 1003
#define mSave 1004
#define mLoad 1005
#define mHoF 1006

#define tmp_file ".\\res\\data\\temp.dat"
#define Ltmp_file L".\\res\\data\\temp.dat"
#define snd_file L".\\res\\snd\\main.dat"
#define help_file L".\\res\\data\\help.dat"
#define record_file L".\\res\\data\\record.dat"
#define save_file L".\\res\\data\\save.dat"

#define first_record 3001
#define record 3002
#define no_record 3003


WNDCLASS bWin = { 0 };
HINSTANCE bIns = nullptr;
HWND bHwnd = nullptr;
HICON Icon = nullptr;
HCURSOR mainCursor = nullptr;
HCURSOR outCursor = nullptr;
POINT cur_pos = { 0 };
HMENU bBar = nullptr;
HMENU bMain = nullptr;
HMENU bStore = nullptr;
HDC PaintDC = { 0 };
PAINTSTRUCT bPaint = { 0 };
MSG bMsg = { 0 };
BOOL bRet = 0;
UINT bTimer = 0;

D2D1_RECT_F b1Rect{ 0, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2Rect{ scr_width / 3, 0, scr_width * 2 / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3Rect{ scr_width - scr_width / 3, 0, scr_width, 50.0f };

D2D1_RECT_F b1TxtRect{ 20.0f, 0, scr_width / 3 - 50.0f, 50.0f };
D2D1_RECT_F b2TxtRect{ scr_width / 3 + 20.0f, 0, scr_width * 2 / 3 - 50.0f, 50.0f };
D2D1_RECT_F b3TxtRect{ scr_width - scr_width / 3 + 20.0f, 0, scr_width, 50.0f };

wchar_t current_player[16] = L"THE KING";

bool pause = false;
bool show_help = false;
bool sound = true;
bool in_client = true;
bool name_set = false;
bool b1Hglt = false;
bool b2Hglt = false;
bool b3Hglt = false;

bool carry_house = false;
bool carry_wall = false;
bool carry_heroe = false;
bool carry_tower = false;
bool base_under_attack = false;
bool win_game = false;

int score = 0;
int wood = 100;
float game_speed = 0.5f;
int mins = 0;
int secs = 0;
int win_percentage = 0;

/////////////////////////////////////////////////////////

ID2D1Factory* iFactory = nullptr;
ID2D1HwndRenderTarget* Draw = nullptr;

ID2D1RadialGradientBrush* butBckg = nullptr;
ID2D1SolidColorBrush* txtBrush = nullptr;
ID2D1SolidColorBrush* txtInactBrush = nullptr;
ID2D1SolidColorBrush* txtHgltBrush = nullptr;
ID2D1SolidColorBrush* FieldBrush = nullptr;

ID2D1SolidColorBrush* LifeBrush = nullptr;
ID2D1SolidColorBrush* HitBrush = nullptr;
ID2D1SolidColorBrush* CritBrush = nullptr;

IDWriteFactory* iWriteFactory = nullptr;
IDWriteTextFormat* nrmTxt = nullptr;
IDWriteTextFormat* midTxt = nullptr;
IDWriteTextFormat* bigTxt = nullptr;

ID2D1Bitmap* bmpSnow = nullptr;
ID2D1Bitmap* bmpSoil = nullptr;
ID2D1Bitmap* bmpHome = nullptr;
ID2D1Bitmap* bmpTownHall = nullptr;
ID2D1Bitmap* bmpTower = nullptr;
ID2D1Bitmap* bmpTree = nullptr;
ID2D1Bitmap* bmpWall = nullptr;
ID2D1Bitmap* bmpFire[16] = { nullptr };

ID2D1Bitmap* bmpHero = nullptr;
ID2D1Bitmap* bmpAxe = nullptr;

ID2D1Bitmap* bmpEvil1 = nullptr;
ID2D1Bitmap* bmpEvil2 = nullptr;
ID2D1Bitmap* bmpEvil3 = nullptr;
ID2D1Bitmap* bmpBear = nullptr;

/////////////////////////////////////////////////////////

struct AXE
{
    dll:: OBJECT Dims;
    dirs dir = dirs::stop;
};


std::vector<dll::Creature> vHeroes;
std::vector<dll::Creature> vEvils;
std::vector<AXE> vAxes;

dll::BUILDING* FieldGrid[14][20];
dll::BUILDING* TownHall = nullptr;

std::vector<dll::BUILDING*> vHomes;
std::vector<dll::BUILDING*> vTrees;
std::vector<dll::BUILDING*> vTowers;
std::vector<dll::BUILDING*> vWalls;
std::vector<dll::BUILDING*> vFires;

/////////////////////////////////////////////////////////

template<typename GARBAGE> bool GarbageCollect(GARBAGE** what)
{
    if ((*what))
    {
        (*what)->Release();
        (*what) = nullptr;
        return true;
    }
    return false;
}
void ErrorLog(LPCWSTR err_ocurred)
{
    std::wofstream err_file(L".\\res\\data\\error.log", std::ios::app);
    err_file << err_ocurred << L" ! Time stamp: " << std::chrono::system_clock::now() << std::endl;
    err_file.close();
}
void ReleaseResources()
{
    GarbageCollect(&iFactory);
    GarbageCollect(&Draw);
    GarbageCollect(&butBckg);
    GarbageCollect(&txtBrush);
    GarbageCollect(&txtHgltBrush);
    GarbageCollect(&txtInactBrush);
    GarbageCollect(&FieldBrush);
    GarbageCollect(&LifeBrush);
    GarbageCollect(&HitBrush);
    GarbageCollect(&CritBrush);

    GarbageCollect(&iWriteFactory);
    GarbageCollect(&nrmTxt);
    GarbageCollect(&midTxt);
    GarbageCollect(&bigTxt);

    GarbageCollect(&bmpAxe);
    GarbageCollect(&bmpBear);
    GarbageCollect(&bmpEvil1);
    GarbageCollect(&bmpEvil2);
    GarbageCollect(&bmpEvil3);
    for (int i = 0; i < 16; i++)GarbageCollect(&bmpFire[i]);
    GarbageCollect(&bmpHero);
    GarbageCollect(&bmpHome);
    GarbageCollect(&bmpSnow);
    GarbageCollect(&bmpSoil);
    GarbageCollect(&bmpTower);
    GarbageCollect(&bmpTownHall);
    GarbageCollect(&bmpTree);
    GarbageCollect(&bmpWall);
}
void ErrExit(int what)
{
    MessageBeep(MB_ICONERROR);
    MessageBox(NULL, ErrHandle(what), L"Критична грешка !", MB_OK | MB_APPLMODAL | MB_ICONERROR);

    ReleaseResources();
    std::remove(tmp_file);
    exit(1);
}
void InitGame()
{
    wcscpy_s(current_player, L"THE KING");
    name_set = false;
    score = 0;
    secs = 0;
    mins = 0;
    wood = 200;
    carry_heroe = false;
    carry_house = false;
    carry_wall = false;
    carry_tower = false;
    base_under_attack = false;
    win_percentage = 0;
    win_game = false;


    vAxes.clear();
    
    if (!vHomes.empty())
        for (int i = 0; i < vHomes.size(); i++)vHomes[i]->Release();
    if (!vTrees.empty())
        for (int i = 0; i < vTrees.size(); i++)vTrees[i]->Release();
    if (!vTowers.empty())
        for (int i = 0; i < vTowers.size(); i++)vTowers[i]->Release();
    if (!vWalls.empty())
        for (int i = 0; i < vWalls.size(); i++)vWalls[i]->Release();
    if (!vFires.empty())
        for (int i = 0; i < vFires.size(); i++)vFires[i]->Release();

    vHomes.clear();
    vTrees.clear();
    vTowers.clear();
    vWalls.clear();
    vFires.clear();

    delete TownHall;
    TownHall = nullptr;

    if (!vHeroes.empty())
        for (int i = 0; i < vHeroes.size(); i++) vHeroes[i]->Release();
    vHeroes.clear();

    if (!vEvils.empty())
        for (int i = 0; i < vEvils.size(); i++) vEvils[i]->Release();
    vEvils.clear();
    
    for (int rows = 0; rows < 14; rows++)
    {
        for (int cols = 0; cols < 20; cols++)
        {
           
            FieldGrid[rows][cols] = dll::BUILDING::TileFactory(buildings::snow_tile, (float)(cols * 50), (float)(rows * 50 + 50));
        
        }
    }

    int first_home_col = (int)(rand() % 15);
    float first_home_x = FieldGrid[9][first_home_col]->x;
    TownHall = dll::BUILDING::TileFactory(buildings::townhall, first_home_x, FieldGrid[9][0]->y);
    
    FieldGrid[8][first_home_col]->type = buildings::soil_tile;
    FieldGrid[9][first_home_col]->type = buildings::soil_tile;
    FieldGrid[10][first_home_col]->type = buildings::soil_tile;
    FieldGrid[11][first_home_col]->type = buildings::soil_tile;
    FieldGrid[12][first_home_col]->type = buildings::soil_tile;

    if (first_home_col >= 2)
    {
        FieldGrid[8][first_home_col - 2]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col - 2]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col - 2]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col - 2]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col - 2]->type = buildings::soil_tile;
        FieldGrid[8][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col - 1]->type = buildings::soil_tile;
    }
    else if (first_home_col > 1)
    {
        FieldGrid[8][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col - 1]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col - 1]->type = buildings::soil_tile;
    }

    if (first_home_col <= 17)
    {
        FieldGrid[8][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[8][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[8][first_home_col + 3]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col + 3]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col + 3]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col + 3]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col + 3]->type = buildings::soil_tile;
    }
    else if (first_home_col <= 18)
    {
        FieldGrid[8][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[8][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col + 2]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col + 2]->type = buildings::soil_tile;
    }
    else if (first_home_col <= 19)
    {
        FieldGrid[8][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[9][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[10][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[11][first_home_col + 1]->type = buildings::soil_tile;
        FieldGrid[12][first_home_col + 1]->type = buildings::soil_tile;
    }

    FieldGrid[13][16]->type = buildings::tower;
    FieldGrid[13][18]->type = buildings::home;
    FieldGrid[13][19]->type = buildings::wall;

    int tree_counter = 0; 
    while (tree_counter < 9)
    {
        int col = rand() % 19;
        int row = tree_counter;

        if (FieldGrid[row][col]->type == buildings::snow_tile)
        {
            vTrees.push_back(dll::BUILDING::TileFactory(buildings::tree, FieldGrid[row][col]->x, FieldGrid[row][col]->y));
            tree_counter++;
        }
    }
}
BOOL CheckRecord()
{
    if (score < 1)return no_record;

    int result = 0;
    CheckFile(record_file, &result);
    if (result == FILE_NOT_EXIST)
    {
        std::wofstream rec(record_file);
        rec << score << std::endl;
        for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
        rec.close();
        return first_record;
    }

    std::wifstream check(record_file);
    check >> result;
    check.close();

    if (score >> result)
    {
        std::wofstream rec(record_file);
        rec << score << std::endl;
        for (int i = 0; i < 16; i++)rec << static_cast<int>(current_player[i]) << std::endl;
        rec.close();
        return record;
    }
    return no_record;
}
void GameOver()
{
    KillTimer(bHwnd, bTimer);
    PlaySound(NULL, NULL, NULL);
    if (win_game)
    {
        score += static_cast<int>(1000 * game_speed);
        PlaySound(L".\\res\\snd\\tada.wav", NULL, SND_ASYNC);

        Draw->BeginDraw();
        Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkBlue));
        if (bigTxt && txtBrush)
            Draw->DrawTextW(L"ПОЛЮСЪТ Е ТВОЙ !", 17, bigTxt, D2D1::RectF(200.0f, scr_height / 2 - 100.0f, 
                scr_width, scr_height), txtBrush);
        Draw->EndDraw();
        Sleep(2000);
    }
    
    wchar_t end_text[35] = L"БАЗАТА Е УНИЩОЖЕНА !";
    int txt_size = 21;

    switch (CheckRecord())
    {
    case no_record:
        PlaySound(L".\\res\\snd\\loose.wav", NULL, SND_ASYNC);
        break;

    case first_record:
        PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(end_text, L"ПЪРВИ РЕКОРД НА ИГРАТА !");
        txt_size = 25;
        break;

    case record:
        PlaySound(L".\\res\\snd\\record.wav", NULL, SND_ASYNC);
        wcscpy_s(end_text, L"НОВ СВЕТОВЕН РЕКОРД НА ИГРАТА !");
        txt_size = 32;
        break;
    }

    Draw->BeginDraw();
    Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkBlue));
    if (bigTxt && txtBrush)
        Draw->DrawTextW(end_text, txt_size, bigTxt, D2D1::RectF(100.0f, scr_height / 2 - 100.0f,
            scr_width, scr_height), txtBrush);
    Draw->EndDraw();
    Sleep(6500);

    bMsg.message = WM_QUIT;
    bMsg.wParam = 0;
}

INT_PTR CALLBACK bDlgProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_INITDIALOG:
        SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)(Icon));
        return true;
        break;

    case WM_CLOSE:
        EndDialog(hwnd, IDCANCEL);
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDCANCEL:
            EndDialog(hwnd, IDCANCEL);
            break;

        case IDOK:
            if (GetDlgItemText(hwnd, IDC_NAME, current_player, 15) < 1)
            {
                if (sound)MessageBeep(MB_ICONASTERISK);
                MessageBox(bHwnd, L"Ха, ха, ха ! Забрави си името !", L"Забраватор !", MB_OK | MB_APPLMODAL | MB_ICONASTERISK);
                wcscpy_s(current_player, L"THE KING");
                EndDialog(hwnd, IDCANCEL);
                break;
            }
            EndDialog(hwnd, IDOK);
        }
        break;

    }

    return (INT_PTR)(FALSE);
}
LRESULT CALLBACK bWinProc(HWND hwnd, UINT ReceivedMsg, WPARAM wParam, LPARAM lParam)
{
    switch (ReceivedMsg)
    {
    case WM_CREATE:
        srand((unsigned int)(time(0)));
        SetTimer(hwnd, bTimer, 1000, 0);

        bBar = CreateMenu();
        bMain = CreateMenu();
        bStore = CreateMenu();

        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bMain), L"Основно меню");
        AppendMenu(bBar, MF_POPUP, (UINT_PTR)(bStore), L"Меню за данни");

        AppendMenu(bMain, MF_STRING, mNew, L"Нова игра");
        AppendMenu(bMain, MF_STRING, mSpeed, L"Турбо режим");
        AppendMenu(bMain, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bMain, MF_STRING, mExit, L"Изход");

        AppendMenu(bStore, MF_STRING, mSave, L"Запази игра");
        AppendMenu(bStore, MF_STRING, mLoad, L"Зареди режим");
        AppendMenu(bStore, MF_SEPARATOR, NULL, NULL);
        AppendMenu(bStore, MF_STRING, mHoF, L"Зала на славата");
        SetMenu(hwnd, bBar);
        InitGame();
        break;

    case WM_CLOSE:
        pause = true;
        if (sound)MessageBeep(MB_ICONEXCLAMATION);
        if (MessageBox(hwnd, L"Ако излезеш, ще загубиш тази игра !\n\nНаистина ли излизаш ?",
            L"Изход !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
        {
            pause = false;
            break;
        }
        GameOver();
        break;

    case WM_PAINT:
        PaintDC = BeginPaint(hwnd, &bPaint);
        FillRect(PaintDC, &bPaint.rcPaint, CreateSolidBrush(RGB(50, 50, 50)));
        EndPaint(hwnd, &bPaint);
        break;

    case WM_TIMER:
        if (pause)break;
        else
        {
            float conquered = 0;
            for (int row = 0; row < 14; row++)
            {
                for (int col = 0; col < 20; col++)
                {
                    if (FieldGrid[row][col]->type == buildings::soil_tile || FieldGrid[row][col]->type == buildings::townhall
                        || FieldGrid[row][col]->type == buildings::home || FieldGrid[row][col]->type == buildings::tower
                        || FieldGrid[row][col]->type == buildings::wall)
                        conquered++;
                }
            }
            win_percentage = (int)(conquered / 280 * 100);
            secs++;
            mins = (int)(floor(secs / 60));
        }
        break;

    case WM_SETCURSOR:
        GetCursorPos(&cur_pos);
        ScreenToClient(hwnd, &cur_pos);
        if (LOWORD(lParam) == HTCLIENT)
        {
            if (!in_client)
            {
                in_client = true;
                pause = false;
            }

            if (cur_pos.y <= 50)
            {
                if (cur_pos.x >= b1TxtRect.left && cur_pos.x <= b1TxtRect.right)
                {
                    if (!b1Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b1Hglt = true;
                    }
                    b2Hglt = false;
                    b3Hglt = false;
                }
                if (cur_pos.x >= b2TxtRect.left && cur_pos.x <= b2TxtRect.right)
                {
                    if (!b2Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b2Hglt = true;
                    }
                    b1Hglt = false;
                    b3Hglt = false;
                }
                if (cur_pos.x >= b3TxtRect.left && cur_pos.x <= b3TxtRect.right)
                {
                    if (!b3Hglt)
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                        b3Hglt = true;
                    }
                    b1Hglt = false;
                    b2Hglt = false;
                }
                SetCursor(outCursor);
                return true;
            }
            else
            {
                if (b1Hglt || b2Hglt || b3Hglt)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                    b1Hglt = false;
                    b2Hglt = false;
                    b3Hglt = false;
                }
                SetCursor(mainCursor);
                return true;
            }
        }
        else
        {
            if (in_client)
            {
                in_client = false;
                pause = true;
            }

            if (b1Hglt || b2Hglt || b3Hglt)
            {
                if (sound)mciSendString(L"play .\\res\\snd\\click.wav", NULL, NULL, NULL);
                b1Hglt = false;
                b2Hglt = false;
                b3Hglt = false;
            }

            SetCursor(LoadCursor(NULL, IDC_ARROW));
            return true;
        }
        break;

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case mNew:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Ако рестартираш, ще загубиш тази игра !\n\nНаистина ли рестартираш ?",
                L"Рестарт !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            InitGame();
            break;

        case mSpeed:
            pause = true;
            if (sound)MessageBeep(MB_ICONEXCLAMATION);
            if (MessageBox(hwnd, L"Наистина ли да включа турбото ?",
                L"Турбо режим !", MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION) == IDNO)
            {
                pause = false;
                break;
            }
            game_speed++;
            break;

        case mExit:
            SendMessage(hwnd, WM_CLOSE, NULL, NULL);
            break;







        }
        break;

    case WM_LBUTTONDOWN:
        if (HIWORD(lParam) <= 50)
        {
            if (LOWORD(lParam) >= b1TxtRect.left && LOWORD(lParam) <= b1TxtRect.right)
            {
                if (name_set)
                {
                    if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                    break;
                }
                if (sound)mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                if (DialogBox(bIns, MAKEINTRESOURCE(IDD_PLAYER), hwnd, &bDlgProc) == IDOK)name_set = true;
                break;
            }
            if (LOWORD(lParam) >= b2TxtRect.left && LOWORD(lParam) <= b2TxtRect.right)
            {
                mciSendString(L"play .\\res\\snd\\select.wav", NULL, NULL, NULL);
                if (sound)
                {
                    sound = false;
                    PlaySound(NULL, NULL, NULL);
                    break;
                }
                else
                {
                    sound = true;
                    PlaySound(snd_file, NULL, SND_ASYNC | SND_LOOP);
                    break;

                }
                break;
            }


        }
        else
        {
            if (!carry_heroe && !carry_house && !carry_wall && !carry_tower)
            {
                if (wood > 0)
                {
                    int row = HIWORD(lParam) / 50;
                    int col = LOWORD(lParam) / 50;

                    if (row == 13 && col == 17)
                    {
                        if (wood < 25)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            break;
                        }
                        carry_heroe = true;
                        wood -= 25;
                        break;
                    }
                    else if (row == 13 && col == 18)
                    {
                        if (wood < 100)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            break;
                        }
                        wood -= 100;
                        carry_house = true;
                        break;
                    }
                    else if (row == 13 && col == 19)
                    {
                        if (wood < 50)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            break;
                        }
                        carry_wall = true;
                        wood -= 50;
                        break;
                    }
                    else if (row == 13 && col == 16)
                    {
                        if (wood < 80)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            break;
                        }
                        carry_tower = true;
                        wood -= 80;
                        break;
                    }
                }
                else if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
            }
            else
            {
                int row = HIWORD(lParam) / 50;
                int col = LOWORD(lParam) / 50;

                if (row == 13 && (carry_heroe || carry_house || carry_tower || carry_wall))
                {
                    if (carry_heroe)wood += 25;
                    if (carry_house)wood += 100;
                    if (carry_wall)wood += 50;
                    if (carry_tower)wood += 80;

                    carry_heroe = false;
                    carry_house = false;
                    carry_tower = false;
                    carry_wall = false;
                    break;
                }

                if (carry_heroe && TownHall)
                {
                    vHeroes.push_back(dll::CreatureFactory(creatures::hero, TownHall->x + (float)(rand() % 50),
                        (float)(TownHall->y - rand() % 20)));
                    carry_heroe = false;
                }
                if (carry_house)
                {
                    if (row < 13 && col < 20)
                    {
                        if (FieldGrid[row][col]->type == buildings::soil_tile)
                        {
                            vHomes.push_back(dll::BUILDING::TileFactory(buildings::home, FieldGrid[row][col]->x,
                                FieldGrid[row][col]->y));
                            carry_house = false;

                            bool up_ok = (row - 1 >= 0);
                            bool down_ok = (row + 1 <= 12);
                            bool left_ok = (col - 1 >= 0);
                            bool right_ok = (col + 1 <= 19);

                            if (up_ok)
                            {
                                FieldGrid[row - 1][col]->type = buildings::soil_tile;
                                if (left_ok)FieldGrid[row - 1][col - 1]->type = buildings::soil_tile;
                                if (right_ok)FieldGrid[row - 1][col + 1]->type = buildings::soil_tile;
                            }
                            if (down_ok)
                            {
                                FieldGrid[row + 1][col]->type = buildings::soil_tile;
                                if (left_ok)FieldGrid[row + 1][col - 1]->type = buildings::soil_tile;
                                if (right_ok)FieldGrid[row + 1][col + 1]->type = buildings::soil_tile;
                            }
                            if (left_ok)FieldGrid[row][col - 1]->type = buildings::soil_tile;
                            if (right_ok)FieldGrid[row][col + 1]->type = buildings::soil_tile;
                        }
                        else
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            carry_house = false;
                            wood += 100;
                        }
                    }
                }
                if (carry_wall)
                {
                    if (row < 13 && col < 20)
                    {
                        if (FieldGrid[row][col]->type == buildings::soil_tile)
                        {
                            vWalls.push_back(dll::BUILDING::TileFactory(buildings::wall, FieldGrid[row][col]->x,
                                FieldGrid[row][col]->y));
                            carry_wall = false;
                        }
                        else
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            carry_wall = false;
                            wood += 50;
                        }
                    }
                }
                if (carry_tower)
                {
                    if (row < 13 && col < 20)
                    {
                        if (FieldGrid[row][col]->type == buildings::soil_tile)
                        {
                            vTowers.push_back(dll::BUILDING::TileFactory(buildings::tower, FieldGrid[row][col]->x,
                                FieldGrid[row][col]->y));
                            carry_tower = false;
                        }
                        else
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\negative.wav", NULL, NULL, NULL);
                            carry_tower = false;
                            wood += 80;
                        }
                    }
                }
            }
        }
        break;

    default: return DefWindowProc(hwnd, ReceivedMsg, wParam, lParam);
    }

    return (LRESULT)FALSE;
}

void CreateResourses()
{
    int result = 0;
    CheckFile(Ltmp_file, &result);
    if (result == FILE_EXIST) ErrExit(eStarted);
    else
    {
        std::wofstream start(Ltmp_file);
        start << L"Game started at: " << std::chrono::system_clock::now();
        start.close();
    }

    int start_x = GetSystemMetrics(SM_CXSCREEN) / 2 - (int)(scr_width / 2);
    if (start_x + (int)(scr_width) > GetSystemMetrics(SM_CXSCREEN) || GetSystemMetrics(SM_CYSCREEN) < (int)scr_height + 50)
        ErrExit(eScreen);

    Icon = (HICON)(LoadImage(NULL, L".\\res\\main.ico", IMAGE_ICON, 255, 108, LR_LOADFROMFILE));
    if (!Icon)ErrExit(eIcon);
    mainCursor = LoadCursorFromFile(L".\\res\\main.ani");
    outCursor = LoadCursorFromFile(L".\\res\\out.ani");
    if (!mainCursor || !outCursor)ErrExit(eCursor);

    bWin.lpszClassName = bWinClassName;
    bWin.hInstance = bIns;
    bWin.lpfnWndProc = &bWinProc;
    bWin.hbrBackground = CreateSolidBrush(RGB(50, 50, 50));
    bWin.hIcon = Icon;
    bWin.hCursor = mainCursor;
    bWin.style = CS_DROPSHADOW;

    if (!RegisterClass(&bWin))ErrExit(eClass);

    bHwnd = CreateWindowW(bWinClassName, L"ЗИМЕН СВЯТ !", WS_CAPTION | WS_SYSMENU, start_x, 50, (int)(scr_width),
        (int)(scr_height), NULL, NULL, bIns, NULL);
    if (!bHwnd)ErrExit(eWindow);
    else ShowWindow(bHwnd, SW_SHOWDEFAULT);
    /////////////////////////////////////////////////////////////////////


    //D2D1 FACTORY ********************************************

    HRESULT hr = S_OK;

    hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &iFactory);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating D2D1Factory");
        ErrExit(eD2D);
    }

    if (iFactory && bHwnd)
        hr = iFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(bHwnd,
            D2D1::SizeU((UINT32)(scr_width), (UINT32)(scr_height))), &Draw);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating D2D1HwndRenderTarget");
        ErrExit(eD2D);
    }

    D2D1_GRADIENT_STOP gStop[2] = { 0 };
    ID2D1GradientStopCollection* gCol = nullptr;

    gStop[0].position = 0;
    gStop[0].color = D2D1::ColorF(D2D1::ColorF::DarkBlue);
    gStop[1].position = 1.0f;
    gStop[1].color = D2D1::ColorF(D2D1::ColorF::DarkSlateGray);

    if (Draw)
        hr = Draw->CreateGradientStopCollection(gStop, 2, &gCol);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating D2D1GradientStopCollection");
        ErrExit(eD2D);
    }
    
    if (Draw && gCol)
        hr = Draw->CreateRadialGradientBrush(D2D1::RadialGradientBrushProperties(D2D1::Point2F(scr_width / 2, 25.0f),
            D2D1::Point2F(0, 0), scr_width / 2, 25.0f), gCol, &butBckg);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating butBckg");
        ErrExit(eD2D);
    }
    GarbageCollect(&gCol);

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::LawnGreen), &txtBrush);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating txtBrush");
        ErrExit(eD2D);
    }
    
    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::YellowGreen), &txtHgltBrush);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating txtHgltBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::IndianRed), &txtInactBrush);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating txtInactBrush");
        ErrExit(eD2D);
    }
    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DimGray), &FieldBrush);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating txtInactBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Green), &LifeBrush);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating LifeBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Orange), &HitBrush);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating HitBrush");
        ErrExit(eD2D);
    }

    if (Draw)
        hr = Draw->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &CritBrush);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating CritBrush");
        ErrExit(eD2D);
    }

    //WRITE FACTORY ******************************

    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&iWriteFactory));
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating iWriteFactory");
        ErrExit(eD2D);
    }

    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 20.0f, L"", &nrmTxt);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating nrmTxt");
        ErrExit(eD2D);
    }
    
    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 36.0f, L"", &midTxt);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating midTxt");
        ErrExit(eD2D);
    }

    if (iWriteFactory)
        hr = iWriteFactory->CreateTextFormat(L"GABRIOLA", NULL, DWRITE_FONT_WEIGHT_EXTRA_BLACK, DWRITE_FONT_STYLE_OBLIQUE,
            DWRITE_FONT_STRETCH_NORMAL, 64.0f, L"", &bigTxt);
    if (hr != S_OK)
    {
        ErrorLog(L"Error creating bigTxt");
        ErrExit(eD2D);
    }

    ////////////////////////////////////////////////

    if (Draw)
    {
        bmpBear = Load(L".\\res\\img\\evils\\bear.png", Draw);
        if (!bmpBear)
        {
            ErrorLog(L"Error Loading bmpBear");
            ErrExit(eD2D);
        }
        
        bmpEvil1 = Load(L".\\res\\img\\evils\\evil1.png", Draw);
        if (!bmpEvil1)
        {
            ErrorLog(L"Error Loading bmpEvil1");
            ErrExit(eD2D);
        }

        bmpEvil2 = Load(L".\\res\\img\\evils\\evil2.png", Draw);
        if (!bmpEvil2)
        {
            ErrorLog(L"Error Loading bmpEvil2");
            ErrExit(eD2D);
        }

        bmpEvil3 = Load(L".\\res\\img\\evils\\evil3.png", Draw);
        if (!bmpEvil3)
        {
            ErrorLog(L"Error Loading bmpEvil3");
            ErrExit(eD2D);
        }

        bmpHero = Load(L".\\res\\img\\hero\\hero.png", Draw);
        if (!bmpHero)
        {
            ErrorLog(L"Error Loading bmpHero");
            ErrExit(eD2D);
        }

        bmpAxe = Load(L".\\res\\img\\hero\\axe.png", Draw);
        if (!bmpAxe)
        {
            ErrorLog(L"Error Loading bmpAxe");
            ErrExit(eD2D);
        }

        bmpSnow = Load(L".\\res\\img\\field\\snow.png", Draw);
        if (!bmpSnow)
        {
            ErrorLog(L"Error Loading bmpSnow");
            ErrExit(eD2D);
        }

        bmpSoil = Load(L".\\res\\img\\field\\soil.png", Draw);
        if (!bmpSoil)
        {
            ErrorLog(L"Error Loading bmpSoil");
            ErrExit(eD2D);
        }

        bmpHome = Load(L".\\res\\img\\field\\home.png", Draw);
        if (!bmpHome)
        {
            ErrorLog(L"Error Loading bmpHome");
            ErrExit(eD2D);
        }

        bmpTownHall = Load(L".\\res\\img\\field\\townhall.png", Draw);
        if (!bmpTownHall)
        {
            ErrorLog(L"Error Loading bmpTownHall");
            ErrExit(eD2D);
        }

        bmpTree = Load(L".\\res\\img\\field\\Tree.png", Draw);
        if (!bmpTree)
        {
            ErrorLog(L"Error Loading bmpTree");
            ErrExit(eD2D);
        }

        bmpWall = Load(L".\\res\\img\\field\\wall.png", Draw);
        if (!bmpWall)
        {
            ErrorLog(L"Error Loading bmpWall");
            ErrExit(eD2D);
        }

        bmpTower = Load(L".\\res\\img\\field\\tower.png", Draw);
        if (!bmpTower)
        {
            ErrorLog(L"Error Loading bmpTower");
            ErrExit(eD2D);
        }

        for (int i = 0; i < 16; i++)
        {
            wchar_t name[100] = L".\\res\\img\\field\\fire\\";
            wchar_t add[5] = L"\0";
            wsprintf(add, L"%d", i);
            wcscat_s(name, add);
            wcscat_s(name, L".png");

            bmpFire[i] = Load(name, Draw);
            if (!bmpFire[i])
            {
                ErrorLog(L"Error Loading bmpFire");
                ErrExit(eD2D);
            }
        }
    }

    D2D1_RECT_F TopScreen{ scr_width / 2 - 300.0f, -50.0f, scr_width, 50.0f };
    D2D1_RECT_F BottomScreen{ scr_width / 2 - 200.0f, scr_height + 50.0f, scr_width, scr_height - 50.0f };

    bool top_ok = false;
    bool bottom_ok = false;

    mciSendString(L"play .\\res\\snd\\intro.wav", NULL, NULL, NULL);

    while (!top_ok || !bottom_ok)
    {
        if (!top_ok)
        {
            TopScreen.top += 2.5f;
            TopScreen.bottom += 2.5f;
            if (TopScreen.bottom >= scr_height / 2 - 100.0f)top_ok = true;
        }
        if (!bottom_ok)
        {
            BottomScreen.top -= 2.5f;
            BottomScreen.bottom -= 2.5f;
            if (BottomScreen.top <= scr_height / 2 + 100.0f)bottom_ok = true;
        }

        if (Draw && bigTxt && txtBrush)
        {
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));
            Draw->DrawTextW(L"ЗИМНИ ВОЙНИ !", 14, bigTxt, TopScreen, txtBrush);
            Draw->DrawTextW(L"dev. Daniel", 12, bigTxt, BottomScreen, txtBrush);
            Draw->EndDraw();
        }
    }
    Sleep(1500);
}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    bIns = hInstance;
    if (!bIns)ErrExit(eClass);
    CreateResourses();

    while (bMsg.message != WM_QUIT)
    {
        if ((bRet = PeekMessage(&bMsg, bHwnd, NULL, NULL, PM_REMOVE)) != 0)
        {
            if (bRet == -1)ErrExit(eMsg);
            TranslateMessage(&bMsg);
            DispatchMessage(&bMsg);
        }

        if (pause)
        {
            if (show_help)continue;
            Draw->BeginDraw();
            Draw->Clear(D2D1::ColorF(D2D1::ColorF::DarkSlateGray));
            Draw->DrawTextW(L"ПАУЗА !", 8, bigTxt, D2D1::RectF(scr_width / 2 - 100.0f, scr_height / 2 - 50.0f, scr_width, 
                scr_height), txtBrush);
            Draw->EndDraw();
            continue;
        }
        ///////////////////////////////////////////////////

        // PEOPLE *****************************************

        if (!vHeroes.empty())
        {
            for (std::vector<dll::Creature>::iterator it = vHeroes.begin(); it < vHeroes.end(); it++)
            {
                AI_INPUT now;
                
                now.base_under_attack = base_under_attack;
                now.current_action = (*it)->AIDataIN.current_action;
                
                now.tree_in_range = false;
                now.near_tree_x = -1;
                now.near_tree_y = -1;
                
                now.near_enemy_lifes = -1;
                now.near_enemy_x = -1;
                now.near_enemy_y = -1;

                now.obst_down = (*it)->AIDataIN.obst_down;
                now.obst_up = (*it)->AIDataIN.obst_up;
                now.obst_left = (*it)->AIDataIN.obst_left;
                now.obst_right = (*it)->AIDataIN.obst_right;
                
                if (TownHall)
                {
                    now.shelter.left = static_cast<LONG>(TownHall->x);
                    now.shelter.right = static_cast<LONG>(TownHall->ex);
                    now.shelter.top = static_cast<LONG>(TownHall->y);
                    now.shelter.bottom = static_cast<LONG>(TownHall->ey);
                }

                if (vEvils.empty()) now.exist_enemy = false;
                else
                {
                    for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); ++evil)
                    {
                        if ((*it)->AIDataIN.current_action != actions::shoot)
                        {
                            if ((float)(abs((*it)->x - (*evil)->x)) < (float)(abs((*it)->x - now.near_enemy_x)) ||
                                (float)(abs((*it)->y - (*evil)->y)) < (float)(abs((*it)->y - now.near_enemy_y)))
                            {
                                now.near_enemy_lifes = (*evil)->lifes;
                                now.near_enemy_x = (*evil)->x;
                                now.near_enemy_y = (*evil)->y;
                            }
                            
                        }
                    }
                }
                if (vTrees.empty()) now.exist_tree = false;
                else
                {
                    if (!now.tree_in_range)
                    {
                        for (std::vector<dll::BUILDING*>::iterator tree = vTrees.begin(); tree < vTrees.end(); ++tree)
                        {
                            if ((float)(abs((*it)->x - (*tree)->x)) < (float)(abs((*it)->x - now.near_enemy_x)) 
                                || (float)(abs((*it)->y - (*tree)->y)) < (float)(abs((*it)->y - now.near_enemy_y)))
                            {
                                now.near_tree_x = (*tree)->x;
                                now.near_tree_y = (*tree)->y;
                                if ((float)(abs((*it)->x - now.near_tree_x) < 10.0f) 
                                    && (float)(abs((*it)->y - now.near_tree_y) < 10.0f))
                                {
                                    now.tree_in_range = true;
                                    now.chopped_tree_number = (int)(std::distance(vTrees.begin(), tree));
                                }
                                else
                                {
                                    now.tree_in_range = false;
                                    now.chopped_tree_number = -1;
                                }
                            }
                        }
                    }
                    else now.chopped_tree_number = (*it)->AIDataIN.chopped_tree_number;
                    
                }

                (*it)->AIManager(now);
                (*it)->AIDataIN.current_action = (*it)->AIDataOut.new_action;
                 
                if ((*it)->AIDataIN.obst_left || (*it)->AIDataIN.obst_right || (*it)->AIDataIN.obst_up 
                    || (*it)->AIDataIN.obst_down)
                {
                    (*it)->AIDataIN.obst_left = false;
                    (*it)->AIDataIN.obst_right = false;
                    (*it)->AIDataIN.obst_up = false;
                    (*it)->AIDataIN.obst_down = false;
                }
            }
        }

        if (!vHeroes.empty())
        {
            for (std::vector<dll::Creature>::iterator it = vHeroes.begin(); it < vHeroes.end(); it++)
            {
                
                switch ((*it)->AIDataIN.current_action)
                {
                case actions::chop:
                    if ((*it)->Chop())
                    {
                        if (sound)mciSendString(L"play .\\res\\snd\\cut.wav", NULL, NULL, NULL);
                        wood += 10;
                        vTrees[(*it)->AIDataIN.chopped_tree_number]->lifes -= 5;
                        
                        if (vTrees[(*it)->AIDataIN.chopped_tree_number]->lifes < 0)
                        {
                            vTrees[(*it)->AIDataIN.chopped_tree_number]->Release();
                            vTrees.erase(vTrees.begin() + (*it)->AIDataIN.chopped_tree_number);
                            if (vTrees.empty())(*it)->AIDataIN.exist_tree = false;
                            if (!vHeroes.empty())
                                for (int i = 0; i < vHeroes.size(); i++)
                                    vHeroes[i]->AIDataIN.tree_in_range = false;
                        }
                    }
                    break;

                case actions::move:
                    if ((*it)->Move(game_speed, (*it)->AIDataOut.new_x, (*it)->AIDataOut.new_y) == DLL_FAIL)
                    {
                        switch ((*it)->dir)
                        {
                        case dirs::up:
                            (*it)->AIDataIN.obst_up = true;
                            break;

                        case dirs::u_r:
                            (*it)->AIDataIN.obst_up = true;
                            (*it)->AIDataIN.obst_right = true;
                            break;

                        case dirs::u_l:
                            (*it)->AIDataIN.obst_up = true;
                            (*it)->AIDataIN.obst_left = true;
                            break;

                        case dirs::down:
                            (*it)->AIDataIN.obst_down = true;
                            break;

                        case dirs::d_r:
                            (*it)->AIDataIN.obst_down = true;
                            (*it)->AIDataIN.obst_right = true;
                            break;

                        case dirs::d_l:
                            (*it)->AIDataIN.obst_down = true;
                            (*it)->AIDataIN.obst_left = true;
                            break;
                       
                        case dirs::left:
                            (*it)->AIDataIN.obst_left = true;
                            break;

                        case dirs::right:
                            (*it)->AIDataIN.obst_right = true;
                            break;
                        }
                    }
                    break;

                case actions::shelter:
                    if ((*it)->Move(game_speed, (*it)->AIDataOut.new_x, (*it)->AIDataOut.new_y) == DLL_FAIL)
                    {
                        switch ((*it)->dir)
                        {
                        case dirs::up:
                            (*it)->AIDataIN.obst_up = true;
                            break;

                        case dirs::u_r:
                            (*it)->AIDataIN.obst_up = true;
                            (*it)->AIDataIN.obst_right = true;
                            break;

                        case dirs::u_l:
                            (*it)->AIDataIN.obst_up = true;
                            (*it)->AIDataIN.obst_left = true;
                            break;

                        case dirs::down:
                            (*it)->AIDataIN.obst_down = true;
                            break;

                        case dirs::d_r:
                            (*it)->AIDataIN.obst_down = true;
                            (*it)->AIDataIN.obst_right = true;
                            break;

                        case dirs::d_l:
                            (*it)->AIDataIN.obst_down = true;
                            (*it)->AIDataIN.obst_left = true;
                            break;

                        case dirs::left:
                            (*it)->AIDataIN.obst_left = true;
                            break;

                        case dirs::right:
                            (*it)->AIDataIN.obst_right = true;
                            break;
                        }
                    }
                    break;
                }
                
            }
        }
 
        //EVILS

        if (vEvils.size() <= 2 + game_speed && rand() % 900 == 66)
            vEvils.push_back(dll::CreatureFactory(static_cast<creatures>(rand() % 4), (float)(rand() % 1000 + 100), 0));

        if (!vEvils.empty())
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                if (!vWalls.empty())
                {
                    (*evil)->AIDataIN.exist_enemy = true;
                    if (!vHeroes.empty())(*evil)->AIDataIN.near_enemy_x = (*vHeroes.begin())->x;
                    if (!vHeroes.empty())(*evil)->AIDataIN.near_enemy_y = (*vHeroes.begin())->y;
                    if (!vHeroes.empty())(*evil)->AIDataIN.near_enemy_lifes = (*vHeroes.begin())->lifes;

                    for (std::vector<dll::BUILDING*>::iterator hero = std::next(vWalls.begin()); hero < vWalls.end(); hero++)
                    {
                        if (abs((*evil)->x - (*hero)->x) < abs((*evil)->AIDataIN.near_enemy_x ||
                            abs((*evil)->y - (*hero)->y) < abs((*evil)->AIDataIN.near_enemy_y)))
                        {
                            (*evil)->AIDataIN.near_enemy_x = (*hero)->x;
                            (*evil)->AIDataIN.near_enemy_y = (*hero)->y;
                            (*evil)->AIDataIN.near_enemy_lifes = (*hero)->lifes;
                        }

                    }
                }
                else (*evil)->AIDataIN.exist_enemy = false;

                if (!vHomes.empty())
                {
                    (*evil)->AIDataIN.exist_enemy = true;
                    if (!vHeroes.empty())(*evil)->AIDataIN.near_enemy_x = (*vHeroes.begin())->x;
                    if (!vHeroes.empty())(*evil)->AIDataIN.near_enemy_y = (*vHeroes.begin())->y;
                    if (!vHeroes.empty())(*evil)->AIDataIN.near_enemy_lifes = (*vHeroes.begin())->lifes;

                    for (std::vector<dll::BUILDING*>::iterator hero = std::next(vHomes.begin()); hero < vHomes.end(); hero++)
                    {
                        if (abs((*evil)->x - (*hero)->x) < abs((*evil)->AIDataIN.near_enemy_x ||
                            abs((*evil)->y - (*hero)->y) < abs((*evil)->AIDataIN.near_enemy_y)))
                        {
                            (*evil)->AIDataIN.near_enemy_x = (*hero)->x;
                            (*evil)->AIDataIN.near_enemy_y = (*hero)->y;
                            (*evil)->AIDataIN.near_enemy_lifes = (*hero)->lifes;
                        }

                    }
                }
                else (*evil)->AIDataIN.exist_enemy = false;
                
                if (!vHeroes.empty())
                {
                    (*evil)->AIDataIN.exist_enemy = true;
                    (*evil)->AIDataIN.near_enemy_x = (*vHeroes.begin())->x;
                    (*evil)->AIDataIN.near_enemy_y = (*vHeroes.begin())->y;
                    (*evil)->AIDataIN.near_enemy_lifes = (*vHeroes.begin())->lifes;

                    for (std::vector<dll::Creature>::iterator hero = std::next(vHeroes.begin()); hero < vHeroes.end(); hero++)
                    {
                        if (abs((*evil)->x - (*hero)->x) < abs((*evil)->AIDataIN.near_enemy_x ||
                            abs((*evil)->y - (*hero)->y) < abs((*evil)->AIDataIN.near_enemy_y)))
                        {
                            (*evil)->AIDataIN.near_enemy_x = (*hero)->x;
                            (*evil)->AIDataIN.near_enemy_y = (*hero)->y;
                            (*evil)->AIDataIN.near_enemy_lifes = (*hero)->lifes;
                        }
                
                    }
                }
                else (*evil)->AIDataIN.exist_enemy = false;

                if (TownHall)
                {
                    (*evil)->AIDataIN.shelter.left = (LONG)(TownHall->x);
                    (*evil)->AIDataIN.shelter.right = (LONG)(TownHall->ex);
                    (*evil)->AIDataIN.shelter.top = (LONG)(TownHall->y);
                    (*evil)->AIDataIN.shelter.bottom = (LONG)(TownHall->ey);
                }

                (*evil)->AIManager((*evil)->AIDataIN);
                (*evil)->AIDataIN.current_action = (*evil)->AIDataOut.new_action;

                if ((*evil)->AIDataIN.obst_left || (*evil)->AIDataIN.obst_right || (*evil)->AIDataIN.obst_up
                    || (*evil)->AIDataIN.obst_down)
                {
                    (*evil)->AIDataIN.obst_left = false;
                    (*evil)->AIDataIN.obst_right = false;
                    (*evil)->AIDataIN.obst_up = false;
                    (*evil)->AIDataIN.obst_down = false;
                }
            }
        }

        if (!vEvils.empty())
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                switch ((*evil)->AIDataOut.new_action)
                {
                    case actions::move:
                        if ((*evil)->Move(game_speed, (*evil)->AIDataOut.new_x, (*evil)->AIDataOut.new_y) == DLL_FAIL)
                        {
                            switch ((*evil)->dir)
                            {
                            case dirs::up:
                                (*evil)->AIDataIN.obst_up = true;
                                break;

                            case dirs::u_r:
                                (*evil)->AIDataIN.obst_up = true;
                                (*evil)->AIDataIN.obst_right = true;
                                break;

                            case dirs::u_l:
                                (*evil)->AIDataIN.obst_up = true;
                                (*evil)->AIDataIN.obst_left = true;
                                break;

                            case dirs::down:
                                (*evil)->AIDataIN.obst_down = true;
                                break;

                            case dirs::d_r:
                                (*evil)->AIDataIN.obst_down = true;
                                (*evil)->AIDataIN.obst_right = true;
                                break;

                            case dirs::d_l:
                                (*evil)->AIDataIN.obst_down = true;
                                (*evil)->AIDataIN.obst_left = true;
                                break;

                            case dirs::left:
                                (*evil)->AIDataIN.obst_left = true;
                                break;

                            case dirs::right:
                                (*evil)->AIDataIN.obst_right = true;
                                break;
                            }
                        }
                        break;

                    case actions::shoot:

                        break;
                }
            }
        }

        if (!vEvils.empty() && !vTrees.empty())
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                for (std::vector<dll::BUILDING*>::iterator tree = vTrees.begin(); tree < vTrees.end(); tree++)
                {
                    if (!((*evil)->x >= (*tree)->ex || (*evil)->ex <= (*tree)->x ||
                        (*evil)->y >= (*tree)->ey || (*evil)->ey <= (*tree)->y))
                    {
                        switch ((*evil)->dir)
                        {
                        case dirs::up:
                            (*evil)->AIDataIN.obst_up = true;
                            break;

                        case dirs::u_r:
                            (*evil)->AIDataIN.obst_up = true;
                            (*evil)->AIDataIN.obst_right = true;
                            break;

                        case dirs::u_l:
                            (*evil)->AIDataIN.obst_up = true;
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::down:
                            (*evil)->AIDataIN.obst_down = true;
                            break;

                        case dirs::d_r:
                            (*evil)->AIDataIN.obst_down = true;
                            (*evil)->AIDataIN.obst_right = true;
                            break;

                        case dirs::d_l:
                            (*evil)->AIDataIN.obst_down = true;
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::left:
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::right:
                            (*evil)->AIDataIN.obst_right = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!vEvils.empty() && !vWalls.empty())
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                for (std::vector<dll::BUILDING*>::iterator wall = vWalls.begin(); wall < vWalls.end(); wall++)
                {
                    if (!((*evil)->x >= (*wall)->ex || (*evil)->ex <= (*wall)->x ||
                        (*evil)->y >= (*wall)->ey || (*evil)->ey <= (*wall)->y))
                    {
                        switch ((*evil)->dir)
                        {
                        case dirs::up:
                            (*evil)->AIDataIN.obst_up = true;
                            break;

                        case dirs::u_r:
                            (*evil)->AIDataIN.obst_up = true;
                            (*evil)->AIDataIN.obst_right = true;
                            break;

                        case dirs::u_l:
                            (*evil)->AIDataIN.obst_up = true;
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::down:
                            (*evil)->AIDataIN.obst_down = true;
                            break;

                        case dirs::d_r:
                            (*evil)->AIDataIN.obst_down = true;
                            (*evil)->AIDataIN.obst_right = true;
                            break;

                        case dirs::d_l:
                            (*evil)->AIDataIN.obst_down = true;
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::left:
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::right:
                            (*evil)->AIDataIN.obst_right = true;
                            break;
                        }
                    }
                }
            }
        }

        if (!vEvils.empty() && !vHomes.empty())
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                for (std::vector<dll::BUILDING*>::iterator home = vHomes.begin(); home < vHomes.end(); home++)
                {
                    if (!((*evil)->x >= (*home)->ex || (*evil)->ex <= (*home)->x ||
                        (*evil)->y >= (*home)->ey || (*evil)->ey <= (*home)->y))
                    {
                        switch ((*evil)->dir)
                        {
                        case dirs::up:
                            (*evil)->AIDataIN.obst_up = true;
                            break;

                        case dirs::u_r:
                            (*evil)->AIDataIN.obst_up = true;
                            (*evil)->AIDataIN.obst_right = true;
                            break;

                        case dirs::u_l:
                            (*evil)->AIDataIN.obst_up = true;
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::down:
                            (*evil)->AIDataIN.obst_down = true;
                            break;

                        case dirs::d_r:
                            (*evil)->AIDataIN.obst_down = true;
                            (*evil)->AIDataIN.obst_right = true;
                            break;

                        case dirs::d_l:
                            (*evil)->AIDataIN.obst_down = true;
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::left:
                            (*evil)->AIDataIN.obst_left = true;
                            break;

                        case dirs::right:
                            (*evil)->AIDataIN.obst_right = true;
                            break;
                        }
                    }
                }
            }
        }


        //BATTLES *****************************************

        if (!vHeroes.empty() && !vEvils.empty())
        {
            bool killed = false;

            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                for (std::vector<dll::Creature>::iterator hero = vHeroes.begin(); hero < vHeroes.end(); hero++)
                {
                    if (!((*hero)->x >= (*evil)->ex || (*hero)->ex <= (*evil)->x ||
                        (*hero)->y >= (*evil)->ey || (*hero)->ey <= (*evil)->y))
                    {
                        if ((*hero)->Shoot())(*evil)->lifes -= (*hero)->strenght;
                        if ((*evil)->Shoot())(*hero)->lifes -= (*evil)->strenght;

                        if ((*hero)->lifes <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\hdeath.wav", NULL, NULL, NULL);
                            (*hero)->Release();
                            vHeroes.erase(hero);
                            (*evil)->AIDataIN.near_enemy_x = -1;
                            (*evil)->AIDataIN.near_enemy_y = -1;
                            break;
                        }
                        if ((*evil)->lifes <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\edeath.wav", NULL, NULL, NULL);
                            score += 10 * (int)(game_speed) * (*evil)->strenght;
                            (*hero)->AIDataIN.near_enemy_x = -1;
                            (*hero)->AIDataIN.near_enemy_y = -1;
                            (*evil)->Release();
                            vEvils.erase(evil);
                            killed = true;
                            break;
                        }
                    }
                }

                if (killed)break;
            }
        }
        
        if (!vTowers.empty() && !vEvils.empty())
        {
            for (std::vector<dll::BUILDING*>::iterator tower = vTowers.begin(); tower < vTowers.end(); tower++)
            {
                bool killed = false;
                for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
                {
                    if (!((*evil)->x >= (*tower)->ex || (*evil)->ex <= (*tower)->x ||
                        (*evil)->y >= (*tower)->ey || (*evil)->ey <= (*tower)->y))
                    {
                        if ((*evil)->Shoot())(*tower)->lifes -= (*evil)->strenght;
                        if ((*tower)->lifes <= 0)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\explosion.wav", NULL, NULL, NULL);
                            vFires.push_back(dll::BUILDING::TileFactory(buildings::fire, (*tower)->x, (*tower)->y));
                            (*tower)->Release();
                            vTowers.erase(tower);
                            killed = true;
                            break;
                        }
                    }
                }
                if (killed)break;
            }
        }

        if (!vTowers.empty() && !vEvils.empty())
        {
            for (std::vector<dll::BUILDING*>::iterator tower = vTowers.begin(); tower < vTowers.end(); tower++)
            {
                for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
                {
         
                    if (((abs((*tower)->x - (*evil)->ex) < 100) || (abs((*tower)->ex - (*evil)->x) < 100)) &&
                        ((abs((*tower)->y - (*evil)->ey) < 100) || (abs((*tower)->ey - (*evil)->y) < 100)))
                    {
                        if (rand() % 100 == 6)
                        {
                            if (sound)mciSendString(L"play .\\res\\snd\\shoot.wav", NULL, NULL, NULL);
                            vAxes.push_back({ dll::OBJECT((*tower)->x + 10.0f, (*tower)->y, 15.0f, 15.0f) });

                            if ((*tower)->y > (*evil)->y && (*tower)->y < (*evil)->ey)
                            {
                                if ((*tower)->x < (*evil)->x)vAxes.back().dir = dirs::right;
                                else if ((*tower)->x > (*evil)->x)vAxes.back().dir = dirs::left;
                            }
                            else if ((*tower)->y < (*evil)->y)
                            {
                                if ((*tower)->x < (*evil)->x)vAxes.back().dir = dirs::d_r;
                                else if ((*tower)->x > (*evil)->x)vAxes.back().dir = dirs::d_l;
                                else vAxes.back().dir = dirs::down;
                            }
                            else if ((*tower)->y > (*evil)->y)
                            {
                                if ((*tower)->x < (*evil)->x)vAxes.back().dir = dirs::u_r;
                                else if ((*tower)->x > (*evil)->x)vAxes.back().dir = dirs::u_l;
                                else vAxes.back().dir = dirs::up;
                            }
                            break;
                        }
                    }
                }
            }
        }

        if (!vAxes.empty())
        {
            for (std::vector<AXE>::iterator axe = vAxes.begin(); axe < vAxes.end(); axe++)
            {
                switch ((*axe).dir)
                {
                case dirs::up:
                    (*axe).Dims.y -= game_speed * 3;
                    break;

                case dirs::down:
                    (*axe).Dims.y += game_speed * 3;
                    break;

                case dirs::left:
                    (*axe).Dims.x -= game_speed * 3;
                    break;

                case dirs::right:
                    (*axe).Dims.x += game_speed * 3;
                    break;

                case dirs::u_r:
                    (*axe).Dims.y -= game_speed * 3;
                    (*axe).Dims.x += game_speed * 3;
                    break;

                case dirs::u_l:
                    (*axe).Dims.y -= game_speed * 3;
                    (*axe).Dims.x -= game_speed * 3;
                    break;
                
                case dirs::d_r:
                    (*axe).Dims.y += game_speed * 3;
                    (*axe).Dims.x += game_speed * 3;
                    break;

                case dirs::d_l:
                    (*axe).Dims.y += game_speed * 3;
                    (*axe).Dims.x -= game_speed * 3;
                    break;
                }

                (*axe).Dims.SetEdges();

                if ((*axe).Dims.ex <= 0 || (*axe).Dims.x >= scr_width || (*axe).Dims.ey <= 50.0f || (*axe).Dims.y >= scr_height)
                {
                    vAxes.erase(axe);
                    break;
                }
            }
        }

        if (!vEvils.empty() && !vWalls.empty())
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                for (std::vector<dll::BUILDING*>::iterator wall = vWalls.begin(); wall < vWalls.end(); wall++)
                {
                    if (!((*evil)->x >= (*wall)->ex || (*evil)->ex <= (*wall)->x ||
                        (*evil)->y >= (*wall)->ey || (*evil)->ey <= (*wall)->y))
                    {
                        (*wall)->lifes -= (*evil)->strenght;
                        if ((*wall)->lifes <= 0)
                        {
                            vFires.push_back(dll::BUILDING::TileFactory(buildings::fire, (*wall)->x, (*wall)->y));
                            (*wall)->Release();
                            vWalls.erase(wall);
                            break;
                        }
                    }
                }
            }
        }

        if (!vEvils.empty() && !vHomes.empty())
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                for (std::vector<dll::BUILDING*>::iterator wall = vHomes.begin(); wall < vHomes.end(); wall++)
                {
                    if (!((*evil)->x >= (*wall)->ex || (*evil)->ex <= (*wall)->x ||
                        (*evil)->y >= (*wall)->ey || (*evil)->ey <= (*wall)->y))
                    {
                        (*wall)->lifes -= (*evil)->strenght;
                        if ((*wall)->lifes <= 0)
                        {
                            vFires.push_back(dll::BUILDING::TileFactory(buildings::fire, (*wall)->x, (*wall)->y));
                            
                            int row = (int)(((*wall)->y - 50.0f) / 50);
                            int col = (int)((*wall)->x / 50);

                            bool up_ok = (row - 1 >= 0);
                            bool down_ok = (row + 1 <= 12);
                            bool left_ok = (col - 1 >= 0);
                            bool right_ok = (col + 1 <= 19);

                            if (up_ok)
                            {
                                if (FieldGrid[row - 1][col]->type == buildings::soil_tile)
                                    FieldGrid[row - 1][col]->type = buildings::snow_tile;
                                
                                if (left_ok)
                                    if (FieldGrid[row - 1][col - 1]->type == buildings::soil_tile)
                                        FieldGrid[row - 1][col - 1]->type = buildings::snow_tile;
                                
                                if (right_ok)
                                    if (FieldGrid[row - 1][col + 1]->type == buildings::soil_tile)
                                        FieldGrid[row - 1][col + 1]->type = buildings::snow_tile;
                            }
                            if (down_ok)
                            {
                                if (FieldGrid[row + 1][col]->type == buildings::soil_tile)
                                    FieldGrid[row + 1][col]->type = buildings::snow_tile;

                                if (left_ok)
                                    if (FieldGrid[row + 1][col - 1]->type == buildings::soil_tile)
                                        FieldGrid[row + 1][col - 1]->type = buildings::snow_tile;

                                if (right_ok)
                                    if (FieldGrid[row + 1][col + 1]->type == buildings::soil_tile)
                                        FieldGrid[row + 1][col + 1]->type = buildings::snow_tile;
                            }
                            if (left_ok)
                                if (FieldGrid[row][col - 1]->type == buildings::soil_tile)
                                    FieldGrid[row][col - 1]->type = buildings::snow_tile;
                            if (right_ok)
                                if (FieldGrid[row][col + 1]->type == buildings::soil_tile)
                                    FieldGrid[row][col + 1]->type = buildings::snow_tile;
                            
                            (*wall)->Release();
                            vHomes.erase(wall);
                            break;
                        }
                    }
                }
            }
        }

        base_under_attack = false;

        if (!vEvils.empty() && TownHall)
        {
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                if (!((*evil)->x >= TownHall->ex || (*evil)->ex <= TownHall->x ||
                    (*evil)->y >= TownHall->ey || (*evil)->ey <= TownHall->y))
                {
                    TownHall->lifes -= (*evil)->strenght;

                    base_under_attack = true;

                    if (TownHall->lifes <= 0)
                    {
                        vFires.push_back(dll::BUILDING::TileFactory(buildings::fire, TownHall->x, TownHall->y));
                        delete TownHall;
                        TownHall = nullptr;
                        break;
                    }
                }
            }
        }

        if (!vEvils.empty() && !vAxes.empty())
        {
            bool killed = false;

            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                for (std::vector<AXE>::iterator axe = vAxes.begin(); axe < vAxes.end(); axe++)
                {
                    if (!((*evil)->x >= axe->Dims.ex || (*evil)->ex <= axe->Dims.x ||
                        (*evil)->y >= axe->Dims.ey || (*evil)->ey <= axe->Dims.y))
                    {
                        vAxes.erase(axe);
                        (*evil)->lifes -= (int)(20 * game_speed);
                        if ((*evil)->lifes <= 0)
                        {
                            score += (int)(50 * game_speed);
                            (*evil)->Release();
                            vEvils.erase(evil);
                            killed = true;
                        }
                        break;
                    }
                }

                if (killed)break;
            }
        }

        //DRAW THINGS ***************************************

        Draw->BeginDraw();
        if (butBckg && nrmTxt && txtBrush && txtHgltBrush && txtInactBrush)
        {
            Draw->FillRectangle(D2D1::RectF(0, 0, scr_width, 50.0f), butBckg);
            if (name_set)
                Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmTxt, b1TxtRect, txtInactBrush);
            else
            {
                if(!b1Hglt)Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmTxt, b1TxtRect, txtBrush);
                else Draw->DrawTextW(L"ИМЕ НА ИГРАЧ", 13, nrmTxt, b1TxtRect, txtHgltBrush);
            }

            if (!b2Hglt)Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTxt, b2TxtRect, txtBrush);
            else Draw->DrawTextW(L"ЗВУЦИ ON / OFF", 15, nrmTxt, b2TxtRect, txtHgltBrush);

            if (!b3Hglt)Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTxt, b3TxtRect, txtBrush);
            else Draw->DrawTextW(L"ПОМОЩ ЗА ИГРАТА", 16, nrmTxt, b3TxtRect, txtHgltBrush);
        }


        for (int row = 0; row < 14; row++)
        {
            for (int col = 0; col < 20; col++)
            {
                switch (FieldGrid[row][col]->type)
                {
                case buildings::snow_tile:
                    if (row < 13)Draw->DrawBitmap(bmpSnow, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                        FieldGrid[row][col]->ex, FieldGrid[row][col]->ey));
                    else Draw->FillRectangle(D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                        FieldGrid[row][col]->ex, FieldGrid[row][col]->ey), FieldBrush);
                    break;

                case buildings::soil_tile:
                    Draw->DrawBitmap(bmpSoil, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                        FieldGrid[row][col]->ex, FieldGrid[row][col]->ey));
                    break;

                case buildings::wall:
                    if (row < 13)Draw->DrawBitmap(bmpWall, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                        FieldGrid[row][col]->x + 50.0f, FieldGrid[row][col]->y + 50.0f));
                    else
                    {
                        Draw->FillRectangle(D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                            FieldGrid[row][col]->ex, FieldGrid[row][col]->ey), FieldBrush);
                        Draw->DrawBitmap(bmpWall, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                            FieldGrid[row][col]->x + 50.0f, FieldGrid[row][col]->y + 35.0f));
                    }
                    break;

                case buildings::home:
                    if (row < 13)Draw->DrawBitmap(bmpHome, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                        FieldGrid[row][col]->x + 50.0f, FieldGrid[row][col]->y + 32.0f));
                    else
                    {
                        Draw->FillRectangle(D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                            FieldGrid[row][col]->ex, FieldGrid[row][col]->ey), FieldBrush);
                        Draw->DrawBitmap(bmpHome, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                            FieldGrid[row][col]->x + 50.0f, FieldGrid[row][col]->y + 32.0f));
                    }
                    break;

                case buildings::tower:
                    if (row < 13)Draw->DrawBitmap(bmpTower, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                        FieldGrid[row][col]->x + 25.0f, FieldGrid[row][col]->y + 50.0f));
                    else
                    {
                        Draw->FillRectangle(D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                            FieldGrid[row][col]->ex, FieldGrid[row][col]->ey), FieldBrush);
                        Draw->DrawBitmap(bmpTower, D2D1::RectF(FieldGrid[row][col]->x, FieldGrid[row][col]->y,
                            FieldGrid[row][col]->x + 25.0f, FieldGrid[row][col]->y + 50.0f));
                    }
                    break;
                }
            }
        }
        if (TownHall)
        {
            Draw->DrawBitmap(bmpTownHall, D2D1::RectF(TownHall->x, TownHall->y, TownHall->ex, TownHall->ey));
            if (LifeBrush && HitBrush && CritBrush)
            {
                if (TownHall->lifes > 500)Draw->DrawLine(D2D1::Point2F(TownHall->x - 10.0f, TownHall->ey + 5.0f),
                    D2D1::Point2F(TownHall->x + (float)(TownHall->lifes / 300), TownHall->ey + 5.0f), LifeBrush, 5.0f);
                else if (TownHall->lifes > 100)Draw->DrawLine(D2D1::Point2F(TownHall->x - 10.0f, TownHall->ey + 5.0f),
                    D2D1::Point2F(TownHall->x + (float)(TownHall->lifes / 300), TownHall->ey + 5.0f), HitBrush, 5.0f);
                else Draw->DrawLine(D2D1::Point2F(TownHall->x - 10.0f, TownHall->ey + 5.0f),
                    D2D1::Point2F(TownHall->x + (float)(TownHall->lifes / 300), TownHall->ey + 5.0f), CritBrush, 5.0f);
            }
        }
        Draw->DrawBitmap(bmpHero, D2D1::RectF(FieldGrid[13][17]->x +10.0f, FieldGrid[13][17]->y, 
            FieldGrid[13][17]->x + 37.0f, FieldGrid[13][17]->y + 40.0f));

        if (carry_heroe)Draw->DrawBitmap(bmpHero, D2D1::RectF((float)(cur_pos.x), (float)(cur_pos.y), 
            (float)(cur_pos.x) + 27.0f, (float)(cur_pos.y) + 40.0f));
        if (carry_house)Draw->DrawBitmap(bmpHome, D2D1::RectF((float)(cur_pos.x), (float)(cur_pos.y), 
            (float)(cur_pos.x) + 40.0f, (float)(cur_pos.y) + 40.0f));
        if (carry_wall)Draw->DrawBitmap(bmpWall, D2D1::RectF((float)(cur_pos.x), (float)(cur_pos.y),
            (float)(cur_pos.x) + 40.0f, (float)(cur_pos.y) + 40.0f));
        if (carry_tower)Draw->DrawBitmap(bmpTower, D2D1::RectF((float)(cur_pos.x), (float)(cur_pos.y),
            (float)(cur_pos.x) + 25.0f, (float)(cur_pos.y) + 50.0f));
        
        //DRAW STATUS ********************************

        wchar_t status[300] = L"\0";
        wchar_t add[5] = L"\0";
        int txt_size = 0;

        wcscpy_s(status, current_player);
        
        wcscat_s(status, L", ДЪРВЕТА: ");
        wsprintf(add, L"%d", wood);
        wcscat_s(status, add);

        wcscat_s(status, L", ЗАВЛАДЕНИ: ");
        wsprintf(add, L"%d", win_percentage);
        wcscat_s(status, add);
        wcscat_s(status, L" %");

        wcscat_s(status, L", РЕЗУЛТАТ: ");
        wsprintf(add, L"%d", score);
        wcscat_s(status, add);

        wcscat_s(status, L", ВРЕМЕ: ");
        if (mins < 10)
            wcscat_s(status, L"0");
        wsprintf(add, L"%d", mins);
        wcscat_s(status, add);

        wcscat_s(status, L" : ");
        if (secs - mins * 60 < 10)
            wcscat_s(status, L"0");
        wsprintf(add, L"%d", secs - mins * 60);
        wcscat_s(status, add);
        
        for (int i = 0; i < 300; i++)
            if (status[i] != '\0')txt_size++;
            else break;

        if (nrmTxt && txtBrush)
            Draw->DrawText(status, txt_size, nrmTxt, D2D1::RectF(30.0f, scr_height - 60.0f, scr_width, scr_height), txtBrush);

        
        ////////////////////////////////////////////

        if (!vHomes.empty())
        {
            for (std::vector<dll::BUILDING*>::iterator it = vHomes.begin(); it < vHomes.end(); ++it)
            {
                Draw->DrawBitmap(bmpHome, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                if (LifeBrush && HitBrush && CritBrush)
                {
                    if ((*it)->lifes > 60)Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 80), (*it)->ey + 5.0f), LifeBrush, 5.0f);
                    else if ((*it)->lifes > 20)Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 80), (*it)->ey + 5.0f), HitBrush, 5.0f);
                    else Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 80), (*it)->ey + 5.0f), CritBrush, 5.0f);
                }
            }
        }
        if (!vTowers.empty())
        {
            for (std::vector<dll::BUILDING*>::iterator it = vTowers.begin(); it < vTowers.end(); ++it)
            {
                Draw->DrawBitmap(bmpTower, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                if (LifeBrush && HitBrush && CritBrush)
                {
                    if ((*it)->lifes > 60)Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 3), (*it)->ey + 5.0f), LifeBrush, 5.0f);
                    else if ((*it)->lifes > 20)Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 3), (*it)->ey + 5.0f), HitBrush, 5.0f);
                    else Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 3), (*it)->ey + 5.0f), CritBrush, 5.0f);
                }
            }
        }
        if (!vWalls.empty())
        {
            for (std::vector<dll::BUILDING*>::iterator it = vWalls.begin(); it < vWalls.end(); ++it)
            {
                Draw->DrawBitmap(bmpWall, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
            }
        }
        if (!vTrees.empty())
        {
            for (std::vector<dll::BUILDING*>::iterator it = vTrees.begin(); it < vTrees.end(); ++it)
            {
                Draw->DrawBitmap(bmpTree, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
            }
        }
        if (!vFires.empty())
        {
            for (std::vector<dll::BUILDING*>::iterator it = vFires.begin(); it < vFires.end(); it++)
            {
                int frame = (*it)->GetFrame();
                if (frame > 15)
                {
                    int row = (int)(((*it)->y - 50.0f) / 50);
                    int col = (int)((*it)->x / 50);
                    FieldGrid[row][col]->type = buildings::snow_tile;
                    (*it)->Release();
                    vFires.erase(it);
                    
                    break;
                }
                else
                    Draw->DrawBitmap(bmpFire[frame], D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
            }
        }
        if (!vAxes.empty())
        {
            for (std::vector<AXE>::iterator it = vAxes.begin(); it < vAxes.end(); ++it)
            {
                Draw->DrawBitmap(bmpAxe, D2D1::RectF((*it).Dims.x, (*it).Dims.y, (*it).Dims.ex, (*it).Dims.ey));
            }
        }


        if (!vHeroes.empty())
        {
            for (std::vector<dll::Creature>::iterator it = vHeroes.begin(); it < vHeroes.end(); ++it)
            {
                Draw->DrawBitmap(bmpHero, D2D1::RectF((*it)->x, (*it)->y, (*it)->ex, (*it)->ey));
                if (LifeBrush && HitBrush && CritBrush)
                {
                    if ((*it)->lifes > 60)Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 3), (*it)->ey + 5.0f), LifeBrush, 5.0f);
                    else if ((*it)->lifes > 20)Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 3), (*it)->ey + 5.0f), HitBrush, 5.0f);
                    else Draw->DrawLine(D2D1::Point2F((*it)->x - 10.0f, (*it)->ey + 5.0f),
                        D2D1::Point2F((*it)->x + (float)((*it)->lifes / 3), (*it)->ey + 5.0f), CritBrush, 5.0f);
                }
            }
        }

        if(!vEvils.empty())
            for (std::vector<dll::Creature>::iterator evil = vEvils.begin(); evil < vEvils.end(); evil++)
            {
                switch ((*evil)->type)
                {
                case creatures::evil1:
                    Draw->DrawBitmap(bmpEvil1, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    break;

                case creatures::evil2:
                    Draw->DrawBitmap(bmpEvil2, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    break;

                case creatures::evil3:
                    Draw->DrawBitmap(bmpEvil3, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    break;

                case creatures::bear:
                    Draw->DrawBitmap(bmpBear, D2D1::RectF((*evil)->x, (*evil)->y, (*evil)->ex, (*evil)->ey));
                    break;
                }

                if (LifeBrush && HitBrush && CritBrush)
                {
                    if ((*evil)->lifes > 50)Draw->DrawLine(D2D1::Point2F((*evil)->x - 10.0f, (*evil)->ey + 5.0f),
                        D2D1::Point2F((*evil)->x + (float)((*evil)->lifes / 3), (*evil)->ey + 5.0f), LifeBrush, 5.0f);
                    else if ((*evil)->lifes > 20)Draw->DrawLine(D2D1::Point2F((*evil)->x - 10.0f, (*evil)->ey + 5.0f),
                        D2D1::Point2F((*evil)->x + (float)((*evil)->lifes / 3), (*evil)->ey + 5.0f), HitBrush, 5.0f);
                    else Draw->DrawLine(D2D1::Point2F((*evil)->x - 10.0f, (*evil)->ey + 5.0f),
                        D2D1::Point2F((*evil)->x + (float)((*evil)->lifes / 3), (*evil)->ey + 5.0f), CritBrush, 5.0f);
                }
            }

        //////////////////////////////////////////////////////
        Draw->EndDraw();

        if (win_percentage >= 85)
        {
            win_game = true;
            GameOver();
        }
        if (!TownHall)GameOver();
    }

    std::remove(tmp_file);
    ReleaseResources();
    return (int) bMsg.wParam;
}