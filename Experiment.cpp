﻿#include <bits\stdc++.h>
#include <windows.h>
#include <conio.h>
#define colorA "\033[0;32m"//&a
#define colorB "\033[0;36m"//&b
#define colorC "\033[0;31m"//&c
#define colorE "\033[0;36m"//&e
#define colorBlack "\033[0;30m"
#define colorRed "\033[0;31m"
#define colorGreen "\033[0;32m"
#define colorYellow "\033[0;33m"
#define colorBlue "\033[0;34m"
#define colorPurple "\033[0;35m"
#define colorAqua "\033[0;36m"
#define colorWhite "\033[0;37m"
#define IMG_LENGTH 100
#define MAP_X 121
#define MAP_Y 31
#define DATALEN 10000
#define BULLET_LIST_SIZE 100
using namespace std;

int xx1, yy1, cmdx, cmdy, friendyMap[MAP_Y][MAP_X], enemyMap[MAP_Y][MAP_X], tick = 0,shootTick=10,skillCD1=10;
bool endGame = false, canExecute = true, inAnimation = false;

enum type {
	FRIENDY_PLANE = 101,
	FRIENDY_BULLET = 102,
	ENEMY_PLANE = 501,
	ENEMY_BULLET = 502
};
struct bullet {
	int x, y, type, direction;
};
bullet bullets[BULLET_LIST_SIZE];
struct entity {
	int x, y, type,health,maxHealth;
};
entity entities[10];
char planeO[5][IMG_LENGTH] = {
	"  \\    ",
	"  =\\-  ",
	"=====--",
	"   =\\- ",
	"     \\ "
};
char plane1[5][IMG_LENGTH] = {
	"     / ",
	"   -/= ",
	"--=====",
	" -/=   ",
	" /     "
};
char boom[3][IMG_LENGTH] = {
	"  ^^^^  ",
	"< BOOM >",
	"  vvvv  "
};
char bulletO_1[1][IMG_LENGTH] = {
	"->",
};
char bulletE_1[1][IMG_LENGTH] = {
	"<-",
};

void enterAnime();
void clearBullet();
void killBullet(int x, int y);

HANDLE hOutput;
HANDLE hOutBuf;
DWORD bytes;
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
		FillConsoleOutputCharacterW(hOutput, ' ', cinfo.dwSize.X * cinfo.dwSize.Y, coord, &recnum);//从{0,0}处开始填充' '字符,成功填充个数为recnum
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
	cmdx = bInfo.dwSize.X;
	cmdy = bInfo.dwSize.Y;
}

void render(int baseX, int baseY, char img[][IMG_LENGTH], int first, int second, int type) {//渲染的x,y坐标，图片，图片的第一维，图片的第二维，图片的类型
	if (baseX >= cmdx || baseY < 0) {//超出屏幕范围
		return;
	}
	int l = 0, r = second;
	if (baseX + r > cmdx) {
		r = cmdx - baseX;
	}
	if (baseX < 0) {
		l = -baseX;
		if (l > second) {
			return;
		}
	}
	for (int i = 0; i < first; ++i) {
		gotoXY(((baseX > 0) ? baseX : 0), baseY + i, hOutput);
		for (int j = l; j < r; ++j) {
			cout << img[i][j];
		}
	}
	if (type == ENEMY_PLANE) {
		gotoXY(((baseX > 0) ? baseX : 0), baseY + first, hOutput);
		cout<<" "<< entities[0].health << "/" << entities[0].maxHealth;
	}
	for (int i = baseY; i < baseY + first; ++i) {
		for (int j = baseX + l; j < baseX + r; ++j) {
			if (i < 0 || j < 0 || i >= MAP_Y || j >= MAP_X) {//超出地图范围
				continue;
			}
			if (type == FRIENDY_PLANE) {
				friendyMap[i][j] = type - 100;
			}
			else if (type == ENEMY_PLANE) {
				enemyMap[i][j] = type - 500;
			}
			else if (type == FRIENDY_BULLET) {
				friendyMap[i][j] = type - 100;
			}
			else if (type == ENEMY_BULLET) {
				enemyMap[i][j] = type - 500;
			}
		}
	}
}

int checkCrush() {//0:无碰撞 1:友军敌军碰撞 2:友军被击中 3:敌军被击中
	for (int i = 0; i < MAP_Y; ++i) {
		for (int j = 0; j < MAP_X; ++j) {
			if (friendyMap[i][j] == 1 && enemyMap[i][j] == 1) {
				render(j, i + 1, boom, 3, strlen(boom[0]), 0);
				entities[0].x = 100;
				entities[0].y = 6;
				entities[0].health = 3;
				updateScreen();
				clearBullet();
				Sleep(700);
				enterAnime();
				return 1;
			}
			else if (friendyMap[i][j] == 1 && enemyMap[i][j] == 2) {
				render(j, i + 1, boom, 3, strlen(boom[0]), 0);
				render(entities[0].x, entities[0].y, plane1, 5, strlen(plane1[0]), ENEMY_PLANE);
				entities[0].x = 100;
				entities[0].y = 6;
				updateScreen();
				clearBullet();
				Sleep(700);
				enterAnime();
				return 2;
			}
			else if (friendyMap[i][j] == 2 && enemyMap[i][j] == 1) {
				entities[0].health -= 1;
				killBullet(j-1, i);
				if (entities[0].health <= 0) {
					render(j, i + 1, boom, 3, strlen(boom[0]), 0);
					render(xx1, yy1, planeO, 5, strlen(planeO[0]), FRIENDY_PLANE);
					entities[0].x = 100;
					entities[0].y = 6;
					entities[0].health = 3;
					updateScreen();
					clearBullet();
					Sleep(200);
					return 3;
				}
			}
		}
	}
	return 0;
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

void enterAnime() {//进入入场动画
	memset(friendyMap, 0, sizeof(friendyMap));
	memset(enemyMap, 0, sizeof(enemyMap));
	xx1 = 0;
	yy1 = 3;
	for (int i = -(int)strlen(planeO[0]); i <= 3; ++i) {
		render(xx1 + i, yy1, planeO, 5, strlen(planeO[0]), FRIENDY_PLANE);
		updateScreen();
		Sleep(50);
	}
	xx1 = 3;
}

void bulletGenerator(int x, int y, int type, int direction) {//子弹生成器
	if (x < 0 || y < 0 || x >= MAP_X || y >= MAP_Y) {
		return;
	}
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].type == 0) {
			bullets[i].x = x + 3;
			bullets[i].y = y + 2;
			bullets[i].type = type;
			bullets[i].direction = direction;
			return;
		}
	}
}

