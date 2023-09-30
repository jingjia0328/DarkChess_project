#ifndef MYAI_INCLUDED
#define MYAI_INCLUDED 
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <memory.h>
#include <time.h>
#include <string>
#include <sstream>
#include <cassert>
#include <random>
#include <unordered_map>
#include <map>
#include <iomanip>
#include <algorithm>
#include <array>
#include "score.h"
// #include "/usr/local/opt/libomp/include/omp.h"
#include "omp.h"
#include <stdint.h>
#define TABLE_SIZE 0x800000ULL
using namespace std;

typedef struct hashdata hashdata;

enum chess {
	TURN,
	MOVE,
	EAT
};

enum hit_state {
	REAL_HIT,
	COLLISION,
	DEPTH_SHALLOW,
	NOTHING
};

struct hashdata {
	int count; int depth; unsigned int piece[16];
	unsigned int NextCurPiece[4]; unsigned int NextHashvalue; int MaxDepth; int NextCurPie; int DCount[14];
	hashdata() {
		count = 0;
		depth = 0;
		NextHashvalue = 0;
		MaxDepth = 0;
	}
};

unsigned int piece[16]; //0空格- 帥k 士g 相m 車r 馬n 炮c 兵p *2 15未翻x 
int piece_count[14];//剩餘棋子數 0-6 7-13
int DCount[14];// 剩餘未翻子
int allEatMove[100][2];//存可吃子的方法 0 src 1 dst 可執行 
int allOnlyMove[100][2];//存可移動非吃子的方法 0 src 1 dst
int AEMindex;//alleatmove index
int AOMindex;//allonlymove index
int EAEMindex;//Ealleatmove index
int EAOMindex;//Eallonlymove index

#pragma omp threadprivate(piece,piece_count,DCount,allEatMove,allOnlyMove,AEMindex,AOMindex,EAEMindex,EAOMindex)

class MyAI{
	public:
		void initial();//初始化 
		void initialBoard(int iPieceCount[], char iCurrentPosition[]);
		void initialhash();
		void generateMove(char move[]);
		void MakeMove(char move[]);
		void Color(int c);
		void initialmap();
		void print_piece(){
			printf("piece: ");
			for (int i = 0; i < 16; i++)
			{
				// cout << hex << piece[i] << " ";
				printf("%x ", piece[i]);
			}
			printf("\n");
			printf("piece_count: ");
			for (int i = 0; i < 14; i++)
			{
				printf("%d ", piece_count[i]);
			}
			printf("\n");
		}
	private:
		unsigned int LS1B(unsigned int x) { return x & (-x); }//取得x的最低位元
		unsigned int MS1B(unsigned int x) { // Most Significant 1 Bit (LS1B)函式
			x |= x >> 32; x |= x >> 16; x |= x >> 8;
			x |= x >> 4; x |= x >> 2; x |= x >> 1;
			return (x >> 1) + 1; //可以取得一串位元中最左邊不為零的位元
		}
		unsigned int CGen(int ssrc, unsigned int toccupied);//返還炮位 
		unsigned int CGenCR(unsigned int x);
		unsigned int CGenCL(unsigned int x);
		int BitsHash(unsigned int x) { return (x * 0x08ED2BE6) >> 27; }
		void chess(int deep);//尋找可用移動
		void chessnp(unsigned int tpiece[16], int deep);
		void readBoard();//讀檔模式 讀取board.txt 把讀入檔案轉成bitboard 還沒倒著存入 
		void createMovetxt(string src, string dst, int srci, int dsti);//創造move.txt 0走步 1翻棋 
		void IndexToBoard(int indexa, int indexb, string& src, string& dst);//把src dst從編號0~31->棋盤編號a1~d4 
		int countAva(int deep, int tAEMindex, int tAOMindex, int tEAEMindex, int tEAOMindex);//呼叫則傳回當前棋版
		int countAvanp(int pie[14], int deep, unsigned int curPiece[16]);
		int search(int depth, int alpha, int beta, uint64_t hashvalue);//搜尋最佳走步
		int searchnp(int depth, unsigned int curPiece[16], int curPie[14], int alpha, int beta, uint64_t hashvalue);
		void drawOrNot();//由past_walk判斷是否平手 之後結果輸出給draw 
		int findPiece(int place, unsigned int curPiece[16]);//傳編號 回傳在這個編號的棋子 
		uint64_t myhash(unsigned int tpiece[16]);//傳入盤面，傳回hash值
		int countAvaEnd(int deep, int tcolor, unsigned int curPiece[16], int tpieCount[14]);
		int factorial(int n);
		int combine(int N, int K);

		// unsigned int genHash(unsigned int tpiece[16]);// 產生盤面hash value
		// unsigned int genTempHash(unsigned int hashvalue, int c1p, int c1_loc, int c2p, int c2_loc, enum chess operation);// 產生暫時盤面hashvalue
		void initialHash();// 初始化rand table
		int findHash(unsigned int piece[16], uint64_t hashindex, int depth);// 查詢盤面是否已存在
		void insertHash(unsigned int piece[16], uint64_t index, int depth);// 將盤面加入同型表
		hit_state checkHit(unsigned int piece[16], uint64_t hashindex, int depth);// 檢查盤面為何種狀態
		
