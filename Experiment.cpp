#include <bits\stdc++.h>
#include <windows.h>
#include <conio.h>
#include <thread>
#include <mmsystem.h>
#pragma comment(lib,"winmm.lib")
#define IMG_LENGTH 400
#define MAP_X 179
#define MAP_Y 41
#define DATALEN 10000
#define BULLET_LIST_SIZE 1000
#define ENTITY_LIST_SIZE 10
using namespace std;

int planeX, planeY, tankX, tankY, cmdX, cmdY, sunX, sunY;
int friendyMap[MAP_Y][MAP_X], enemyMap[MAP_Y][MAP_X], itemMap[MAP_Y][MAP_X];
int tick, shootTick, skillCD, strengthTick, laserTick,sleepTick;
int score;
bool endGame, canExecute, inAnimation, isInBossFight, disableEnemyEvent_All, isPlaneDead, isTankDead;
double difficulty = 1;

enum type {
	FRIENDY_PLANE = 100,
	ENEMY_PLANE = 200,
	FRIENDY_TANK = 300,
	ENEMY_BOSS = 500,
	FRIENDY_BULLET = 1000,
	ENEMY_BULLET = 2000,
	ITEM_STRENGTH = 3000,
	ITEM_LASER = 3001
};

struct bullet {
	int x, y, type, direction;
};
bullet bullets[BULLET_LIST_SIZE];
struct entity {
	int x, y, type, health, maxHealth,jump,jumpDirection;
};
entity entities[ENTITY_LIST_SIZE];
entity aqua;

//颜色: A淡蓝色 a深蓝色 B绿色 b深绿色 C亮红色 c深红色 D天蓝色 d蓝色 E金色 e棕色 F粉色 f紫色        k强制渲染该字符
char planeO[5][IMG_LENGTH] = {
	"  \\    --b----",
	"  =\\-  --ABC--",
	"=====--AaBbCcc",
	"   =\\- ---ABC-",
	"     \\ -----b-"
};
char plane1[5][IMG_LENGTH] = {
	"     / ccccccc",
	"   -/= ccccccc",
	"--=====ccccccc",
	" -/=   ccccccc",
	" /     ccccccc"
};
char boom[3][IMG_LENGTH] = {
	"  ^^^^  cccccccc",
	"< BOOM >cCCCCCCc",
	"  vvvv  cccccccc"
};
char colorTest[1][IMG_LENGTH] = {
	"A a B b C c D d E e F f A-a-B-b-C-c-D-d-E-e-F-f-",
};
char colorTestB[1][IMG_LENGTH] = {
	"AaBbCcDdEeFf",
};
char colorTestRGB[1][IMG_LENGTH] = {
	"ABCDEFGHIJKL",
};
char bulletO_1[1][IMG_LENGTH] = {
	"->aA",
};
char bulletLaser[1][IMG_LENGTH] = {
	"---------------DDDDDDDDDDDDDDD",
};
char bulletE_1[1][IMG_LENGTH] = {
	"<-Cc",
};
char itemStrength[1][IMG_LENGTH] = {
	"STBB",
};
char itemLaser[1][IMG_LENGTH] = {
	"LAAA",
};
char ground[1][IMG_LENGTH] = {
	"~~```~`~``~``#````~`~~~`~~`~~`~~`~~~~~`#`~`~`````~~~~``#`~`~```````#`~`~``````~`~~~`~~`#`#`~`~`````~~~~``#`~`~```````#`~`~``````~`~~~`~~`###~``````~`~``````~`~~~`~#~~~``#`~`~``````#~~``~```~``````~`~~~`~~#`~~`~~#~"
};
char sun1[6][IMG_LENGTH] = {
	"   \\_____/    ccccccccccccc",
	"   /     \\   ccccccccccccc",
	"__/       \\__ccccccccccccc",
	"  \\       /   ccccccccccccc",
	"   \\_____/    ccccccccccccc",
	"   /     \\   ccccccccccccc",
};
char sun2[6][IMG_LENGTH] = {
	"   \\_____/    CCECCCCCECCCC",
	"   /     \\   CCCCCCCCCCCCC",
	"__/       \\__EECCCCCCCCCEE",
	"  \\       /   CCCCCCCCCCCCC",
	"   \\_____/    CCCCCCCCCCCCC",
	"   /     \\   CCCECCCCCECCC",
};
char onion[35][IMG_LENGTH] = {//35*33//性能问题  很悲伤
	"-----------------------D---------",
	"------------------------DD-------",
	"------------------WWWW---D-------",
	"---------------WW-WWaWWGWD-------",
	"-------------WWWWGWaaaWGWD-------",
	"-------------WWWWGaWaWaGWDG------",
	"-----------WGGWWWGGaaaGGDWG------",
	"----------WWWGGWFFFDFDFDWWGG-----",
	"----------WWWGfFFFDFFFDFfGGG-----",
	"----------WWWfFFFFDFFFDFFfGG-----",
	"----------fWfFFFFFDFFFDFFffG-----",
	"---AAAAA--fWfFFFFFDFFFDFFffAAA---",
	"----AAAAA-fffFFFeFDFFFDeFffAA----",
	"-----AAaAAfFfFFFeFDFFFDeFffAA----",
	"------AAaaFffFFFeFFFFDFeFffAf----",
	"------ffaafFeF----eeee---ffAAf---",
	"-----FFAAaFfe--WWf-ee-Wf-ffAAff--",
	"----FFAAaAfewF-WfffRRRff-ffAAff--",
	"---DFFAAAd-ewFewFFFRRRFFeffAAfff-",
	"--DDFFAAdd-feFFwwwwwwwwwef-dAfff-",
	"FFDDFFA-dd--fFFwwwwwwwww-fdddfffd",
	"FFDDFF-dDD----Fewwweeww-ffdd-ffdd",
	"FFD-FFFDD----AFaWaaaWa--f-dddffdd",
	"FFD--FFFF----AAAwwwweWa----dfffdd",
	"FFF---FF----WAAWWWWWwaaw-------dd",
	"FFF---------wWWAWWWWwwwe-------df",
	"-FF-------Wwww-AAAwaa-eeww-----ff",
	"--F------wwWw-AAAAAAaa-weee---ff-",
	"----------ww-WAAAAAAaaw-ee-------",
	"------------WWwWWwwWwWww---------",
	"--------------wwWWwWee-----------",
	"-------------www----ee-----------",
	"------------www-----ee-----------",
	"------------ww------ww-----------",
	"------------AA------aa-----------",
};
char trueAqua[13][IMG_LENGTH] = {
	"-----f------",
	"----fFfff---",
	"----fffF----",
	"---fffff----",
	"--fffFfFf---",
	"-fFfFFFfff--",
	"fFFfFFFfFFf-",
	"fFffFFFffFFf",
	"fFffFFFFfFFf",
	"fFFfFFFFfFFf",
	"-fFfFFFffFf-",
	"--ffFFFfFf--",
	"---ffffff---",
};
char mountain[9][IMG_LENGTH] = {
	"        /\\        ",
	"       /  \\       ",
	"      /    \\      ",
	"     /      \\     ",
	"    /        \\    ",
	"   /          \\   ",
	"  /            \\  ",
	" /              \\ ",
	"/                \\",
};