void checkExecute() {//检查是否有输入
	if (_kbhit()) {
		char c = _getch();
		if (c == 'w' && yy1 > 0) {
			yy1 -= 1;
		}
		else if (c == 's') {
			yy1 += 1;
		}
		else if (c == 'a') {
			xx1 -= 2;
		}
		else if (c == 'd') {
			xx1 += 2;
		}else if (c == ' ') {
			bulletGenerator(xx1, yy1, FRIENDY_BULLET, 1);
		}else if (c == 'j') {
			if (skillCD1 < tick) {
				skillCD1= tick + 500;
				for (int i = -3; i < 4; ++i) {
					bulletGenerator(xx1, yy1 +i, FRIENDY_BULLET, 1);
					bulletGenerator(xx1+i, yy1, FRIENDY_BULLET, 1);
				}
			}
		}
	}
}

void checkBullet(){//检查子弹
	for (int i = 0; i<BULLET_LIST_SIZE; ++i) {
		if (bullets[i].type == FRIENDY_BULLET) {
			bullets[i].x += 1 * bullets[i].direction;
			if (bullets[i].x >= MAP_X || bullets[i].x < 0) {
				bullets[i].type = 0;
				continue;
			}
			render(bullets[i].x, bullets[i].y, bulletO_1, 1, strlen(bulletO_1[0]), FRIENDY_BULLET);
		}
		else if (bullets[i].type == ENEMY_BULLET) {
			bullets[i].x += 1 * bullets[i].direction;
			if (bullets[i].x >= MAP_X || bullets[i].x < 0) {
				bullets[i].type = 0;
				continue;
			}
			render(bullets[i].x, bullets[i].y, bulletE_1, 1, strlen(bulletE_1[0]), ENEMY_BULLET);
		}
	}
}

void killBullet(int x,int y) {
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].x == x && bullets[i].y == y) {
			bullets[i].type = 0;
			return;
		}
	}
}

void clearBullet() {
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		bullets[i].type = 0;
	}
}

int getBullet(int type) {
	for (int i = 0; i < BULLET_LIST_SIZE; ++i) {
		if (bullets[i].type==type) {
			return i;
		}
	}
	return -1;
}

void gameInfo() {
	gotoXY(0, 0, hOutput);
	cout << "WASD:移动 Space:攻击 J:技能[" << ((skillCD1 < tick) ? ("√") : (to_string((skillCD1 - tick) / 16))) << "]" << endl;
}

void enemyEvent() {
	if (tick == shootTick) {
		bulletGenerator(entities[0].x, entities[0].y, ENEMY_BULLET, -1);
		shootTick+=random(75,110);
	}
	if (entities[0].x<2) {
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
		else {
			//entities[0].y += random(-1, 1);
		}
		if (entities[0].x < 0) {
			entities[0].x=0;
		}if (entities[0].x > MAP_X) {
			entities[0].x = MAP_X;
		}if (entities[0].y < 0) {
			entities[0].y = 0;
		}if (entities[0].y > cmdy-5) {
			entities[0].y = cmdy-5;
		}
	}
	if (tick % 160 == 0) {
		bulletGenerator(entities[0].x, entities[0].y-1, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x, entities[0].y, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x, entities[0].y+1, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x-1, entities[0].y, ENEMY_BULLET, -1);
	}
	if (tick % 320 == 0) {
		bulletGenerator(entities[0].x - 1, entities[0].y, ENEMY_BULLET, -1);

		bulletGenerator(entities[0].x, entities[0].y - 1, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x, entities[0].y, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x, entities[0].y + 1, ENEMY_BULLET, -1);

		bulletGenerator(entities[0].x+1, entities[0].y - 2, ENEMY_BULLET, -1);
		bulletGenerator(entities[0].x+1, entities[0].y + 2, ENEMY_BULLET, -1);
	}
}	

int main() {
	getCmdXY();
	setRandomSeed();
	hideCursor();
	ScreenBuff();
	bufferOn();
	clearScreen();
	system("cls");
	enterAnime();
	entities[0].x = 100;
	entities[0].y = 6;
	entities[0].type = ENEMY_PLANE;
	entities[0].health = 3;
	entities[0].maxHealth = 3;
	while (!endGame) {
		++tick;
		gameInfo();
		memset(friendyMap, 0, sizeof(friendyMap));
		memset(enemyMap, 0, sizeof(enemyMap));
		checkExecute();
		checkBullet();
		enemyEvent();
		render(xx1, yy1, planeO, 5, strlen(planeO[0]), FRIENDY_PLANE);
		render(entities[0].x, entities[0].y, plane1, 5, strlen(plane1[0]), ENEMY_PLANE);
		updateScreen();
		checkCrush();
		Sleep(16);
	}
	cout<< "Game Over!" << endl;
	updateScreen();
	system("pause");
	bufferOff();
	return 0;
}