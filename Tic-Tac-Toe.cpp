#include "mbed.h"
#include "lpc1114etf.h"
#include <array>
#include "N5110.h"

// kolone
DigitalIn Column1(dp9);
DigitalIn Column2(dp10);
DigitalIn Column3(dp11);
DigitalIn Column4(dp13);
BusOut Rows(dp16, dp15, dp17, dp18); // redovi

Ticker ScreenPrint; // ticker za ispis
Ticker Keyboard; // ticker za unos sa tastature
Timer Debounce; // debouncing
N5110 NokiaScreen(dp4, dp25, dp23, dp26, dp2, dp6, dp24); // ekran
enum class Phase {Initialisation, ChooseLanguage, MainMenu, ChooseSign, Play, Reset, Result, Standings, Repeat, End}; // razlicite faze igre
enum class Language {English, German, Bosnian}; // jezici
enum class Sign {Empty, O, X}; // moguce vrijednosti unutar polja
enum class Player {Player1, Player2, None}; // pobjednik
std::array<Sign, 10> Board = {Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty}; // niz sa vrijednostima svih polja
std::array<Sign, 10> Empty_Board = {Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty, Sign::Empty}; // backup prazni niz
Language Current_Language = Language::English;
Sign Current_Sign = Sign::Empty;
Phase Current_Phase = Phase::Initialisation;
Player Last_Winner = Player::None;
Player Current_Player = Player::Player1;
bool Repeat = 1; // 0 za ne, 1 za da
char Row1[20]; // char niz za ispis prvog reda
char Row2[20];
char Row3[20];
char Row4[20];
char Row5[20];
char Row6[20];
char Row7[20];
char Row8[20];
int Counter = 0; // brojac
int WinnerCheckCounter = 0; // brojac ulazaka u f-ju WinnerCheck
int Player1_Wins = 0; // broj pobjeda igraca 1
int Player2_Wins = 0; // broj pobjeda igraca 2

void WeHaveAWinner() // poziva se pri pobjednickoj kombinaciji
{
    if(Last_Winner == Player::Player1)
        Player1_Wins++;
    if(Last_Winner == Player::Player2)
        Player2_Wins++;
    WinnerCheckCounter = 0;
    Current_Phase = Phase::Result;
    Board = Empty_Board;
    NokiaScreen.clear();
}