void enterAnime();
void clearBullet();
void killBullet(int x, int y);
void bulletGenerator(int x, int y, int type, int direction);
void checkBullet();
void checkExecute();
void gameInfo();
void render(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second, int type);
void enemyEvent();
void printMap();
void setRandomSeed();
void gotoXY(short x, short y, HANDLE h);
void getCmdXY();
void hideCursor();
void ScreenBuff();
void bufferOn();
void bufferOff();
void updateScreen();
void clearScreen();
void renderAll();
void setDefaultSettings();
void resetAllMap();
void enemyReborn();
void characterPlaneReborn();
void skill_1();
void renderGround(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second, int startPos, int renderLength);
void renderMountain(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second);
void playBgm(string cmd, string bgmName);
void setConsoleColor(char color);
int random(int min, int max);
int getBullet(int type);
int checkCrush();
LPCWSTR charToLPCWSTR(const char* charArray);

HANDLE hOutput;
HANDLE hOutBuf;
DWORD bytes;
LPWORD lpWord = new WORD[DATALEN];
bool isOn;
char dataA[DATALEN];
void ScreenBuff() {
	bytes = 0;
	hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
	hOutBuf = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
	);
	CONSOLE_CURSOR_INFO cci;
	cci.bVisible = 0;
	cci.dwSize = 1;
	SetConsoleCursorInfo(hOutput, &cci);
	SetConsoleCursorInfo(hOutBuf, &cci);
}
void bufferOn() {//开启双缓冲后，在输出后必须调用update才能显示 
	isOn = true;
	SetConsoleActiveScreenBuffer(hOutBuf);
}
void bufferOff() {//关闭双缓冲 
	isOn = false;
	SetConsoleActiveScreenBuffer(hOutput);
}
void updateScreen() {//更新屏幕 
	if (isOn) {
		COORD coord = { 0,0 };
		DWORD recnum;
		CONSOLE_SCREEN_BUFFER_INFO cinfo;//用于储存控制台缓冲区信息,在这里主要获取控制台缓冲区大小
		GetConsoleScreenBufferInfo(hOutput, &cinfo);//cinfo.dwSize储存的是缓冲区大小//cinfo.dwSize.X * cinfo.dwSize.Y 即需填充的字符数
		ReadConsoleOutputCharacterA(hOutput, dataA, DATALEN, coord, &bytes);//从控制台缓冲区读取数据
		WriteConsoleOutputCharacterA(hOutBuf, dataA, DATALEN, coord, &bytes);//将数据写入双缓冲区
		ReadConsoleOutputAttribute(hOutput, lpWord, DATALEN, coord, &bytes);
		WriteConsoleOutputAttribute(hOutBuf, lpWord, DATALEN, coord, &bytes);
		FillConsoleOutputCharacterW(hOutput, ' ', cinfo.dwSize.X * cinfo.dwSize.Y, coord, &recnum);//从{0,0}处开始填充' '字符,成功填充个数为recnum
		FillConsoleOutputAttribute(hOutput, 0, cinfo.dwSize.X * cinfo.dwSize.Y, coord, &recnum);//设置输出颜色,如果不是单一颜色,可能会有清除字符而背景色没有清除的现象
	}
}
void clearScreen() {//清空屏幕 
	CONSOLE_SCREEN_BUFFER_INFO cinfo;//用于储存控制台缓冲区信息,在这里主要获取控制台缓冲区大小
	DWORD recnum;
	COORD coord = { 0,0 };
	GetConsoleScreenBufferInfo(hOutput, &cinfo);//cinfo.dwSize储存的是缓冲区大小//cinfo.dwSize.X * cinfo.dwSize.Y 即需填充的字符数
	FillConsoleOutputCharacterW(hOutput, L' ', cinfo.dwSize.X * cinfo.dwSize.Y, coord, &recnum);//从{0,0}处开始填充' '字符,成功填充个数为recnum
	FillConsoleOutputAttribute(hOutput, 0, cinfo.dwSize.X * cinfo.dwSize.Y, coord, &recnum);//设置输出颜色,如果不是单一颜色,可能会有清除字符而背景色没有清除的现象
	SetConsoleCursorPosition(hOutput, coord);//将光标设为{0,0}
	updateScreen();
}