		int index32[32] = { 31, 0, 1, 5, 2, 16, 27, 6, 3, 14, 17, 19, 28, 11, 7, 21, 30, 4, 15, 26, 13,
		18, 10, 20, 29, 25, 12, 9, 24, 8, 23, 22 };
		int GetIndex(unsigned int mask) { return index32[BitsHash(mask)]; }//輸入遮罩返回棋盤編號
		unsigned int pMoves[32] = { 0x00000012,0x00000025,0x0000004A,0x00000084,0x00000121,0x00000252,0x000004A4,0x00000848,
		0x00001210,0x00002520,0x00004A40,0x00008480,0x00012100,0x00025200,0x0004A400,0x00084800,
		0x00121000,0x00252000,0x004A4000,0x00848000,0x01210000,0x02520000,0x04A40000,0x08480000,
		0x12100000,0x25200000,0x4A400000,0x84800000,0x21000000,0x52000000,0xA4000000,0x48000000 };//棋子移動遮罩
		unsigned int pMoves2[32] = {
		0x00000116,0x0000022D,0x0000044B,0x00000886,0x00001161,0x000022D2,0x000044B4,0x00008868,
		0x00011611,0x00022D22,0x00044B44,0x00088688,0x00116110,0x0022D220,0x0044B440,0x00886880,
		0x01161100,0x022D2200,0x044B4400,0x08868800,0x11611000,0x22D22000,0x44B44000,0x88688000,
		0x16110000,0x2D220000,0x4B440000,0x86880000,0x61100000,0xD2200000,0xB4400000,0x68800000 };//翻棋遮罩
		unsigned int pMoves3[32] = {
		0x00000136,0x0000027D,0x000004EB,0x000008C6,0x00001363,0x000027D7,0x00004EB4,0x00008C6C,
		0x00013631,0x00027D72,0x0004EBE4,0x0008C6C8,0x00136310,0x0027D720,0x004EBE40,0x008C6C80,
		0x01363100,0x027D7200,0x04EBE400,0x08C6C800,0x13631000,0x27D72000,0x4EBE4000,0x8C6C8000,
		0x36310000,0x7D720000,0xEBE40000,0xC6C80000,0x63100000,0xD7200000,0xBE400000,0x6C800000 };//翻棋遮罩
		unsigned int file[4] = { 0x11111111,0x22222222,0x44444444,0x88888888 };//行遮罩 
		unsigned int row[8] = { 0x0000000F,0x000000F0,0x00000F00,0x0000F000,0x000F0000,0x00F00000,0x0F000000,0xF0000000 };//列遮罩 
		unsigned int red, black, occupied;//紅 黑 有棋子 
		int influenceValue[7][7] = {
			120,108,60,36,24,48,0,
			0,106,60,36,24,48,12,
			0,0,58,36,24,48,12,
			0,0,0,34,24,48,12,
			0,0,0,0,22,48,12,
			0,0,0,0,0,0,0,
			108,0,0,0,0,0,10
		};

		string move = "a1-a1";//下一步行動 用於背景 
		
		string current_position[32];//盤面狀況總覽 
		string history;
		int timeCount;//剩餘時間 
		int initailBoard = 1;//是否讀取初始版面 
		int past_walk[7][2] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0 };//前七步 用來處理平局問題 
		int draw = 0;//0無平手情況 1有可能進入平手 
		//int RMcount=13;//讀取模式需要 從13行開始讀取結果 
		int threads = 8;

		int color;//0 red 1 black
		int maxDepth = 6;
		unsigned int open = 0xffffffff;//非未翻棋
		unsigned int ch;//需要search的位置 
		int noReDepth = 1;
		uint64_t randtable[15][32]; //0~13 為雙方兵種 14 未翻
		// unordered_map<uint64_t, hashdata> hashtable;
		vector<hashdata> hashtable = vector<hashdata>(TABLE_SIZE);
		vector<hashdata> hashtablemap = vector<hashdata>(TABLE_SIZE);
		// unordered_map<uint64_t, hashdata> hashtablemap;
		// hashdata hashtable[TABLE_SIZE];
		// hashdata hashtablemap[TABLE_SIZE];

		bool isFileExists_ifstream(string &name);

		bool inend = false;

		int TotalSearch = 0;
		int HashHit = 0;
		int RealHit = 0;
		int HashHitSameTurn = 0;
		int MoreDepth = 0;
		int LessDepth = 0;
		int NoUseHash = 0;

		int Power[2][3][3][3][3][3][6][7];//all possible power
		int DistanceP[32][32][7][7];
		string src_out, dst_out;

		clock_t start, stop;//程式執行時間
		int TimeOut;//是否超時
		int TimeLimit;//時間限制
		int test;
		// Hash zhash;
		unsigned int EallEatMove[100][2];//存對手可吃子的方法 0 src 1 dst 下回合才有可能執行 
		unsigned int EallOnlyMove[100][2];//存對手可移動非吃子的方法 0 src 1 dst
};

#endif