void WinnerCheck(Player P) // provjera da li postoji pobjednik
{
    WinnerCheckCounter++;
    if(Board.at(1) == Board.at(4) && Board.at(4) == Board.at(7) && Board.at(7) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(Board.at(2) == Board.at(5) && Board.at(5) == Board.at(8) && Board.at(8) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(Board.at(3) == Board.at(6) && Board.at(6) == Board.at(9) && Board.at(9) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(Board.at(1) == Board.at(5) && Board.at(5) == Board.at(9) && Board.at(9) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(Board.at(3) == Board.at(5) && Board.at(5) == Board.at(7) && Board.at(7) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(Board.at(1) == Board.at(2) && Board.at(2) == Board.at(3) && Board.at(3) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(Board.at(4) == Board.at(5) && Board.at(5) == Board.at(6) && Board.at(6) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(Board.at(7) == Board.at(8) && Board.at(8) == Board.at(9) && Board.at(9) != Sign::Empty)
        {Last_Winner = P;
        WeHaveAWinner();}
    if(WinnerCheckCounter == 9)
        {Last_Winner = Player::None;
        WeHaveAWinner();}
}

void DrawX(int x0, int y0, int x1, int y1) // crta X
{
    NokiaScreen.drawLine(x0, y0, x1, y1, 1);
    NokiaScreen.drawLine(x0, y1, x1, y0, 1);
}

void CurrentSignChange() // mijenja znak tokom igre
{
    WinnerCheck(Current_Player); 
    if(Current_Sign == Sign::X)
        Current_Sign = Sign::O;
    else if(Current_Sign == Sign::O)
        Current_Sign = Sign::X;    
}

void CurrentPlayerChange() // mijenja igraca tokom igre
{
    if(Current_Player == Player::Player1)
        Current_Player = Player::Player2;
    else if(Current_Player == Player::Player2)
        Current_Player = Player::Player1;      
}

void Push() // matricna tastatura
{
    Rows = 1;
    if(Column1 && Current_Phase == Phase::ChooseLanguage && Debounce.read_ms()>200)
        {Current_Language = Language::English;
        Current_Phase = Phase::MainMenu;
        NokiaScreen.clear();
        Debounce.reset();}
    if(Column2 && Current_Phase == Phase::ChooseLanguage && Debounce.read_ms()>200)
        {Current_Language = Language::German;
        Current_Phase = Phase::MainMenu;
        NokiaScreen.clear();
        Debounce.reset();}
    if(Column3 && Current_Phase == Phase::ChooseLanguage && Debounce.read_ms()>200)
        {Current_Language = Language::Bosnian;
        Current_Phase = Phase::MainMenu;
        NokiaScreen.clear();
        Debounce.reset();}

    if(Column1 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(1) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(38, 2, 50, 14);
            Board.at(1) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(44, 8, 6, 0);
            Board.at(1) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();}

    if(Column2 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(2) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(54, 2, 66, 14);
            Board.at(2) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(60, 8, 6, 0);
            Board.at(2) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();}

    if(Column3 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(3) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(70, 2, 82, 14);
            Board.at(3) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(76, 8, 6, 0);
            Board.at(3) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();}      

    if(Column1 && Current_Phase == Phase::Repeat && Debounce.read_ms()>200)
        {Repeat = 1;
        Current_Phase = Phase::ChooseSign;
        NokiaScreen.clear();
        Debounce.reset();}  

    Rows = 2;
    if(Column1 && Current_Phase == Phase::MainMenu && Debounce.read_ms()>200)
        {Current_Phase = Phase::ChooseSign;
        NokiaScreen.clear();
        Debounce.reset();}
    if(Column2 && Current_Phase == Phase::MainMenu && Debounce.read_ms()>200)
        {Current_Phase = Phase::Standings;
        NokiaScreen.clear();
        Debounce.reset();}
    if(Column3 && Current_Phase == Phase::MainMenu && Debounce.read_ms()>200)
        {Current_Phase = Phase::ChooseLanguage;
        NokiaScreen.clear();
        Debounce.reset();}

    if(Column1 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(4) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(38, 18, 50, 30);
            Board.at(4) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(44, 24, 6, 0);
            Board.at(4) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();}
    if(Column2 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(5) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(54, 18, 66, 30);
            Board.at(5) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(60, 24, 6, 0);
            Board.at(5) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();}
    if(Column3 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(6) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(70, 18, 82, 30);
            Board.at(6) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(76, 24, 6, 0);
            Board.at(6) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();} 

    Rows = 4;
    if(Column1 && Current_Phase == Phase::MainMenu && Debounce.read_ms()>200)
        {Current_Phase = Phase::Reset;
        NokiaScreen.clear();
        Debounce.reset();}

    if(Column1 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(7) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(38, 34, 50, 46);
            Board.at(7) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(44, 40, 6, 0);
            Board.at(7) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();}

    if(Column2 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(8) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(54, 34, 66, 46);
            Board.at(8) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(60, 40, 6, 0);
            Board.at(8) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();}

    if(Column3 && Current_Phase == Phase::Play && Debounce.read_ms()>200 && Board.at(9) == Sign::Empty)
        {if(Current_Sign == Sign::X)
            {DrawX(70, 34, 82, 46);
            Board.at(9) = Sign::X;}
        if(Current_Sign == Sign::O)
            {NokiaScreen.drawCircle(76, 40, 6, 0);
            Board.at(9) = Sign::O;}
        Debounce.reset();
        CurrentSignChange();
        CurrentPlayerChange();} 

    Rows = 8;
    if(Column1 && Current_Phase == Phase::ChooseSign && Debounce.read_ms()>200)
        {Current_Sign = Sign::X;
        Current_Phase = Phase::Play;
        NokiaScreen.clear();
        Debounce.reset();}

    if(Column2 && Current_Phase == Phase::ChooseSign && Debounce.read_ms()>200)
        {Current_Sign = Sign::O;
        Current_Phase = Phase::Play;
        NokiaScreen.clear();
        Debounce.reset();}

    if(Column3 && Debounce.read_ms()>200 && Current_Phase != Phase::End)
        {Last_Winner = Player::None;
        int Counter = 0;
        WeHaveAWinner();
        Current_Phase = Phase::MainMenu;   
        Debounce.reset();}

    if(Column1 && Current_Phase == Phase::Standings && Debounce.read_ms()>200)
        {Current_Phase = Phase::Repeat;
        NokiaScreen.clear();
        Debounce.reset();}        

    if(Column2 && Current_Phase == Phase::Repeat && Debounce.read_ms()>200)
        {Repeat = 0;
        NokiaScreen.clear();
        Current_Phase = Phase::End;
        Debounce.reset();}   
}

void Initialisation() // prikaz pri ukljucivanju igre
{
    Counter++;
    sprintf(Row1, "Tic Tac Toe");
    NokiaScreen.printString(Row1, 0, 0);
    if(Counter > 100)
        {DrawX(7, 10, 35, 38);
        NokiaScreen.drawCircle(62, 24, 14, 0);}
    if(Counter > 200)
        {sprintf(Row2, "ETF Sarajevo");
        NokiaScreen.printString(Row2, 0, 5);}
    if(Counter > 400)
        {NokiaScreen.clear();
        Counter = 0;
        Current_Phase = Phase::ChooseLanguage;}
}

// izaberi jezik - tri inacice
void ChooseLanguageEN() 
{
    sprintf(Row1, "Choose the ");
    sprintf(Row2, "language");
    sprintf(Row3, "1 - English");
    sprintf(Row4, "2 - German");
    sprintf(Row5, "3 - Bosnian");
    sprintf(Row6, "# - Main menu");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
    NokiaScreen.printString(Row6, 0, 5);
}

void ChooseLanguageDE()
{
    sprintf(Row1, "Waehlen Sie ");
    sprintf(Row2, "die Sprache");
    sprintf(Row3, "1 - Englisch");
    sprintf(Row4, "2 - Deutsch");
    sprintf(Row5, "3 - Bosnisch");
    sprintf(Row6, "# - Hauptmenue");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
    NokiaScreen.printString(Row6, 0, 5);
}

void ChooseLanguageBA()
{
    sprintf(Row1, "Odaberite ");
    sprintf(Row2, "jezik");
    sprintf(Row3, "1 - Engleski");
    sprintf(Row4, "2 - Njemacki");
    sprintf(Row5, "3 - Bosanski");
    sprintf(Row6, "# Glavni meni");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
    NokiaScreen.printString(Row6, 0, 5);
}

// glavni meni - tri inacice
void MainMenuEN()
{
    sprintf(Row1, "Main menu");
    sprintf(Row2, "4 - Play");
    sprintf(Row3, "5 - Standings");
    sprintf(Row4, "6 - Language");
    sprintf(Row5, "7 - Reset");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
}

void MainMenuDE()
{
    sprintf(Row1, "Hauptmenue");
    sprintf(Row2, "4 - Spiel");
    sprintf(Row3, "5 - Tabelle");
    sprintf(Row4, "6 - Sprache");
    sprintf(Row5, "7 - Zrueckstzn");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
}

void MainMenuBA()
{
    sprintf(Row1, "Glavni meni");
    sprintf(Row2, "4 - Igraj");
    sprintf(Row3, "5 - Tabela");
    sprintf(Row4, "6 - Jezik");
    sprintf(Row5, "7 - Reset");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
}

// izaberi znak - tri inacice
void ChooseSignEN()
{
    if(Current_Player == Player::Player1)
        sprintf(Row1, "Player 1");
    else if(Current_Player == Player::Player2)
        sprintf(Row1, "Player 2");
    sprintf(Row2, "Choose the ");
    sprintf(Row3, "sign");
    sprintf(Row4, "0 - Circle");
    sprintf(Row5, "* - Cross");
    sprintf(Row6, "# - Main menu");    
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
    NokiaScreen.printString(Row6, 0, 5);    
}

void ChooseSignDE()
{
    if(Current_Player == Player::Player1)
        sprintf(Row1, "Spieler 1");
    else if(Current_Player == Player::Player2)
        sprintf(Row1, "Spieler 2");
    sprintf(Row2, "Waehlen Sie ");
    sprintf(Row3, "das Zeichen");
    sprintf(Row4, "0 - Kreis");
    sprintf(Row5, "* - Kreuz");
    sprintf(Row6, "# - Hauptmenue");    
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
    NokiaScreen.printString(Row6, 0, 5);    
}

void ChooseSignBA()
{
    if(Current_Player == Player::Player1)
        sprintf(Row1, "Igrac 1");
    else if(Current_Player == Player::Player2)
        sprintf(Row1, "Igrac 2");
    sprintf(Row2, "Odaberite ");
    sprintf(Row3, "znak");
    sprintf(Row4, "0 - Kruzic");
    sprintf(Row5, "* - Krizic");
    sprintf(Row6, "# Glavni meni");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
    NokiaScreen.printString(Row6, 0, 5);
}

// postavi inicijalni prikaz igre - tri inacice
void SetInitialPlayscreenEN()
{
    sprintf(Row1, "PL1");
    sprintf(Row2, "PL2");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 3);
    if(Current_Player == Player::Player1)
        {NokiaScreen.drawLine(0, 8, 31, 8, 1);
        NokiaScreen.drawLine(0, 32, 31, 32, 0);}
    if(Current_Player == Player::Player2)
        {NokiaScreen.drawLine(0, 8, 31, 8, 0);
        NokiaScreen.drawLine(0, 32, 31, 32, 1);}
    if((Current_Player == Player::Player1 && Current_Sign == Sign::X) || (Current_Player == Player::Player2 && Current_Sign == Sign::O))
        {DrawX(25, 0, 31, 6);
        NokiaScreen.drawCircle(28, 27, 3, 0);}
    if((Current_Player == Player::Player1 && Current_Sign == Sign::O) || (Current_Player == Player::Player2 && Current_Sign == Sign::X))
        {DrawX(25, 24, 31, 30);
        NokiaScreen.drawCircle(28, 3, 3, 0);}
    NokiaScreen.drawLine(35, 16, 83, 16, 1);
    NokiaScreen.drawLine(35, 32, 83, 32, 1);
    NokiaScreen.drawLine(68, 0, 68, 47, 1);
    NokiaScreen.drawLine(52, 0, 52, 47, 1);
    Counter = 0;         
}

void SetInitialPlayscreenDE()
{
    sprintf(Row1, "SP1");
    sprintf(Row2, "SP2");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 3);
    if(Current_Player == Player::Player1)
        {NokiaScreen.drawLine(0, 8, 31, 8, 1);
        NokiaScreen.drawLine(0, 32, 31, 32, 0);}
    if(Current_Player == Player::Player2)
        {NokiaScreen.drawLine(0, 8, 31, 8, 0);
        NokiaScreen.drawLine(0, 32, 31, 32, 1);}
    if((Current_Player == Player::Player1 && Current_Sign == Sign::X) || (Current_Player == Player::Player2 && Current_Sign == Sign::O))
        {DrawX(25, 0, 31, 6);
        NokiaScreen.drawCircle(28, 27, 3, 0);}
    if((Current_Player == Player::Player1 && Current_Sign == Sign::O) || (Current_Player == Player::Player2 && Current_Sign == Sign::X))
        {DrawX(25, 24, 31, 30);
        NokiaScreen.drawCircle(28, 3, 3, 0);}
    NokiaScreen.drawLine(35, 16, 83, 16, 1);
    NokiaScreen.drawLine(35, 32, 83, 32, 1);
    NokiaScreen.drawLine(68, 0, 68, 47, 1);
    NokiaScreen.drawLine(52, 0, 52, 47, 1);   
    Counter = 0;       
}

void SetInitialPlayscreenBA()
{
    sprintf(Row1, "IG1");
    sprintf(Row2, "IG2");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 3);
    if(Current_Player == Player::Player1)
        {NokiaScreen.drawLine(0, 8, 31, 8, 1);
        NokiaScreen.drawLine(0, 32, 31, 32, 0);}
    if(Current_Player == Player::Player2)
        {NokiaScreen.drawLine(0, 8, 31, 8, 0);
        NokiaScreen.drawLine(0, 32, 31, 32, 1);}
    if((Current_Player == Player::Player1 && Current_Sign == Sign::X) || (Current_Player == Player::Player2 && Current_Sign == Sign::O))
        {DrawX(25, 0, 31, 6);
        NokiaScreen.drawCircle(28, 27, 3, 0);}
    if((Current_Player == Player::Player1 && Current_Sign == Sign::O) || (Current_Player == Player::Player2 && Current_Sign == Sign::X))
        {DrawX(25, 24, 31, 30);
        NokiaScreen.drawCircle(28, 3, 3, 0);}
    NokiaScreen.drawLine(35, 16, 83, 16, 1);
    NokiaScreen.drawLine(35, 32, 83, 32, 1);
    NokiaScreen.drawLine(68, 0, 68, 47, 1);
    NokiaScreen.drawLine(52, 0, 52, 47, 1);  
    Counter = 0;        
}

// prikazi rezultat - tri inacice
void ShowTheResultEN()
{
    Counter++;
    if(Last_Winner == Player::None)
        {sprintf(Row1, "Draw");
        sprintf(Row6, "# - Main menu");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Last_Winner == Player::Player1)
        {sprintf(Row1, "The winner is");
        sprintf(Row2, "Player 1");
        sprintf(Row6, "# - Main menu");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row6, 0, 5);
        }
    if(Last_Winner == Player::Player2)
        {sprintf(Row1, "The winner is");
        sprintf(Row2, "Player 2");
        sprintf(Row6, "# - Main menu");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Counter > 200)
        {Counter = 0;
        NokiaScreen.clear();
        Current_Phase = Phase::Standings;}      
}

void ShowTheResultDE()
{
    Counter++;
    if(Last_Winner == Player::None)
        {sprintf(Row1, "Unentschieden");
        sprintf(Row6, "# - Hauptmenue"); 
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Last_Winner == Player::Player1)
        {sprintf(Row1, "Der Sieger ist");
        sprintf(Row2, "Spieler 1");
        sprintf(Row6, "# - Hauptmenue"); 
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Last_Winner == Player::Player2)
        {sprintf(Row1, "Der Sieger ist");
        sprintf(Row2, "Spieler 2");
        sprintf(Row6, "# - Hauptmenue"); 
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Counter > 200)
        {Counter = 0;
        NokiaScreen.clear();
        Current_Phase = Phase::Standings;}         
}

void ShowTheResultBA()
{
    Counter++;
    if(Last_Winner == Player::None)
        {sprintf(Row1, "Nerijeseno");
        sprintf(Row6, "# Glavni meni");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Last_Winner == Player::Player1)
        {sprintf(Row1, "Pobjednik je");
        sprintf(Row2, "Igrac 1");
        sprintf(Row6, "# Glavni meni");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Last_Winner == Player::Player2)
        {sprintf(Row1, "Pobjednik je");
        sprintf(Row2, "Igrac 2");
        sprintf(Row6, "# Glavni meni");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row6, 0, 5);}
    if(Counter > 200)
        {Counter = 0;
        NokiaScreen.clear();
        Current_Phase = Phase::Standings;}      
}

// prikazi tabelu - tri inacice
void ShowTheStandingsEN()
{
    if(Player1_Wins >= Player2_Wins)
        {sprintf(Row1, "Standings:");
        sprintf(Row2, "Player 1");
        sprintf(Row3, "%d", Player1_Wins);
        sprintf(Row4, "Player 2");
        sprintf(Row5, "%d", Player2_Wins);
        sprintf(Row6, "# - Main menu");
        sprintf(Row7, "* - Exit menu");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.printString(Row6, 0, 5);
        if(Current_Phase == Phase::Standings)
            NokiaScreen.printString(Row7, 0, 4);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Player2_Wins > Player1_Wins)
        {sprintf(Row1, "Standings:");
        sprintf(Row2, "Player 2");
        sprintf(Row3, "%d", Player2_Wins);
        sprintf(Row4, "Player 1");
        sprintf(Row5, "%d", Player1_Wins);
        sprintf(Row6, "# - Main menu");
        sprintf(Row7, "* - Exit menu");        
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.printString(Row6, 0, 5);
        if(Current_Phase == Phase::Standings)
            NokiaScreen.printString(Row7, 0, 4);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}           
}

void ShowTheStandingsDE()
{
    if(Player1_Wins >= Player2_Wins)
        {sprintf(Row1, "Tabelle:");
        sprintf(Row2, "Spieler 1");
        sprintf(Row3, "%d", Player1_Wins);
        sprintf(Row4, "Spieler 2");
        sprintf(Row5, "%d", Player2_Wins);
        sprintf(Row6, "# - Hauptmenue");
        sprintf(Row7, "* - Ausgang");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.printString(Row6, 0, 5);
        if(Current_Phase == Phase::Standings)
            NokiaScreen.printString(Row7, 0, 4);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Player2_Wins > Player1_Wins)
        {sprintf(Row1, "Tabelle:");
        sprintf(Row2, "Spieler 2");
        sprintf(Row3, "%d", Player2_Wins);
        sprintf(Row4, "Spieler 1");
        sprintf(Row5, "%d", Player1_Wins);
        sprintf(Row6, "# - Hauptmenue");
        sprintf(Row7, "* - Ausgang");        
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.printString(Row6, 0, 5);
        if(Current_Phase == Phase::Standings)
            NokiaScreen.printString(Row7, 0, 4);       
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}       
}

void ShowTheStandingsBA()
{
    if(Player1_Wins >= Player2_Wins)
        {sprintf(Row1, "Tabela:");
        sprintf(Row2, "Igrac 1");
        sprintf(Row3, "%d", Player1_Wins);
        sprintf(Row4, "Igrac 2");
        sprintf(Row5, "%d", Player2_Wins);
        sprintf(Row6, "# Glavni meni");
        sprintf(Row7, "* Izlazni meni");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.printString(Row6, 0, 5);
        if(Current_Phase == Phase::Standings)
            NokiaScreen.printString(Row7, 0, 4);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Player2_Wins > Player1_Wins)
        {sprintf(Row1, "Tabela:");
        sprintf(Row2, "Igrac 2");
        sprintf(Row3, "%d", Player2_Wins);
        sprintf(Row4, "Igrac 1");
        sprintf(Row5, "%d", Player1_Wins);
        sprintf(Row6, "# - Glavni meni");
        sprintf(Row7, "* - Izlazni meni");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.printString(Row6, 0, 5);
        if(Current_Phase == Phase::Standings)
            NokiaScreen.printString(Row7, 0, 4);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}     
}

// igraj ponovo ili zavrsi igru - tri inacice
void PlayAgainEN()
{
    sprintf(Row1, "Would you ");
    sprintf(Row2, "like to");
    sprintf(Row3, "play again?");
    sprintf(Row4, "0 - NO");
    sprintf(Row5, "1 - YES");
    sprintf(Row6, "# - Main menu");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row5, 0, 4);
    NokiaScreen.printString(Row6, 0, 5);
    Counter = 0;
}

void PlayAgainDE()
{
    sprintf(Row1, "Moechten Sie ");
    sprintf(Row2, "wiederspielen?");
    sprintf(Row3, "0 - NEIN");
    sprintf(Row4, "1 - JA");
    sprintf(Row6, "# - Hauptmenue");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row6, 0, 5);
    Counter = 0;   
}