void hideCursor() {
	CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

void gotoXY(short x, short y, HANDLE h) {
	COORD coord = { x, y };
	SetConsoleCursorPosition(h, coord);
}

void getCmdXY() {
	CONSOLE_SCREEN_BUFFER_INFO bInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &bInfo);
	cmdX = bInfo.dwSize.X;
	//cmdY = bInfo.dwSize.Y;
	cmdY = MAP_Y;
}

void printMap() {//调试用
	cout << endl;
	for (int i = 0; i < MAP_Y; ++i) {
		for (int j = 0; j < MAP_X; ++j) {
			cout << friendyMap[i][j];
		}
		cout << endl;
	}
	cout << endl;
	for (int i = 0; i < MAP_Y; ++i) {
		for (int j = 0; j < MAP_X; ++j) {
			cout << enemyMap[i][j];
		}
		cout << endl;
	}
}

void setRandomSeed() {//设置随机种子
	srand((unsigned)time(NULL));
}

int random(int min, int max) {//生成[min,max]的随机数
	return rand() % (max - min + 1) + min;
}

void resetAllMap() {
	memset(friendyMap, 0, sizeof(friendyMap));
	memset(enemyMap, 0, sizeof(enemyMap));
	memset(itemMap, 0, sizeof(itemMap));
}

void enterAnime() {//进入入场动画
	canExecute = false;
	inAnimation = true;
	memset(friendyMap, 0, sizeof(friendyMap));
	memset(enemyMap, 0, sizeof(enemyMap));
	planeX = 0;
	planeY = 3;
	for (int i = -(int)strlen(planeO[0]); i <= 3; ++i) {
		planeX = i;
		Sleep(50);
	}
	canExecute = true;
	inAnimation = false;
}

LPCWSTR charToLPCWSTR(const char* charArray) {
	WCHAR* wString;
	int len = MultiByteToWideChar(CP_ACP, 0, charArray, -1, NULL, 0);
	wString = new WCHAR[len];
	MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, len);
	return wString;
}

void playBgm(string cmd, string bgmName) {
	string fCmd = "";
	ifstream ifileRelease(".\\res\\" + bgmName);
	ifstream ifileDebug(".\\x64\\Debug\\res\\" + bgmName);
	if (ifileRelease) {
		fCmd = cmd + " .\\res\\" + bgmName;
	}
	else if (ifileDebug) {
		fCmd = cmd + " .\\x64\\Debug\\res\\" + bgmName;
	}
	else {
		cout << bgmName << " not found！" << endl;
	}
	mciSendString(charToLPCWSTR(fCmd.c_str()), NULL, 0, NULL);
}

void renderGround(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second, int startPos, int renderLength) {
	if (baseX >= cmdX || baseY < 0) {//超出屏幕范围
		return;
	}
	if (baseX + renderLength > cmdX) {
		renderLength = cmdX - baseX;
	}
	for (int i = 0; i < first; ++i) {
		for (int j = 0; j < renderLength; ++j) {
			gotoXY(((baseX + j > 0) ? (baseX + j) : 0), baseY + i, hOutput);
			cout << img[i][(j + startPos) % renderLength];
		}
	}
}

void renderMountain(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second) {
	if (baseX >= cmdX || baseY < 0) {//超出屏幕范围
		return;
	}
	int l = 0, r = second;
	if (baseX + r > cmdX) {
		r = cmdX - baseX;
	}
	if (baseX < 0) {
		l = -baseX;
		if (l > second) {
			return;
		}
	}
	for (int i = 0; i < first; ++i) {
		for (int j = l; j < r; ++j) {
			if (img[i][j] != ' ') {
				gotoXY(((baseX + j > 0) ? (baseX + j) : 0), baseY + i, hOutput);
				if (img[i][j] == '/' && i<first - 2 && baseX + j > sunX + 6) {
					SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				else if (img[i][j] == '\\' && i < first - 2 && baseX + j < sunX + 6) {
					SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
				else {
					SetConsoleTextAttribute(hOutput, FOREGROUND_BLUE);
				}
				cout << img[i][j];
				SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
		}
	}
}

void skill_1() {
	if (skillCD < tick) {
		int tempX = planeX + 3, tempY = planeY + 2;
		skillCD = tick + 300;
		for (int i = -2; i <= 2; ++i) {
			bulletGenerator(tempX, tempY + abs(2 + i), FRIENDY_BULLET, 1);
			bulletGenerator(tempX, tempY - abs(2 + i), FRIENDY_BULLET, 1);
			Sleep(100);
		}
		bulletGenerator(tempX, tempY + 5, FRIENDY_BULLET, 1);
		bulletGenerator(tempX, tempY - 5, FRIENDY_BULLET, 1);
		Sleep(100);
		for (int i = -2; i <= 2; ++i) {
			bulletGenerator(tempX, tempY + abs(2 - i), FRIENDY_BULLET, 1);
			bulletGenerator(tempX, tempY - abs(2 - i), FRIENDY_BULLET, 1);
			Sleep(100);
		}
	}
}

void setConsoleColor(char color) {
	if (color == 'A') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	}
	else if (color == 'B') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
	else if (color == 'C') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (color == 'D') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
	else if (color == 'E') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
	else if (color == 'F') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_BLUE | FOREGROUND_RED | FOREGROUND_INTENSITY);
	}
	else if (color == 'a') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_BLUE);
	}
	else if (color == 'b') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_GREEN);
	}
	else if (color == 'c') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_RED);
	}
	else if (color == 'd') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_BLUE | FOREGROUND_GREEN);
	}
	else if (color == 'e') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN);
	}
	else if (color == 'f') {
		SetConsoleTextAttribute(hOutput, FOREGROUND_BLUE | FOREGROUND_RED);
	}
	else {
		SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	}
}

void setConsoleColorBackground(char color) {
	if (color == 'A') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_BLUE | BACKGROUND_INTENSITY);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0x3B, 0x78, 0xFF);
	}
	else if (color == 'B') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_GREEN | BACKGROUND_INTENSITY);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0x16, 0xC6, 0x0C);
	}
	else if (color == 'C') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_RED | BACKGROUND_INTENSITY);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0xE7, 0x48, 0x56);
	}
	else if (color == 'D') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0x61, 0xD6, 0xD6);
	}
	else if (color == 'E') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0xF9, 0xF1, 0xA5);
	}
	else if (color == 'F') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0xB4, 0x00, 0x9E);
	}
	else if (color == 'a') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_BLUE);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0x00, 0x37, 0xDA);
	}
	else if (color == 'b') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_GREEN);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0x13, 0xA1, 0x0E);
	}
	else if (color == 'c') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_RED);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0xC5, 0x0F, 0x1F);
	}
	else if (color == 'd') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_BLUE | BACKGROUND_GREEN);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0x3A, 0x96, 0xDD);
	}
	else if (color == 'e') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_RED | BACKGROUND_GREEN);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0xC1, 0x9C, 0x00);
	}
	else if (color == 'f') {
		//SetConsoleTextAttribute(hOutput, BACKGROUND_BLUE | BACKGROUND_RED);
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0x88, 0x17, 0x98);
	}
	else if (color == 'W') {
		wprintf(L"\x1b[48;2;%d;%d;%dm", 0xF0, 0xF8, 0xF8);
	}
	else {
		SetConsoleTextAttribute(hOutput, BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
	}
}