void PlayAgainBA()
{
    sprintf(Row1, "Zelite li ");
    sprintf(Row2, "igrati ponovo");
    sprintf(Row3, "0 - NE");
    sprintf(Row4, "1 - DA");
    sprintf(Row6, "# Glavni meni");
    NokiaScreen.printString(Row1, 0, 0);
    NokiaScreen.printString(Row2, 0, 1);
    NokiaScreen.printString(Row3, 0, 2);
    NokiaScreen.printString(Row4, 0, 3);
    NokiaScreen.printString(Row6, 0, 5);
    Counter = 0;  
}

// kraj - tri inacice
void TheEndEN()
{
    if(Player1_Wins >= Player2_Wins && Counter < 200)
        {sprintf(Row1, "Standings:");
        sprintf(Row2, "Player 1");
        sprintf(Row3, "%d", Player1_Wins);
        sprintf(Row4, "Player 2");
        sprintf(Row5, "%d", Player2_Wins);
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Player2_Wins > Player1_Wins && Counter < 200)
        {sprintf(Row1, "Standings:");
        sprintf(Row2, "Player 2");
        sprintf(Row3, "%d", Player2_Wins);
        sprintf(Row4, "Player 1");
        sprintf(Row5, "%d", Player1_Wins);
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Counter >= 200)
        {sprintf(Row1, "Please");
        sprintf(Row2, "turn off");
        sprintf(Row3, "your device");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 0, 2);}        
    Counter++;
    if(Counter == 200)
        NokiaScreen.clear();
    if(Counter == 400)
        {NokiaScreen.clear();
        Counter = 0;}               
}