//应该修改对碰撞箱的渲染，为类型附加下标，以便检测碰撞时确定是哪个实体
void render(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second, int type) {//渲染的x,y坐标，渲染目标，目标的第一维，目标的第二维，目标的类型
	if (baseX >= cmdX || baseY < 0) {//超出屏幕范围
		return;
	}
	second /= 2;
	int l = 0, r = second, colorStart = second;
	if (baseX + r > cmdX) {
		r = cmdX - baseX;
	}
	if (baseX < 0) {
		l = -baseX;
		if (l > second) {
			return;
		}
	}
	for (int i = 0; i < first; ++i) {
		for (int j = l; j < r; ++j) {
			if (img[i][j] != ' ' || img[i][j + colorStart] == 'k') {
				gotoXY(((baseX + j > 0) ? (baseX + j) : 0), baseY + i, hOutput);
				if (img[i][j + colorStart] != '-') {
					setConsoleColor(img[i][j + colorStart]);
				}
				cout << img[i][j];
				SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
			}
		}
	}
	if (type == 0) {
		return;
	}
	if (type == ENEMY_PLANE) {
		gotoXY(((baseX > 0) ? baseX : 0), baseY + first, hOutput);
		cout << " " << entities[0].health << "/" << entities[0].maxHealth;
	}
	for (int i = baseY; i < baseY + first; ++i) {
		for (int j = baseX + l; j < baseX + r; ++j) {
			if (i < 0 || j < 0 || i >= MAP_Y || j >= MAP_X) {//超出地图范围
				continue;
			}
			if (img[i - baseY][j - baseX] == ' ' && img[i - baseY][j - baseX + colorStart] != 'k') {
				continue;
			}
			if (type == FRIENDY_PLANE) {
				friendyMap[i][j] = type;
			}
			else if (type == ENEMY_PLANE) {
				enemyMap[i][j] = type;
			}
			else if (type == FRIENDY_BULLET) {
				friendyMap[i][j] = type;
			}
			else if (type == ENEMY_BULLET) {
				enemyMap[i][j] = type;
			}
			else if (type == ITEM_STRENGTH) {
				itemMap[i][j] = type;
			}
			else if (type == ITEM_LASER) {
				itemMap[i][j] = type;
			}
		}
	}
}

void renderAqua(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second, int type) {
	if (baseX >= cmdX || baseY < 0) {//超出屏幕范围
		return;
	}
	//second /= 2;
	int l = 0, r = second, colorStart = second;
	if (baseX + r > cmdX) {
		r = cmdX - baseX;
	}
	if (baseX < 0) {
		l = -baseX;
		if (l > second) {
			return;
		}
	}
	for (int i = 0; i < first; ++i) {
		for (int j = l; j < r; ++j) {
			//if (img[i][j] != ' ' || img[i][j + colorStart] == 'k') {
				if (img[i][j] != '-') {
					gotoXY(((baseX + j > 0) ? (baseX + j) : 0) * 2, baseY + i, hOutput);
					setConsoleColorBackground(img[i][j]);
					cout << "  ";
					SetConsoleTextAttribute(hOutput, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
				}
			//}
		}
	}
	if (type == 0) {
		return;
	}
	for (int i = baseY; i < baseY + first; ++i) {
		for (int j = baseX + l; j < baseX + r; ++j) {
			if (i < 0 || j*2 < 0 || i >= MAP_Y || j*2 >= MAP_X) {//超出地图范围
				continue;
			}
			if (img[i - baseY][j - baseX] == '-') {
				continue;
			}
			if (type == ENEMY_BOSS) {
				enemyMap[i][j*2] = type;
			}
		}
	}
}

void characterPlaneReborn() {
	isPlaneDead = true;
	canExecute = false;
	Sleep(1000);
	isPlaneDead = false;
	thread t1(enterAnime);
	t1.detach();
	canExecute = true;
}

void enemyReborn() {
	entities[0].health = 0;
	if (isInBossFight) {
		Sleep(1100);
		entities[0].health = 0;
		return;
	}
	Sleep(1000);
	entities[0].x = 100;
	entities[0].y = 6;
	entities[0].health = (int)(8 * difficulty);
	entities[0].maxHealth = (int)(8 * difficulty);
}

int checkCrush() {//0:无碰撞 1:友军敌军碰撞 2:友军被击中 3:敌军被击中
	for (int i = 0; i < MAP_Y; ++i) {
		for (int j = 0; j < MAP_X; ++j) {
			if (friendyMap[i][j] == FRIENDY_PLANE && (enemyMap[i][j] == ENEMY_PLANE||enemyMap[i][j]==ENEMY_BOSS)) {
				difficulty += 1;
				thread T1(enemyReborn);
				thread T2(characterPlaneReborn);
				T1.detach();
				T2.detach();
				clearBullet();
				return 1;
			}
			else if (friendyMap[i][j] == FRIENDY_PLANE && enemyMap[i][j] == ENEMY_BULLET) {
				difficulty += 0.1;
				thread T1(characterPlaneReborn);
				T1.detach();
				clearBullet();
				return 2;
			}
			else if (friendyMap[i][j] == FRIENDY_BULLET && enemyMap[i][j] == ENEMY_PLANE) {
				entities[0].health -= 1;
				killBullet(j - 1, i);
				if (entities[0].health <= 0) {
					++score;
					difficulty *= 1.15;
					clearBullet();
					thread T1(enemyReborn);
					T1.detach();
					if (rand() % 4 == 0) {
						bulletGenerator(j + 3, i + 3, ITEM_LASER, 1);
					}
					else {
						bulletGenerator(j + 3, i + 3, ITEM_STRENGTH, 1);
					}
					return 3;
				}
			}
			else if (friendyMap[i][j] == FRIENDY_BULLET && enemyMap[i][j] == ENEMY_BOSS) {
				aqua.health -= 1;
				killBullet(j - 1, i);
				if (laserTick==0&&rand() % 60 == 0) {
					if (rand() % 4 == 0) {
						bulletGenerator(rand()%cmdX, 0, ITEM_LASER, 1);
					}
					else {
						bulletGenerator(rand() % cmdX, 0, ITEM_STRENGTH, 1);
					}
				}
				else if (laserTick) {
					laserTick -= 1;
				}
			}
			else if (friendyMap[i][j] == FRIENDY_PLANE && itemMap[i][j] == ITEM_STRENGTH) {
				strengthTick += 5;
				killBullet(j, i);
			}
			else if (friendyMap[i][j] == FRIENDY_PLANE && itemMap[i][j] == ITEM_LASER) {
				laserTick += 100;
				killBullet(j, i);
			}
		}
	}
	return 0;
}

void bulletGenerator(int x, int y, int type, int direction) {//子弹生成器
	if (x < 0 || y < 0 || x >= MAP_X || y >= MAP_Y) {
		return;
	}
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].type == 0) {
			bullets[i].x = x;
			bullets[i].y = y;
			bullets[i].type = type;
			bullets[i].direction = direction;
			return;
		}
	}
}

void checkExecute() {//检查是否有输入
	if (!canExecute || inAnimation) {
		return;
	}
	if (_kbhit()) {
		if (GetAsyncKeyState('W') && planeY > 0 && tick % 2 == 0) {
			planeY -= 1;
		}
		if (GetAsyncKeyState('S') && planeY + 5 < MAP_Y && tick % 2 == 0) {
			planeY += 1;
		}
		if (GetAsyncKeyState('A') && tick % 2 == 0) {
			planeX -= 2;
		}
		if (GetAsyncKeyState('D') && tick % 2 == 0) {
			planeX += 2;
		}
		if (GetAsyncKeyState(' ') && tick % 2 == 0) {
			if (tick % 4 == 0) {
				bulletGenerator(planeX + 3, planeY + 2, FRIENDY_BULLET, 1);
			}
			if (strengthTick > 0 && tick % 8 == 0) {
				--strengthTick;
				bulletGenerator(planeX + 3 - 1, planeY + 2 - 1, FRIENDY_BULLET, 1);
				bulletGenerator(planeX + 3 - 1, planeY + 2 + 1, FRIENDY_BULLET, 1);
			}
		}
		if (GetAsyncKeyState('J')) {
			thread Tskill_1(skill_1);
			Tskill_1.detach();
		}
	}
}

void checkBullet() {//检查子弹
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].type == FRIENDY_BULLET) {
			bullets[i].x += 1 * bullets[i].direction;
			if (bullets[i].x >= MAP_X || bullets[i].x < 0) {
				bullets[i].type = 0;
				continue;
			}
			render(bullets[i].x, bullets[i].y, bulletO_1, 1, (int)strlen(bulletO_1[0]), FRIENDY_BULLET);
		}
		else if (bullets[i].type == ENEMY_BULLET) {
			bullets[i].x += 1 * bullets[i].direction;
			if (bullets[i].x >= MAP_X || bullets[i].x < 0) {
				bullets[i].type = 0;
				continue;
			}
			render(bullets[i].x, bullets[i].y, bulletE_1, 1, (int)strlen(bulletE_1[0]), ENEMY_BULLET);
		}
		else if (bullets[i].type == ITEM_STRENGTH) {
			if (tick % 13 == 0) {
				bullets[i].y += 1 * bullets[i].direction;
			}
			if (bullets[i].y >= MAP_Y || bullets[i].y < 0) {
				bullets[i].type = 0;
				continue;
			}
			render(bullets[i].x, bullets[i].y, itemStrength, 1, (int)strlen(itemStrength[0]), ITEM_STRENGTH);
		}
		else if (bullets[i].type == ITEM_LASER) {
			if (tick % 13 == 0) {
				bullets[i].y += 1 * bullets[i].direction;
			}
			if (bullets[i].y >= MAP_Y || bullets[i].y < 0) {
				bullets[i].type = 0;
				continue;
			}
			render(bullets[i].x, bullets[i].y, itemLaser, 1, (int)strlen(itemLaser[0]), ITEM_LASER);
		}
	}
}

//////////////////////////////////////找个时间重写一个
void killBullet(int x, int y) {
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].x == x && bullets[i].y == y) {
			bullets[i].type = 0;
			return;
		}
	}
}
//////////////////////////////////////找个时间重写一个

void clearBullet() {
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].type == ITEM_STRENGTH || bullets[i].type == ITEM_LASER) {

		}
		else {
			bullets[i].type = 0;
		}
	}
}

//////////////////////////////////////找个时间重写一个
int getBullet(int type) {
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].type == type) {
			return i;
		}
	}
	return -1;
}
//////////////////////////////////////找个时间重写一个

void gameInfo() {
	gotoXY(0, 0, hOutput);
	cout << "WASD:移动 Space:射击 J:技能[" << ((skillCD < tick) ? ("√") : (to_string((skillCD - tick) / 16))) << "]" << endl;
	cout << "强化: " << ((strengthTick > 0) ? (to_string(strengthTick)) : ("无")) << " 镭射: " << ((laserTick > 0) ? (to_string(laserTick)) : ("无")) << endl;
	cout << "Score: " << score << " Difficulty: " << difficulty <<" tick: " <<tick<< endl;
}