void TheEndDE()
{
    if(Player1_Wins >= Player2_Wins && Counter < 200)
        {sprintf(Row1, "Tabelle:");
        sprintf(Row2, "Spieler 1");
        sprintf(Row3, "%d", Player1_Wins);
        sprintf(Row4, "Spieler 2");
        sprintf(Row5, "%d", Player2_Wins);
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Player2_Wins > Player1_Wins && Counter < 200)
        {sprintf(Row1, "Tabelle:");
        sprintf(Row2, "Spieler 2");
        sprintf(Row3, "%d", Player2_Wins);
        sprintf(Row4, "Spieler 1");
        sprintf(Row5, "%d", Player1_Wins);
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Counter >= 200)
        {sprintf(Row1, "Schalten Sie ");
        sprintf(Row2, "bitte Ihr ");
        sprintf(Row3, "Geraet aus");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 0, 2);}        
    Counter++;
    if(Counter == 200)
        NokiaScreen.clear();
    if(Counter == 400)
        {NokiaScreen.clear();
        Counter = 0;}  
}

void TheEndBA()
{
    if(Player1_Wins >= Player2_Wins && Counter < 200)
        {sprintf(Row1, "Tabela:");
        sprintf(Row2, "Igrac 1");
        sprintf(Row3, "%d", Player1_Wins);
        sprintf(Row4, "Igrac 2");
        sprintf(Row5, "%d", Player2_Wins);
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Player2_Wins > Player1_Wins && Counter < 200)
        {sprintf(Row1, "Tabela:");
        sprintf(Row2, "Igrac 2");
        sprintf(Row3, "%d", Player2_Wins);
        sprintf(Row4, "Igrac 1");
        sprintf(Row5, "%d", Player1_Wins);
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 60, 1);
        NokiaScreen.printString(Row4, 0, 2);
        NokiaScreen.printString(Row5, 60, 2);
        NokiaScreen.drawLine(57, 8, 57, 24, 1);}
    if(Counter >= 200)
        {sprintf(Row1, "Molimo");
        sprintf(Row2, "iskljucite");
        sprintf(Row3, "Vas uredjaj");
        NokiaScreen.printString(Row1, 0, 0);
        NokiaScreen.printString(Row2, 0, 1);
        NokiaScreen.printString(Row3, 0, 2);}        
    Counter++;
    if(Counter == 200)
        NokiaScreen.clear();
    if(Counter == 400)
        {NokiaScreen.clear();
        Counter = 0;}  
}