void enemyEvent() {
	if (disableEnemyEvent_All) {
		return;
	}
	if (entities[0].health <= 0) {
		return;
	}
	if (tick == shootTick) {
		bulletGenerator(entities[0].x + 3, entities[0].y + 2, ENEMY_BULLET, -1);
		shootTick += random(75, 110);
	}
	if (entities[0].x < 2) {
		endGame = true;
	}
	if (tick % 4 == 0) {
		if (random(0, 2)) {
			entities[0].x -= 1;
		}
		entities[0].x += random(-1, 1);
		if (getBullet(FRIENDY_BULLET) != -1) {
			if (entities[0].y + 2 < bullets[getBullet(FRIENDY_BULLET)].y && abs(entities[0].y + 2 - bullets[getBullet(FRIENDY_BULLET)].y) < 6) {
				entities[0].y -= 1;
			}
			else if (entities[0].y + 2 >= bullets[getBullet(FRIENDY_BULLET)].y && abs(entities[0].y + 2 - bullets[getBullet(FRIENDY_BULLET)].y) < 6) {
				entities[0].y += 1;
			}
		}
		if (entities[0].x < 0) {
			entities[0].x = 0;
		}if (entities[0].x > MAP_X) {
			entities[0].x = MAP_X;
		}if (entities[0].y < 0) {
			entities[0].y = 0;
		}if (entities[0].y > cmdY - 5) {
			entities[0].y = cmdY - 5;
		}
	}
	if (tick % 160 == 0) {
		bulletGenerator(entities[0].x + 3, entities[0].y + 2 - 1, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3, entities[0].y + 2, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3, entities[0].y + 2 + 1, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3 - 1, entities[0].y + 2, ENEMY_BULLET, -1);
	}
	if (tick % 320 == 0) {
		bulletGenerator(entities[0].x + 3 - 1, entities[0].y + 2, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3, entities[0].y + 2 - 1, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3, entities[0].y + 2, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3, entities[0].y + 2 + 1, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3 + 1, entities[0].y + 2 - 2, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x + 3 + 1, entities[0].y + 2 + 2, ENEMY_BULLET, -1);
	}
}

void drawBossHealthBar() {
	aqua.health -= rand() % 300;
	gotoXY(cmdX / 2 - 15, cmdY - 2, hOutput);
	wprintf(L"\x1b[38;2;%d;%d;%dm", 0xB4, 0x00, 0x9E);
	cout << "#1 湊阿库娅 ";
	cout << "" << aqua.health << "/" << aqua.maxHealth << " ";
	for (int i = 0; i < 10; ++i) {
		if (aqua.health - i * (aqua.maxHealth / 10) > 0) {
			cout << "\x1b[38;2;22;198;12m■";
		}
		else {
			cout << "\x1b[38;2;231;72;86m□";
		}
	}
	cout << "\x1b[38;2;240;248;248";
	gotoXY(cmdX / 2 - 7, cmdY - 1, hOutput);
	wprintf(L"\x1b[38;2;%d;%d;%dm", 0xB4, 0x00, 0x9E);
	cout << "\"世界第一的偶像！\"";
	cout << "\x1b[38;2;240;248;248";
}

void printRGBList() {
	for (int i = 0; i < 256; i += 8) {
		for (int j = 0; j < 256; j += 8) {
			for (int k = 0; k < 256; k += 8) {
				wprintf(L"\x1b[48;2;%d;%d;%dm %2X%2X%2X ", i, j, k, i, j, k);
			}
		}
	}
}