void PrintScreen() // svi ispisi
{
    if(Current_Phase == Phase::Initialisation)
        Initialisation();

    if(Current_Phase == Phase::ChooseLanguage && Current_Language == Language::English)
        ChooseLanguageEN();
    if(Current_Phase == Phase::ChooseLanguage && Current_Language == Language::German)
        ChooseLanguageDE();
    if(Current_Phase == Phase::ChooseLanguage && Current_Language == Language::Bosnian)
        ChooseLanguageBA();

    if(Current_Phase == Phase::MainMenu && Current_Language == Language::English)
        MainMenuEN();
    if(Current_Phase == Phase::MainMenu && Current_Language == Language::German)
        MainMenuDE();
    if(Current_Phase == Phase::MainMenu && Current_Language == Language::Bosnian)
        MainMenuBA();

    if(Current_Phase == Phase::ChooseSign && Current_Language == Language::English)
        ChooseSignEN();    
    if(Current_Phase == Phase::ChooseSign && Current_Language == Language::German)
        ChooseSignDE();   
    if(Current_Phase == Phase::ChooseSign && Current_Language == Language::Bosnian)
        ChooseSignBA();

    if(Current_Phase == Phase::Play && Current_Language == Language::English)
        {SetInitialPlayscreenEN();}
    if(Current_Phase == Phase::Play && Current_Language == Language::German)
        {SetInitialPlayscreenDE();}
    if(Current_Phase == Phase::Play && Current_Language == Language::Bosnian)
        {SetInitialPlayscreenBA();}

    if(Current_Phase == Phase::Result && Current_Language == Language::English)
        ShowTheResultEN();   
    if(Current_Phase == Phase::Result && Current_Language == Language::German)
        ShowTheResultDE();  
    if(Current_Phase == Phase::Result && Current_Language == Language::Bosnian)
        ShowTheResultBA();

    if(Current_Phase == Phase::Standings && Current_Language == Language::English)
        ShowTheStandingsEN();
    if(Current_Phase == Phase::Standings && Current_Language == Language::German)
        ShowTheStandingsDE(); 
    if(Current_Phase == Phase::Standings && Current_Language == Language::Bosnian)
        ShowTheStandingsBA();

    if(Current_Phase == Phase::Repeat && Current_Language == Language::English)
        PlayAgainEN();
    if(Current_Phase == Phase::Repeat && Current_Language == Language::German)
        PlayAgainDE(); 
    if(Current_Phase == Phase::Repeat && Current_Language == Language::Bosnian)
        PlayAgainBA();

    if(Current_Phase == Phase::End && Current_Language == Language::English)
        TheEndEN();
    if(Current_Phase == Phase::End && Current_Language == Language::German)
        TheEndDE();  
    if(Current_Phase == Phase::End && Current_Language == Language::Bosnian)
        TheEndBA();

    if(Current_Phase == Phase::Reset && Current_Language == Language::English)
        {Player1_Wins = 0;
        Player2_Wins = 0;
        Current_Player = Player::Player1;
        ShowTheStandingsEN();
        sprintf(Row8, "Reset");
        sprintf(Row7, "successiful");
        NokiaScreen.printString(Row8, 0, 3);
        NokiaScreen.printString(Row7, 0, 4);}
    if(Current_Phase == Phase::Reset && Current_Language == Language::German)
        {Player1_Wins = 0;
        Player2_Wins = 0;
        Current_Player = Player::Player1;
        ShowTheStandingsDE();
        sprintf(Row8, "Zuruecksetzung");
        sprintf(Row7, "erfolgreich");
        NokiaScreen.printString(Row8, 0, 3);
        NokiaScreen.printString(Row7, 0, 4);}
    if(Current_Phase == Phase::Reset && Current_Language == Language::Bosnian)
        {Player1_Wins = 0;
        Player2_Wins = 0;
        Current_Player = Player::Player1;
        ShowTheStandingsBA();
        sprintf(Row8, "Reset");
        sprintf(Row7, "uspjesan");
        NokiaScreen.printString(Row8, 0, 3);
        NokiaScreen.printString(Row7, 0, 4);}                  
}

int main() {
    Debounce.start();
    NokiaScreen.init();
    ScreenPrint.attach(&PrintScreen, 10ms); // osvjezava ispis svakih 10 ms
    Keyboard.attach(&Push, 1ms);
    while (1) {}
}