void renderAll() {
	renderGround(0, cmdY, ground, 1, (int)strlen(ground[0]), tick / 4, cmdX);
	if ((tick / 10) % 2) {
		render(sunX, sunY, sun1, 6, (int)strlen(sun1[0]), 0);
	}
	else {
		render(sunX, sunY, sun2, 6, (int)strlen(sun2[0]), 0);
	}
	renderMountain(cmdX + 1 - ((tick / 4) % (cmdX + 18 + 1)), cmdY - 9, mountain, 9, (int)strlen(mountain[0]));
	renderMountain(cmdX + 11 - ((tick / 4) % (cmdX + 18 + 11)), cmdY - 9, mountain, 9, (int)strlen(mountain[0]));
	renderMountain(cmdX + 70 - ((tick / 4) % (cmdX + 18 + 70)), cmdY - 9, mountain, 9, (int)strlen(mountain[0]));
	if (isPlaneDead) {
		render(planeX + 1, planeY + 2, boom, 3, (int)strlen(boom[0]), 0);
	}
	else {
		render(planeX, planeY, planeO, 5, (int)strlen(planeO[0]), FRIENDY_PLANE);
	}
	if (entities[0].health > 0&&!isInBossFight) {
		render(entities[0].x, entities[0].y, plane1, 5, (int)strlen(plane1[0]), ENEMY_PLANE);
	}
	else if(!isInBossFight){
		render(entities[0].x + 1, entities[0].y + 2, boom, 3, (int)strlen(boom[0]), 0);
	}
	if (laserTick > 0 && !isPlaneDead) {
		render(planeX + 6, planeY + 1, bulletLaser, 1, (int)strlen(bulletLaser[0]), FRIENDY_BULLET);
		render(planeX + 7, planeY + 3, bulletLaser, 1, (int)strlen(bulletLaser[0]), FRIENDY_BULLET);
		--laserTick;
	}
	//render(0, 8, colorTest, 1, (int)strlen(colorTest[0]), 0);
	//renderAqua(0, 9, colorTestB, 1, (int)strlen(colorTestB[0]), 0);
	//renderAqua(0, 8, colorTestRGB, 1, (int)strlen(colorTestRGB[0]), 0);
	//renderAqua(40,cmdY-35, onion, 35, (int)strlen(onion[0]), 0);
	//renderAqua(40, cmdY-13, trueAqua, 13, (int)strlen(trueAqua[0]), 0);
	if (isInBossFight) {
		drawBossHealthBar();
		renderAqua(aqua.x, aqua.y, trueAqua, 13, (int)strlen(trueAqua[0]), aqua.type);
	}
}

double calculateJump(int midPos,int x) {
	return (-1.0 * (x - midPos) * (x - midPos) + midPos * midPos);
}

void inBossFight() {//need 5300tick?
	//boss不会死亡
	static int enterCnt = 0;
	if (enterCnt == 0) {
		++enterCnt;
		playBgm("close", "ba.mp3");
		playBgm("open", "th.mp3");
		playBgm("play", "th.mp3");
		thread T1(enemyReborn);
		T1.detach();
		aqua.x = (cmdX+36) / 4;
		aqua.y = cmdY - 13;
		aqua.type = ENEMY_BOSS;
		aqua.health = 44500;
		aqua.maxHealth = 44500;
		aqua.jump = -1;
		sleepTick = 8;
	}
	if (tick % 6 == 0&&aqua.jump==-1) {
		aqua.x += random(-1, 1);
	}
	//模拟boss跳跃
	if (aqua.jump ==-1 &&rand() % 60 == 0) {
		aqua.jump = 8;
		aqua.jumpDirection = random(0, 1);
	}
	else if (aqua.jump >= 0&&tick%4==0) {
		aqua.y=cmdY-13-calculateJump(4,aqua.jump);
		aqua.jump -= 1;
		if (aqua.jumpDirection == 0) {
			aqua.x -= 1;
		}
		else {
			aqua.x += 1;
		}
	}

	//检查坐标合法性
	if (aqua.x < 80/2) {
		aqua.x = (80)/2;
	}if (aqua.x > (cmdX)/2-13) {
		aqua.x = (cmdX)/2-13;
	}if (aqua.y+13< 0) {
		aqua.y = -13;
	}if (aqua.y > cmdY - 13) {
		aqua.y = cmdY - 13;
	}
}

void setDefaultSettings() {
	endGame = false;
	disableEnemyEvent_All = false;
	canExecute = true;
	inAnimation = false;
	isInBossFight = false;
	isPlaneDead = false;
	isTankDead = false;

	getCmdXY();
	setRandomSeed();
	hideCursor();
	ScreenBuff();
	bufferOn();
	clearScreen();

	tick = 0;
	sleepTick = 16;
	shootTick = 0;
	skillCD = 10;
	score = 0;
	strengthTick = 0;
	laserTick = 0;
	sunX = 120;
	sunY = 5;
	entities[0].x = 100;
	entities[0].y = 6;
	entities[0].type = ENEMY_PLANE;
	entities[0].health = 8;
	entities[0].maxHealth = 8;
}

//下一步增加功能：使用多线程实现一个消息队列，选一个地方显示消息，用于显示击杀提示等   参数包含显示时间，显示位置，显示内容
//为boss战做好准备
//游戏结束的处理
int main() {
	cout << "请将窗口最大化后按任意键开始游戏..." << endl;
	//printRGBList();
	_getch();
	playBgm("open", "ba.mp3");
	playBgm("play", "ba.mp3");
	system("cls");
	setDefaultSettings();
	thread T1(enterAnime);
	T1.detach();
	while (!endGame) {
		++tick;
		if (difficulty > 1.1) {
			isInBossFight = true;
			inBossFight();
		}
		gameInfo();
		resetAllMap();
		checkExecute();
		checkBullet();
		enemyEvent();
		renderAll();
		updateScreen();
		checkCrush();
		Sleep(sleepTick);//62tps
	}
	playBgm("close", "ba.mp3");
	playBgm("close", "th.mp3");
	gotoXY(cmdX / 2, cmdY / 2, hOutput);
	cout << "Game Over!" << endl;
	updateScreen();
	bufferOff();
	delete[] lpWord;
	Sleep(100000);
	return 0;
}