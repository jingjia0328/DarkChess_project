#include "MyAI.h"

void MyAI::initial(){//初始化 
	for (int i = 0; i <= 14; i++)
		piece[i] = 0;
	red = 0;
	black = 0;
	occupied = 0xFFFFFFFF;
	//piece[15] = 0xFFFFFFFF;
	score score;
	score.dynamicPower(Power);
	score.CreateDistanceP(DistanceP);
	AEMindex = 0;
	AOMindex = 0;
	EAEMindex = 0;
	EAOMindex = 0;
}

void MyAI::initialBoard(int iPieceCount[], char iCurrentPosition[]){
	initial();
	initialHash();
	for (int i = 0; i < 14; i++) {
		DCount[i] = iPieceCount[i];
	}
	for (int i = 0; i < 14; i++) {
		piece_count[i] = 0;
	}
	for (int i = 0; i < 16; i++) {
		piece[i] = 0;
	}
	cout << "\n";
	for (int i = 0; i < 32; i++) {
		char type[14] = { 'K', 'G', 'M', 'R', 'N', 'C', 'P', 'k', 'g', 'm', 'r', 'n', 'c', 'p' };
		//0空格- 帥k 士g 相m 車r 馬n 炮c 兵p *2 15未翻x
		int n = (i / 4) * 4 + (4 - (i % 4));
		for (int j = 0; j < 14;j++){
			if(iCurrentPosition[i]==type[j]){
				piece_count[j]++;
				//piece[j + 1] += (unsigned int)pow(2,n-1);
				piece[j + 1] |= 1<<(n-1);
				break;
			}
		}
		if(iCurrentPosition[i]=='-'){
			piece[0] |= 1<<(n-1);
		}
			//piece[0] += (unsigned int)pow(2, n-1);
		if(iCurrentPosition[i]=='X'){
			piece[15] |= 1<<(n-1);
		}
			//piece[15] += (unsigned int)pow(2, n-1);
	}

	for (int i = 0; i < 14; i++) {
		piece_count[i] += DCount[i];
	}
	inend = false;
}

void MyAI::generateMove(char move[]){
	start = clock();
	srand(time(NULL));
	stop = clock();

	int onboardi = 0;//計算場面上未翻棋數量 
	int onboardpi = 0;//計算場面上未翻棋種類 
	int LivePieces = 0;//計算場面上明棋數量 
	int complex = 0;
	int n = 7;
	maxDepth = 6;
	int rc= 0,bc=0;
	for (int i = 0; i < 14; i++)
	{
		onboardi += DCount[i];
		LivePieces += piece_count[i];
		if (DCount[i])
			onboardpi++;
		if(i<7)
			rc+=piece_count[i];
		else
			bc+=piece_count[i];
		
	}
	//if (onboardi == 0)maxDepth = 10;
	if(onboardi+LivePieces>12&&onboardi!=0){
		n = 7;
		maxDepth = 6;
	}else{
		n = 11;
		maxDepth = 10;
	}

	
	if(inend==false&&onboardi==0){
		inend = true;
		hashtable.clear();
		// hashtable.shrink_to_fit();
	}
	drawOrNot();
	complex = onboardi * onboardpi + LivePieces;
	//if (onboardi <=27)maxDepth = (1000 - complex) / 100;
	open = 0xffffffff;
	ch = 0x00000000;
	open ^= piece[15];//非未翻棋
	if (piece[15] == 0xffffffff)
	{
		move[0] = 'b';
		move[1] = '4';
		move[2] = '-';
		move[3] = 'b';
		move[4] = '4';
		return;
	}
	while (open)
	{
		unsigned int take = LS1B(open);
		open ^= take;
		take = GetIndex(take);
		ch |= pMoves3[take];
	}
	uint64_t hashvalue = myhash(piece);
	for (TimeLimit = 1000000; (double(stop - start) < TimeLimit && maxDepth < n); maxDepth++) {
		TotalSearch = 0;
		HashHit = 0;
		HashHitSameTurn = 0;
		RealHit = 0;
		int b;
		if (onboardi != 0) {
			cout << "use search" << endl;
			search(0, -999999, 999999, hashvalue);
		}
		else {
			unsigned int temp_piece[16]; 
			memcpy(temp_piece, piece, sizeof(piece));
			// cout<<"use hashtablemap percent"<<float(hashtablemap.size())/float(hashtablemap.max_size())*100<<endl;
			b = searchnp(0, temp_piece, piece_count, -999999, 999999, hashvalue);
			cout <<"best:"<<b << endl;
		}

		stop = clock();
		cout << " 此步耗時 : " << double(stop - start) / CLOCKS_PER_SEC << " 秒(精準度0.001秒) " << endl;
		cout << " 深度 : " << maxDepth << endl;
		cout << " 本方為 : ";
		if (color == 0) cout << "紅 " << endl;
		else cout << "黑 " << endl;
		cout << "total search: " << TotalSearch << endl;
		cout << "hash hit: " << HashHit << endl;
		cout << "hash hit & same turn: " << HashHitSameTurn << endl;
		cout << "real hit: " << RealHit << endl;
		cout << "complex: " << complex << endl;
		cout << "MoreDepth: " << MoreDepth << endl;
		cout << "LessDepth: " << LessDepth << endl;
		cout << "NoUseHash: " << NoUseHash << endl;
		cout << "test: " << test << endl;
		// cout << "hashtable size: " << hashtable.size() << endl;
		// cout << "hashtablemap size: " << hashtablemap.size() << endl;

		if (src_out == dst_out) {
			move[0] = src_out.at(0);
			move[1] = src_out.at(1);
			move[2] = '-';
			move[3] = src_out.at(0);
			move[4] = src_out.at(1);
		}
		else {
			move[0] = src_out.at(0);
			move[1] = src_out.at(1);
			move[2] = '-';
			move[3] = dst_out.at(0);
			move[4] = dst_out.at(1);
		}
	}
	for (int i = 0; i < 6;i++)
		cout << move[i];
	cout << endl;
}

void MyAI::MakeMove(char move[]){
	int cp = 0;//current_position計算位址 

	if (move[2] == '(') {//record move 'flip' (kgmrncp)
		//string a=move[i].substr(0,2);
		int aa = 100 - move[0];
		int bb = 4 * (56 - move[1]);
		unsigned int cc = aa + bb;//棋盤編號0~31
		cc = 1 << cc;
		if (move[3] == 'K') { piece[1] |= cc; red |= cc; piece[15] ^= cc; DCount[0]--; }
		if (move[3] == 'G') { piece[2] |= cc; red |= cc; piece[15] ^= cc; DCount[1]--; }
		if (move[3] == 'M') { piece[3] |= cc; red |= cc; piece[15] ^= cc; DCount[2]--; }
		if (move[3] == 'R') { piece[4] |= cc; red |= cc; piece[15] ^= cc; DCount[3]--; }
		if (move[3] == 'N') { piece[5] |= cc; red |= cc; piece[15] ^= cc; DCount[4]--; }
		if (move[3] == 'C') { piece[6] |= cc; red |= cc; piece[15] ^= cc; DCount[5]--; }
		if (move[3] == 'P') { piece[7] |= cc; red |= cc; piece[15] ^= cc; DCount[6]--; }
		if (move[3] == 'k') { piece[8] |= cc; black |= cc; piece[15] ^= cc; DCount[7]--; }
		if (move[3] == 'g') { piece[9] |= cc; black |= cc; piece[15] ^= cc; DCount[8]--; }
		if (move[3] == 'm') { piece[10] |= cc; black |= cc; piece[15] ^= cc; DCount[9]--; }
		if (move[3] == 'r') { piece[11] |= cc; black |= cc; piece[15] ^= cc; DCount[10]--; }
		if (move[3] == 'n') { piece[12] |= cc; black |= cc; piece[15] ^= cc; DCount[11]--; }
		if (move[3] == 'c') { piece[13] |= cc; black |= cc; piece[15] ^= cc; DCount[12]--; }
		if (move[3] == 'p') { piece[14] |= cc; black |= cc; piece[15] ^= cc; DCount[13]--; }
		//board[move[i].at(0)-'a'][move[i].at(1)-'1'] = move[3]);	//board[a][1]=k,ai(k)
		//cout << board[move[i].at(0) - 'a'][move[i].at(1) - '1'] ;
	}
	else {//a1-b1之類 
		int c1p, c2p;
		c2p = -1;//如果是-1為移動 
		int aa = 100 - move[0];
		// int bb = 4 * (56 - move[1]);
		int bb = (56 - move[1]) << 2;
		unsigned int c1 = aa + bb;//棋盤編號0~31
		past_walk[6][0] = past_walk[5][0];
		past_walk[5][0] = past_walk[4][0];
		past_walk[4][0] = past_walk[3][0];
		past_walk[3][0] = past_walk[2][0];
		past_walk[2][0] = past_walk[1][0];
		past_walk[1][0] = past_walk[0][0];
		past_walk[0][0] = aa + bb;
		c1 = 1 << c1;
		//cout<<hex<<c1<<"c1 "; 

		int aaa = 100 - move[3];
		// int bbb = 4 * (56 - move[4]);
		int bbb = (56 - move[4]) << 2;
		unsigned int c2 = aaa + bbb;//棋盤編號0~31
		past_walk[6][1] = past_walk[5][1];
		past_walk[5][1] = past_walk[4][1];
		past_walk[4][1] = past_walk[3][1];
		past_walk[3][1] = past_walk[2][1];
		past_walk[2][1] = past_walk[1][1];
		past_walk[1][1] = past_walk[0][1];
		past_walk[0][1] = aaa + bbb;
		c2 = 1 << c2;
		//cout<<hex<<c2; 
		for (int ii = 1; ii < 15; ii++) {
			unsigned int check = piece[ii] & c1;
			if (check != 0) {
				c1p = ii;
				break;
			}
		}
		for (int ii = 1; ii < 15; ii++) {//找c2 清掉 剩餘棋子數更改 
			unsigned int check = piece[ii] & c2;
			if (check != 0) {
				c2p = ii;
				break;
			}
		}
		piece[c1p] ^= c1;//清除原位置c1
		piece[0] |= c1;//空格+c1
		piece[c1p] |= c2;//移動
		if (c2p != -1) {//吃子移動 
			piece[c2p] ^= c2;//清除原位置c2
			int c2ps = c2p - 1;
			piece_count[c2ps]--;
			if (8 > c1p) {//紅吃黑
				red ^= c1;
				black ^= c2;
				red |= c2;
			}
			else {//黑吃紅 
				black ^= c1;
				red ^= c2;
				black |= c2;
			}
			occupied ^= c1;//c1無子 
		}
		else//無吃子移動 
		{
			if (8 > c1p) {//紅動 
				red ^= c1;
				red |= c2;
			}
			else {//黑動 
				black ^= c1;
				black |= c2;
			}
			piece[0] ^= c2;//空格-c2
			occupied ^= c1;//c1無子 
			occupied |= c2;//c2有子 
		}
		//board[move[i].at(3) - 'a'][move[i].at(4) - '1'] = board[move[i].at(0) - 'a'][move[i].at(1) - '1'];	//board[a][2]=k, a1-a2
		//board[move[i].at(0) - 'a'][move[i].at(1) - '1'] = '-';												//board[a][1]='-', a1-a2
		//cout << board[move[i].at(3) - 'a'][move[i].at(4) - '1'];
	}

	//cout<<hex<<piece[9];
}

void MyAI::Color(int c){
	printf("# call color(): color = %d\n", c);
	this->color = c;
}

void MyAI::chess(int deep)
{
	// 每次搜尋走步時，先initial
	/*for (int i = 0; i < 100;i++){
		allEatMove[i][0] = -1;
		allEatMove[i][1] = -1;
		allOnlyMove[i][0] = -1;
		allOnlyMove[i][1] = -1;
	}*/

	unsigned int tred, tblack;
	unsigned int dest;//可以吃子的行動
	AEMindex = 0;
	AOMindex = 0;
	EAEMindex = 0;
	EAOMindex = 0;
	unsigned int toccupied = 0xFFFFFFFF;
	toccupied ^= piece[0];
	tred = piece[1] | piece[2] | piece[3] | piece[4] | piece[5] | piece[6] | piece[7];
	tblack = piece[8] | piece[9] | piece[10] | piece[11] | piece[12] | piece[13] | piece[14];
	int ssrc = 0;
	int check = (color + deep) & 1;//0red 1black
	if (check == 0) {//紅 
		//cout<<"Ours available eat:"<<endl;
		for (int i = 1; i < 8; i++) { //1~7 為帥~兵,src 為棋子起點,dest 為終點。  先算我方 
			unsigned int p = piece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				if (i == 1) //帥,周圍卒(14)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ piece[14]);
				else if (i == 2) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ piece[8]);
				else if (i == 3) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tblack ^ piece[8] ^ piece[9]);
				else if (i == 4) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (piece[11] | piece[12] | piece[13] | piece[14]);
				else if (i == 5) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (piece[12] | piece[13] | piece[14]);
				else if (i == 6) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tblack;
				else if (i == 7) //兵,只能吃將(8)、卒(14)。
					dest = pMoves[ssrc] & (piece[8] | piece[14]);
				else
					dest = 0;
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					allEatMove[AEMindex][0] = ssrc;
					allEatMove[AEMindex][1] = result;
					AEMindex++;
				}
			}
		}
		for (int i = 1; i < 8; i++) { //紅方純移動
			unsigned int p = piece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & piece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					allOnlyMove[AOMindex][0] = ssrc;
					allOnlyMove[AOMindex][1] = result;
					AOMindex++;
				}
			}
		}
		for (int i = 8; i < 15; i++) { //再算對手行動 
			unsigned int p = piece[i];
			while (p) {
				unsigned int mask = LS1B(p);
				p ^= mask;
				ssrc = GetIndex(mask);
				if (i == 8)
					dest = pMoves[ssrc] & (tred ^ piece[7]);
				else if (i == 9) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tred ^ piece[1]);
				else if (i == 10) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tred ^ piece[1] ^ piece[2]);
				else if (i == 11) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (piece[4] | piece[5] | piece[6] | piece[7]);
				else if (i == 12) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (piece[5] | piece[6] | piece[7]);
				else if (i == 13) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tred;
				else if (i == 14) //兵,只能吃將、卒(14)。
					dest = pMoves[ssrc] & (piece[1] | piece[7]);
				else
					dest = 0;
				while (dest) { //對手行動存入EallEatMove 
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					//EallEatMove[EAEMindex][0] = ssrc;
					//EallEatMove[EAEMindex][1] = result;
					EAEMindex++;
				}
			}
		}
		for (int i = 8; i < 15; i++) { //對手純移動 
			unsigned int p = piece[i]; //取得棋子位置
			while (p) { //將黑色 8~14 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & piece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					//EallOnlyMove[EAOMindex][0] = ssrc;
					//EallOnlyMove[EAOMindex][1] = result;
					EAOMindex++;
				}
			}
		}
	}
	else {//黑方移動 
		for (int i = 8; i < 15; i++) { //1~7 為帥~兵,src 為棋子起點,dest 為終點。
			unsigned int p = piece[i]; //取得棋子位置
			while (p) { //將黑色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				if (i == 8) //帥,周圍卒(14)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tred ^ piece[7]);
				else if (i == 9) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tred ^ piece[1]);
				else if (i == 10) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tred ^ piece[1] ^ piece[2]);
				else if (i == 11) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (piece[4] | piece[5] | piece[6] | piece[7]);
				else if (i == 12) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (piece[5] | piece[6] | piece[7]);
				else if (i == 13) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tred;
				else if (i == 14) //兵,只能吃將、卒(14)。
					dest = pMoves[ssrc] & (piece[1] | piece[7]);
				else
					dest = 0;
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					allEatMove[AEMindex][0] = ssrc;
					allEatMove[AEMindex][1] = result;
					AEMindex++;
				}
			}
		}
		for (int i = 8; i < 15; i++) { //黑方純移動
			unsigned int p = piece[i]; //取得棋子位置
			while (p) { //將黑色 8~14 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & piece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					allOnlyMove[AOMindex][0] = ssrc;
					allOnlyMove[AOMindex][1] = result;
					AOMindex++;
				}
			}
		}
		for (int i = 1; i < 8; i++) { //計算敵方移動 
			unsigned int p = piece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				if (i == 1) //帥,周圍卒(14)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ piece[14]);
				else if (i == 2) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ piece[8]);
				else if (i == 3) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tblack ^ piece[8] ^ piece[9]);
				else if (i == 4) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (piece[11] | piece[12] | piece[13] | piece[14]);
				else if (i == 5) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (piece[12] | piece[13] | piece[14]);
				else if (i == 6) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tblack;
				else if (i == 7) //兵,只能吃將(8)、卒(14)。
					dest = pMoves[ssrc] & (piece[8] | piece[14]);
				else
					dest = 0;
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					//EallEatMove[EAEMindex][0] = ssrc;
					//EallEatMove[EAEMindex][1] = result;
					EAEMindex++;
				}
			}
		}
		for (int i = 1; i < 8; i++) { //紅方純移動
			unsigned int p = piece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & piece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					int result = GetIndex(mask2);
					//EallOnlyMove[EAOMindex][0] = ssrc;
					//EallOnlyMove[EAOMindex][1] = result;
					EAOMindex++;
				}
			}
		}
	}
}

void MyAI::chessnp(unsigned int tpiece[16], int deep)
{
	/*for (int i = 0; i < 100;i++){
		allEatMove[i][0] = -1;
		allEatMove[i][1] = -1;
		allOnlyMove[i][0] = -1;
		allOnlyMove[i][1] = -1;
	}*/

	unsigned int tred, tblack;
	unsigned int dest;//可以吃子的行動
	AEMindex = 0;
	AOMindex = 0;
	EAEMindex = 0;
	EAOMindex = 0;
	unsigned int toccupied = 0xFFFFFFFF;
	toccupied ^= tpiece[0];
	tred = tpiece[1] | tpiece[2] | tpiece[3] | tpiece[4] | tpiece[5] | tpiece[6] | tpiece[7];
	tblack = tpiece[8] | tpiece[9] | tpiece[10] | tpiece[11] | tpiece[12] | tpiece[13] | tpiece[14];
	int ssrc = 0;
	int check = (color + deep) & 1;//0red 1black
	if (check == 0) {//紅 
		//cout<<"Ours available eat:"<<endl;
		for (int i = 1; i < 8; i++) { //1~7 為帥~兵,src 為棋子起點,dest 為終點。  先算我方 
			unsigned int p = tpiece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				if (i == 1) //帥,周圍卒(14)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ tpiece[14]);
				else if (i == 2) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ tpiece[8]);
				else if (i == 3) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tblack ^ tpiece[8] ^ tpiece[9]);
				else if (i == 4) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (tpiece[11] | tpiece[12] | tpiece[13] | tpiece[14]);
				else if (i == 5) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (tpiece[12] | tpiece[13] | tpiece[14]);
				else if (i == 6) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tblack;
				else if (i == 7) //兵,只能吃將(8)、卒(14)。
					dest = pMoves[ssrc] & (tpiece[8] | tpiece[14]);
				else
					dest = 0;
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					allEatMove[AEMindex][0] = ssrc;
					allEatMove[AEMindex][1] = result;
					AEMindex++;
				}
			}
		}
		for (int i = 1; i < 8; i++) { //紅方純移動
			unsigned int p = tpiece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & tpiece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					allOnlyMove[AOMindex][0] = ssrc;
					allOnlyMove[AOMindex][1] = result;
					AOMindex++;
				}
			}
		}
		for (int i = 8; i < 15; i++) { //再算對手行動 
			unsigned int p = tpiece[i];
			while (p) {
				unsigned int mask = LS1B(p);
				p ^= mask;
				ssrc = GetIndex(mask);
				if (i == 8)
					dest = pMoves[ssrc] & (tred ^ tpiece[7]);
				else if (i == 9) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tred ^ tpiece[1]);
				else if (i == 10) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tred ^ tpiece[1] ^ tpiece[2]);
				else if (i == 11) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (tpiece[4] | tpiece[5] | tpiece[6] | tpiece[7]);
				else if (i == 12) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (tpiece[5] | tpiece[6] | tpiece[7]);
				else if (i == 13) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tred;
				else if (i == 14) //兵,只能吃將、卒(14)。
					dest = pMoves[ssrc] & (tpiece[1] | tpiece[7]);
				else
					dest = 0;
				while (dest) { //對手行動存入EallEatMove 
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					//EallEatMove[EAEMindex][0] = ssrc;
					//EallEatMove[EAEMindex][1] = result;
					EAEMindex++;
				}
			}
		}
		for (int i = 8; i < 15; i++) { //對手純移動 
			unsigned int p = tpiece[i]; //取得棋子位置
			while (p) { //將黑色 8~14 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & tpiece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					//EallOnlyMove[EAOMindex][0] = ssrc;
					//EallOnlyMove[EAOMindex][1] = result;
					EAOMindex++;
				}
			}
		}
	}
	else {//黑方移動 
		for (int i = 8; i < 15; i++) { //1~7 為帥~兵,src 為棋子起點,dest 為終點。
			unsigned int p = tpiece[i]; //取得棋子位置
			while (p) { //將黑色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				if (i == 8) //帥,周圍卒(14)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tred ^ tpiece[7]);
				else if (i == 9) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tred ^ tpiece[1]);
				else if (i == 10) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tred ^ tpiece[1] ^ tpiece[2]);
				else if (i == 11) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (tpiece[4] | tpiece[5] | tpiece[6] | tpiece[7]);
				else if (i == 12) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (tpiece[5] | tpiece[6] | tpiece[7]);
				else if (i == 13) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tred;
				else if (i == 14) //兵,只能吃將、卒(14)。
					dest = pMoves[ssrc] & (tpiece[1] | tpiece[7]);
				else
					dest = 0;
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					allEatMove[AEMindex][0] = ssrc;
					allEatMove[AEMindex][1] = result;
					AEMindex++;
				}
			}
		}
		for (int i = 8; i < 15; i++) { //黑方純移動
			unsigned int p = tpiece[i]; //取得棋子位置
			while (p) { //將黑色 8~14 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & tpiece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					allOnlyMove[AOMindex][0] = ssrc;
					allOnlyMove[AOMindex][1] = result;
					AOMindex++;
				}
			}
		}
		for (int i = 1; i < 8; i++) { //計算敵方移動 
			unsigned int p = tpiece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				if (i == 1) //帥,周圍卒(14)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ tpiece[14]);
				else if (i == 2) //仕,周圍將(8)以外的黑子都可以吃。
					dest = pMoves[ssrc] & (tblack ^ tpiece[8]);
				else if (i == 3) //相,周圍將、士以外的黑子都可以吃。	
					dest = pMoves[ssrc] & (tblack ^ tpiece[8] ^ tpiece[9]);
				else if (i == 4) //?,只能吃車(11)、馬、炮、卒。
					dest = pMoves[ssrc] & (tpiece[11] | tpiece[12] | tpiece[13] | tpiece[14]);
				else if (i == 5) //傌,只能吃馬(12)、炮、卒。
					dest = pMoves[ssrc] & (tpiece[12] | tpiece[13] | tpiece[14]);
				else if (i == 6) //炮,特殊處理。
					dest = CGen(ssrc, toccupied) & tblack;
				else if (i == 7) //兵,只能吃將(8)、卒(14)。
					dest = pMoves[ssrc] & (tpiece[8] | tpiece[14]);
				else
					dest = 0;
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					EallEatMove[EAEMindex][0] = ssrc;
					EallEatMove[EAEMindex][1] = result;
					EAEMindex++;
				}
			}
		}
		for (int i = 1; i < 8; i++) { //紅方純移動
			unsigned int p = tpiece[i]; //取得棋子位置
			while (p) { //將紅色 1~7 號的子都搜尋一遍
				unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
				p ^= mask; //除去位於最低位元的該兵種
				ssrc = GetIndex(mask); //將最低位元的兵種設為走步起點
				dest = pMoves[ssrc] & tpiece[0];//只走空格
				while (dest) { //如果 dest 有多個位置的話,分開存起來。
					unsigned int mask2 = LS1B(dest);
					dest ^= mask2;
					unsigned int result = GetIndex(mask2);
					EallOnlyMove[EAOMindex][0] = ssrc;
					EallOnlyMove[EAOMindex][1] = result;
					EAOMindex++;
				}
			}
		}
	}
}

unsigned int MyAI::CGen(int ssrc, unsigned int toccupied)
{
	int r = ssrc / 4;//列 
	int c = ssrc % 4;//行 
	unsigned int result = 0;
	unsigned int resulta = 0;
	unsigned int x = ((row[r] & toccupied) ^ (1 << ssrc)) >> (4 * r);//取出跟炮同列 削去炮本身那格
	if (x && c == 0) {
		result |= CGenCL(x);
	}
	else if (x && c == 1) {
		result |= ((x & 12) == 12) ? 8 : 0;
	}
	else if (x && c == 2) {
		result |= ((x & 3) == 3) ? 1 : 0;
	}
	else if (x && c == 3) {
		result |= CGenCR(x);
	}
	result = result << (4 * r);

	x = ((file[c] & toccupied) ^ (1 << ssrc)) >> c;//取出跟炮同行 並全部放到第1行
	if (x && r == 0)
	{
		resulta |= CGenCL(x);
	}
	else if (x && r == 1)
	{
		resulta |= CGenCL(x & 0x11111100);
	}
	else if (x && r == 2)
	{
		unsigned int part = 0;
		if ((x & 0x00000011) == 0x00000011)
			part |= 1;
		resulta |= part | CGenCL(x & 0x11111000);
	}
	else if (x && r == 3)
	{
		unsigned int part = 0;
		part = CGenCR(x & 0x00000111);
		resulta |= part | CGenCL(x & 0x11110000);
	}
	else if (x && r == 4)
	{
		unsigned int part = 0;
		part = CGenCR(x & 0x00001111);
		resulta |= part | CGenCL(x & 0x11100000);
	}
	else if (x && r == 5)
	{
		unsigned int part = 0;
		part = CGenCR(x & 0x00011111);
		resulta |= part | CGenCL(x & 0x11000000);
	}
	else if (x && r == 6)
	{
		resulta |= CGenCR(x & 0x00111111);
	}
	else if (x && r == 7)
	{
		resulta |= CGenCR(x);
	}
	resulta = resulta << c;
	unsigned int re = result | resulta;
	//cout<<hex<<occupied<<" "<<re<<endl;
	//cout<<dec<<r<<" "<<c<<endl;
	return re;
}

unsigned int MyAI::CGenCL(unsigned int x) {
	if (x) {
		unsigned int mask = LS1B(x); //mask 為炮架的遮罩位置,讓 x 消去炮架。
		return (x ^= mask) ? LS1B(x) : 0; //狀況 5~8 傳回 LS1B(x),狀況 2~4 傳回 0。
	}
	else return 0;
}

unsigned int MyAI::CGenCR(unsigned int x) {
	if (x) {
		unsigned int mask = MS1B(x); //mask 為炮架的遮罩位置,讓 x 消去炮架。
		return (x ^= mask) ? MS1B(x) : 0; //狀況 5~8 傳回 MS1B(x),狀況 2~4 傳回 0。
	}
	else return 0;
}

int MyAI::countAva(int deep, int tAEMindex, int tAOMindex, int tEAEMindex, int tEAOMindex)//將士相車馬炮兵
{
	int eat[10];
	int power = 0;
	int redleft = 0;
	int blackleft = 0;
	if (color == 0)//紅 
	{
		for (int i = 0; i < 7; i++)
		{
			power += piece_count[i] * Power[piece_count[7]][piece_count[8]][piece_count[9]][piece_count[10]][piece_count[11]][piece_count[12]][piece_count[13]][i];
			redleft += piece_count[i];
		}
		for (int i = 7; i < 14; i++)
		{
			power -= piece_count[i] * Power[piece_count[0]][piece_count[1]][piece_count[2]][piece_count[3]][piece_count[4]][piece_count[5]][piece_count[6]][i - 7];
			blackleft += piece_count[i];
		}
		if (redleft == 0) power = -100000 + deep * 1000;
		if (blackleft == 0) power = 100000 - deep * 1000;
	}
	else
	{
		for (int i = 7; i < 14; i++)
		{
			power += piece_count[i] * Power[piece_count[0]][piece_count[1]][piece_count[2]][piece_count[3]][piece_count[4]][piece_count[5]][piece_count[6]][i - 7];
			blackleft += piece_count[i];
		}
		for (int i = 0; i < 7; i++)
		{
			power -= piece_count[i] * Power[piece_count[7]][piece_count[8]][piece_count[9]][piece_count[10]][piece_count[11]][piece_count[12]][piece_count[13]][i];
			redleft += piece_count[i];
		}
		if (blackleft == 0) power = -100000 + deep * 1000;
		if (redleft == 0) power = 100000 - deep * 1000;
	}
	//------------------------------------------------------判斷距離 
	int distanceScore = 0;
	if (piece[15] == 0) {
		if (color == 0)
		{
			for (int i = 1; i < 8; i++)
			{
				if (piece[i] && i != 6)//curp!=0 版面上存在
				{
					unsigned int tempcpa = piece[i];
					while (tempcpa)
					{
						unsigned int a = LS1B(tempcpa);//一個一個處理 
						tempcpa = tempcpa ^ a;
						for (int ii = i + 7; ii < 15; ii++)
						{
							if (i == 1 && ii == 14) continue;
							if (piece[ii])
							{
								unsigned int tempcpb = piece[ii];
								while (tempcpb)
								{
									unsigned int b = LS1B(tempcpb);//一個一個處理 
									tempcpb = tempcpb ^ b;

									int ia = GetIndex(a);//得到a的index 0~31 
									int ib = GetIndex(b);//得到b的index 

									int tempS = DistanceP[ia][ib][i - 1][ii - 8];
									distanceScore += tempS;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			for (int i = 8; i < 15; i++)
			{
				if (piece[i] && i != 13)//curp!=0 版面上存在
				{
					unsigned int tempcpa = piece[i];
					while (tempcpa)
					{
						unsigned int a = LS1B(tempcpa);//一個一個處理 
						tempcpa = tempcpa ^ a;
						for (int ii = i - 7; ii < 8; ii++)
						{
							if (i == 8 && ii == 7) continue;
							if (piece[ii])
							{
								unsigned int tempcpb = piece[ii];
								while (tempcpb)
								{
									unsigned int b = LS1B(tempcpb);//一個一個處理 
									tempcpb = tempcpb ^ b;
									int ia = GetIndex(a);//得到a的index 0~31
									int ib = GetIndex(b);//得到b的index

									int tempS = DistanceP[ia][ib][i - 8][ii - 1];
									distanceScore += tempS;
								}
							}
						}
					}
				}
			}
		}
	}
	//------------------------------------------------------移動數 
	int movePoint = 0;
	if (piece[15] != 0) {
		if ((deep & 1) == 0)
		{
			movePoint = tAEMindex * 20 + tAOMindex - tEAEMindex * 50 - tEAOMindex;
		}
		else
		{
			movePoint = tEAEMindex * 20 + tEAOMindex - tAEMindex * 50 - tAOMindex;
		}
	}
	else {
		if ((deep & 1) == 0)
		{
			movePoint = tAOMindex - tEAEMindex * 80 - tEAOMindex;
		}
		else
		{
			movePoint = tEAOMindex - tAEMindex * 80 - tAOMindex;
		}
	}
	//------------------------------------------------------加總
	return power + movePoint - deep + distanceScore;
}

int MyAI::countAvanp(int pie[14], int deep, unsigned int curPiece[16])//將士相車馬炮兵
{
	int eat[10];
	int power = 0;
	int redleft = 0;
	int blackleft = 0;
	if (color == 0)//紅 
	{
		for (int i = 0; i < 7; i++)
		{
			power += pie[i] * Power[pie[7]][pie[8]][pie[9]][pie[10]][pie[11]][pie[12]][pie[13]][i];
			redleft += pie[i];
		}
		for (int i = 7; i < 14; i++)
		{
			power -= pie[i] * Power[pie[0]][pie[1]][pie[2]][pie[3]][pie[4]][pie[5]][pie[6]][i - 7];
			blackleft += pie[i];
		}
		if (redleft == 0) power = -100000 + deep * 1000;
		if (blackleft == 0) power = 100000 - deep * 1000;
	}
	else
	{
		for (int i = 7; i < 14; i++)
		{
			power += pie[i] * Power[pie[0]][pie[1]][pie[2]][pie[3]][pie[4]][pie[5]][pie[6]][i - 7];
			blackleft += pie[i];
		}
		for (int i = 0; i < 7; i++)
		{
			power -= pie[i] * Power[pie[7]][pie[8]][pie[9]][pie[10]][pie[11]][pie[12]][pie[13]][i];
			redleft += pie[i];
		}
		if (blackleft == 0) power = -100000 + deep * 1000;
		if (redleft == 0) power = 100000 - deep * 1000;
	}
	//------------------------------------------------------判斷距離 
	int distanceScore = 0;
	if (curPiece[15] == 0) {
		if (color == 0)
		{
			for (int i = 1; i < 8; i++)
			{
				if (curPiece[i] && i != 6)//curp!=0 版面上存在
				{
					unsigned int tempcpa = curPiece[i];
					while (tempcpa)
					{
						unsigned int a = LS1B(tempcpa);//一個一個處理 
						tempcpa = tempcpa ^ a;
						for (int ii = i + 7; ii < 15; ii++)
						{
							if (i == 1 && ii == 14) continue;
							if (curPiece[ii])
							{
								unsigned int tempcpb = curPiece[ii];
								while (tempcpb)
								{
									unsigned int b = LS1B(tempcpb);//一個一個處理 
									tempcpb = tempcpb ^ b;

									int ia = GetIndex(a);//得到a的index 0~31 
									int ib = GetIndex(b);//得到b的index 

									int tempS = DistanceP[ia][ib][i - 1][ii - 8];
									distanceScore += tempS;
								}
							}
						}
					}
				}
			}
		}
		else
		{
			for (int i = 8; i < 15; i++)
			{
				if (curPiece[i] && i != 13)//curp!=0 版面上存在
				{
					unsigned int tempcpa = curPiece[i];
					while (tempcpa)
					{
						unsigned int a = LS1B(tempcpa);//一個一個處理 
						tempcpa = tempcpa ^ a;
						for (int ii = i - 7; ii < 8; ii++)
						{
							if (i == 8 && ii == 7) continue;
							if (curPiece[ii])
							{
								unsigned int tempcpb = curPiece[ii];
								while (tempcpb)
								{
									unsigned int b = LS1B(tempcpb);//一個一個處理 
									tempcpb = tempcpb ^ b;
									int ia = GetIndex(a);//得到a的index 0~31
									int ib = GetIndex(b);//得到b的index

									int tempS = DistanceP[ia][ib][i - 8][ii - 1];
									distanceScore += tempS;
								}
							}
						}
					}
				}
			}
		}
	}
	//------------------------------------------------------移動數 
	int movePoint = 0;
	if (curPiece[15] != 0) {
		if ((deep & 1) == 0)
		{
			movePoint = AEMindex * 20 + AOMindex - EAEMindex * 50 - EAOMindex;
		}
		else
		{
			movePoint = EAEMindex * 20 + EAOMindex - AEMindex * 50 - AOMindex;
		}
	}
	else {
		if ((deep & 1) == 0)
		{
			movePoint = AOMindex - EAEMindex * 80 - EAOMindex;
		}
		else
		{
			movePoint = EAOMindex - AEMindex * 80 - AOMindex;
		}
	}
	//------------------------------------------------------加總
	return power + movePoint - deep + distanceScore;
}

int MyAI::countAvaEnd(int deep, int tcolor, unsigned int curPiece[16], int tpieCount[14]) {
	int check = (deep ^ tcolor);
	int alpha = 0;
	int beta = 0;
	int gamma = 0;
	vector<int> indexb;
	vector<int> indexg;
	stringstream ss;
	unsigned int tcurPiece[16];
	memcpy(tcurPiece, curPiece, sizeof(tcurPiece));
	ss << "data\\";

	ss << (tpieCount[0] * 2 + tpieCount[7]) << (tpieCount[1] * 3 + tpieCount[8]) << (tpieCount[2] * 3 + tpieCount[9])
		<< (tpieCount[3] * 3 + tpieCount[10]) << (tpieCount[4] * 3 + tpieCount[11]) << (tpieCount[5] * 3 + tpieCount[12])
		<< tpieCount[6] << tpieCount[13];
	int piecelive = 0;
	for (int i = 0; i < 14; i++) {
		if (tpieCount[i]) {
			piecelive += tpieCount[i];
		}
	}
	//a//
	for (int i = 0; i < 14; i++) {
		if (tpieCount[i]) {
			alpha = GetIndex(LS1B(tcurPiece[i + 1]));
			tcurPiece[i + 1] ^= LS1B(tcurPiece[i + 1]);
			break;
		}
	}
	//a//

	for (int i = 1; i < 15; i++) {//get index in order of piece 帥士.....
		unsigned int p = tcurPiece[i];
		while (p) {
			unsigned int mask = LS1B(p);
			p ^= mask;
			indexg.push_back(GetIndex(mask));
		}
	}

	indexb.assign(indexg.begin(), indexg.end());
	sort(indexb.begin(), indexb.end());

	try
	{
		//b//
		int K = (piecelive - 1);
		int N = 31;
		vector<char> betabitmask;
		for (int i = 0; i < K; i++)
		{
			betabitmask.push_back(1);
		}
		betabitmask.resize(N, 0);
		
		// string betabitmask(K, 1); // K leading 1's
		// betabitmask.resize(N, 0); // N-K trailing 0's
		
		int betacount = 0;	
		int got = 0;
		do {
			vector<int> index;
			for (int i = 0; i < N; ++i) // [0..N-1] integers
			{
				if (betabitmask[i]) {
					index.push_back((i >= alpha) ? (i + 1) : i);
				}
			}
			betacount++;
			if (index == indexb) {
				got = 1;
				beta = betacount;
				break;
			}
		} while (prev_permutation(betabitmask.begin(), betabitmask.end()));
		if (beta == 0) {
			/*cout << "err beta=0" << endl;
			cout << "alpha: " << alpha << endl;
			cout << "K: " << K << endl;
			cout << "indexb: ";
			for (int i = 0; i < indexb.size(); i++) {
				cout << " " << indexb[i];
			}
			cout << endl;
			cout << "betabitmask: " << betabitmask << endl;
			cout << "got: " << got << endl;
			cout << endl;
			for (int i = 0; i < 16; i++) {
				cout << hex << curPiece[i] << oct << endl;
			}*/
			//system("pause");
		}
		//b//

		//c//
		int gammacount = 0;
		do {
			gammacount++;
			if (indexb == indexg) {
				gamma = gammacount;
			}
		} while (next_permutation(indexb.begin(), indexb.end()));
		//c//

		if (check == 0) {//紅
			ss << ".r";
		}
		else {//黑
			ss << ".b";
		}

		string fname;
		ss >> fname;
		ss.clear();
		if(isFileExists_ifstream(fname)==true){
			ifstream ifile;
			ifile.open(fname, ios::in);
			//cout << "file open success" << endl;
			int in = 0;
			int result = factorial(K) * (combine(31, K) * alpha + beta) + gamma;
			while (ifile >> in) {
				//cout << "in: " << in << endl;
				if (in == result) {
					ifile >> in;
					/*
					if (in == 2) {
						cout << check << endl;
						for (int i = 0; i < 16; i++) {
							cout <<i<<": "<< hex << curPiece[i] << oct << endl;
						}
						cout << endl;
					}
					*/
					//cout << "countavaend success" << endl;
					ifile.close();
					return in;
				}
			}
			// printf("loop3 success\nerr\nss: %s\nresult: %d\nalpha: %d\nbeta: %d\ngamma: %d\n", ss.str(), result, alpha, beta, gamma);
			return countAvanp(tpieCount, deep, tcurPiece);
			cout << "loop3 success" << endl;
			cout << "err" << endl;
			cout << "ss: " << ss.str() << endl;
			cout << "result: " << result << endl;
			cout << "alpha: " << alpha << endl;
			cout << "beta: " << beta << endl;
			cout << "gamma: " << gamma << endl;
		}else{
			//cout << "countavaend success" << endl;
			return countAvanp(tpieCount, deep, tcurPiece);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "countAvaEnd" << e.what() << '\n';
		return countAvanp(tpieCount, deep, tcurPiece);
	}
}
int MyAI::search(int depth, int alpha, int beta, uint64_t hashvalue)
{
	if (double(stop - start) > TimeLimit) {
		TimeOut = 1;
		return 0;
	}
	stop = clock();
	TotalSearch++;
	int lastcount = 0;
	for (int i = 0; i < 14; i++) {
		if (piece_count[i] > 0) {
			lastcount += piece_count[i];
		}
	}
	// clock_t search_countAvaEnd_1 = clock();
	if (lastcount <= 2 && depth != 0) {
		test++;
		int got = countAvaEnd(depth, color, piece, piece_count);
		if (got > 0) {
			return 100000 - got * 1000;
		}
		else if(got<0){
			return -100000 + got * 1000;
		}
		else {
			return 0;
		}
	}
	// clock_t search_countAvaEnd_2 = clock();
	// if(search_countAvaEnd_2 - search_countAvaEnd_1 > 20000) printf("search_countAvaEnd\n");
	
	uint64_t hashindex = hashvalue % TABLE_SIZE;
	int tempscore = 0;
	int HashCheck = 0;
	unsigned int HashTempC1;
	unsigned int HashTempC2;
	
	// int board_score = hashtable[hashindex].count;
	// bool same_turn = (hashtable[hashindex].depth & 1 == depth & 1);
	// bool record_or_not = true;
	// bool search_depth = ((hashtable[hashindex].MaxDepth - hashtable[hashindex].depth) >= (maxDepth - depth));

	// for (int i = 0; i < 16; i++) {
	// 	if (piece[i] ^ hashtable[hashindex].piece[i]) { // 相同的話 xor == 0
	// 		record_or_not = false;
	// 		break;
	// 	}
	// }

	// if(board_score && same_turn && record_or_not && search_depth && maxDepth & 1 == hashtable[hashindex].MaxDepth & 1){
	// 	// state = REAL_HIT;
	// 	HashHit++;
	// 	HashHitSameTurn++;
	// 	RealHit++;
	// 	return hashtable[hashindex].count - (depth - hashtable[hashindex].depth);
	// }
	// else if(board_score && same_turn && record_or_not && !search_depth && depth > noReDepth && hashtable[hashindex].NextHashvalue){
	// 	// state = DEPTH_SHALLOW;
	// 	HashHit++;
	// 	HashHitSameTurn++;
	// 	MoreDepth++;
	// }
	// else if(board_score && ((same_turn && !record_or_not) || !same_turn)){
	// 	// state = COLLISION;
	// 	HashHit++;
	// 	HashHitSameTurn++;
	// 	insertHash(piece, hashindex, depth);
	// }
	// else if(board_score){
	// 	// 新盤面
	// 	HashHit++;
	// 	insertHash(piece, hashindex, depth);
	// }
	// else{
	// 	// state = NOTHING;
	// 	insertHash(piece, hashindex, depth);
	// }
	// clock_t search_hashtable_1 = clock();
	if (hashtable[hashindex].count != 0) {
		HashHit++;
		if ((hashtable[hashindex].depth & 1) == (depth & 1)) {
			HashHitSameTurn++;
			int temp = 0;
			for (int i = 0; i < 16; i++) {
				if (piece[i] ^ hashtable[hashindex].piece[i]) {
					temp++;
					break;
				}
			}
			if (temp == 0) {
				RealHit++;
				if (((hashtable[hashindex].MaxDepth - hashtable[hashindex].depth) > (maxDepth - depth))
					) {
					if ((hashtable[hashindex].MaxDepth & 1) == (maxDepth & 1)) {
						LessDepth++;
						return hashtable[hashindex].count - (depth - hashtable[hashindex].depth);
					}
					else {
						NoUseHash++;
					}
				}
				else {
					if (depth > noReDepth && hashtable[hashindex].NextHashvalue) {
						MoreDepth++;
						if (0) {

						}
						else {
							
							// HashCheck = 1;
							
							// int tempNextCurPie = hashtable[hashindex].NextCurPie;
							// unsigned int tempc1 = hashtable[hashindex].NextCurPiece[0];
							// unsigned int tempc2 = hashtable[hashindex].NextCurPiece[1];
							// HashTempC1 = tempc1;
							// HashTempC2 = tempc2;
							// int tempc1p = hashtable[hashindex].NextCurPiece[2];
							// int tempc2p = hashtable[hashindex].NextCurPiece[3];
							// unsigned int tempNextHashvalue = hashtable[hashindex].NextHashvalue;

							// if (tempNextCurPie != -1) {//sim
							// 	piece[tempc1p] ^= tempc1;//清除原位置c1
							// 	piece[tempc1p] |= tempc2;//移動
							// 	piece[0] |= tempc1;//空格+c1
							// 	piece[tempc2p] ^= tempc2;//清除原位置c2
							// 	piece_count[tempNextCurPie]--;
							// }
							// else {
							// 	piece[tempc1p] ^= tempc1;//清除原位置c1
							// 	piece[tempc1p] |= tempc2;//移動
							// 	piece[0] |= tempc1;//空格+c1
							// 	piece[0] ^= tempc2;//空格-c2
							// }

							// tempscore = search(depth + 1, alpha, beta, tempNextHashvalue);

							// if (tempNextCurPie != -1) {//unsim
							// 	piece[tempc1p] ^= tempc2;//清除原位置c2
							// 	piece[tempc1p] |= tempc1;//移動
							// 	piece[0] ^= tempc1;//空格-c1
							// 	piece[tempc2p] |= tempc2;//回原位置c2
							// 	piece_count[tempNextCurPie]++;
							// }
							// else {
							// 	piece[tempc1p] ^= tempc2;//清除原位置c2
							// 	piece[0] |= tempc2;//空格+c2
							// 	piece[tempc1p] |= tempc1;//移動
							// 	piece[0] ^= tempc1;//空格-c1
							// }

							// if (depth % 2 == 0)//max
							// {
							// 	if (tempscore > alpha)
							// 	{
							// 		alpha = tempscore;
							// 	}

							// }
							// else//min
							// {
							// 	if (tempscore < beta)
							// 	{
							// 		beta = tempscore;
							// 	}
							// }
							// if (beta <= alpha)
							// {
							// 	hashtable[hashindex].depth = depth;
							// 	hashtable[hashindex].MaxDepth = maxDepth;
							// 	if (depth % 2 == 0) {//max
							// 		hashtable[hashindex].count = alpha;
							// 		return alpha;
							// 	}
							// 	else {//min
							// 		hashtable[hashindex].count = beta;
							// 		return beta;
							// 	}
							// }
							
						}
					}
					else {
						NoUseHash++;
					}
				}
			}
			else {//hash collision
				hashtable[hashindex].NextHashvalue = 0;
				memcpy(hashtable[hashindex].piece, piece, sizeof(hashtable[hashindex].piece));
				memcpy(hashtable[hashindex].DCount, DCount, sizeof(hashtable[hashindex].DCount));
			}
		}
		else {//層數不同
			hashtable[hashindex].NextHashvalue = 0;
			memcpy(hashtable[hashindex].piece, piece, sizeof(hashtable[hashindex].piece));
			memcpy(hashtable[hashindex].DCount, DCount, sizeof(hashtable[hashindex].DCount));
		}
	}
	else {//新盤面
		hashtable[hashindex].depth = depth;
		hashtable[hashindex].MaxDepth = maxDepth;
		memcpy(hashtable[hashindex].piece, piece, sizeof(hashtable[hashindex].piece));
		memcpy(hashtable[hashindex].DCount, DCount, sizeof(hashtable[hashindex].DCount));
	}

	chess(depth);//void chess(unsigned int tpiece[16], int deep,int AEMindex,int AOMindex ,int EAEMindex ,int EAOMindex , unsigned int allEatMove[50][2] , unsigned int allOnlyMove[50][2])

	unsigned int taEM[100][2];//存可吃子的方法 0 src 1 dst 避免被往下搜尋時刷掉
	unsigned int taOM[100][2];//存可移動非吃子的方法 0 src 1 dst
	int tAEMi = AEMindex;//alleatmove index
	int tAOMi = AOMindex;//allonlymove index
	int tEAEMi = EAEMindex;//allonlymove index
	int tEAOMi = EAOMindex;//allonlymove index
	memcpy(taEM, allEatMove, sizeof(taEM));
	memcpy(taOM, allOnlyMove, sizeof(taOM));



	vector<int> weights;//計算所有移動與翻棋的位置src
	vector<int> weightd;//計算所有移動與翻棋的位置dst
	vector<int> weight;//計算所有移動與翻棋的得分
	int wp = 0;
	int best = -9999999;
	if ((depth & 1) == 1) {
		best = 9999999;
	}

	if (HashCheck) {
		weight.push_back(tempscore);
		best = tempscore;
		weights.push_back(100);
		weightd.push_back(100);
		wp++;
	}

	for (int i = 0; i < tAEMi; i++)//吃子
	{
		int deeper = depth + 1;
		int c1p, c2p = -1;
		unsigned int c1 = 1 << taEM[i][0];
		unsigned int c2 = 1 << taEM[i][1];
		for (int ii = 1; ii < 15; ii++) {//找到c1 放入c1p  
			unsigned int check = piece[ii] & c1;
			if (check != 0) {
				c1p = ii;
				break;
			}
		}
		for (int ii = 1; ii < 15; ii++) {//找c2
			unsigned int check = piece[ii] & c2;
			if (check != 0) {
				c2p = ii;
				break;
			}
		}
		if (c2p == -1 || c1p == -1) {//errrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
#pragma omp critical
			{
				// cout << endl;
				cout << "err1 c1: " << hex << c1 << " c2: " << c2 << endl;
				// cout << "depth: " << depth << endl;
				// for (int j = 0; j < 16; j++) {
				// 	//cout << hex << piece[j] << dec << endl;
				// }
			}
			break;
		}
		piece[c1p] ^= c1;//清除原位置c1
		piece[c1p] |= c2;//移動
		piece[0] |= c1;//空格+c1
		piece[c2p] ^= c2;//清除原位置c2
		piece_count[c2p - 1]--;
		unsigned int tempc1 = c1;
		unsigned int tempc2 = c2;
		unsigned int tempc1p = c1p;
		unsigned int tempc2p = c2p;
		int tempNextCurPie = c2p - 1;
		uint64_t tempNextHashvalue = hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c2p - 1][GetIndex(c2)] ^ randtable[c1p - 1][GetIndex(c2)];
		if (HashCheck == 1) {
			if (c1 == HashTempC1 && c2 == HashTempC2) {
				piece[c1p] ^= c2;//清除原位置c2
				piece[c1p] |= c1;//移動
				piece[0] ^= c1;//空格-c1
				piece[c2p] |= c2;//回原位置c2
				piece_count[c2p - 1]++;
				continue;
			}
			else {
			}
		}
		weights.push_back(taEM[i][0]);
		weightd.push_back(taEM[i][1]);

		int tempweight = 0;
		tempweight = search(deeper, alpha, beta, hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c2p - 1][GetIndex(c2)] ^ randtable[c1p - 1][GetIndex(c2)]);
		weight.push_back(tempweight);
		if (depth == 0)
		{
			int r = rand() % 6;;
			string a[6] = { "⊙3⊙e","(--;)e","(〃ω〃)e","(’-_-`)e","|ω˙）e","(*≧艸≦)e" };
			cout << a[r] + ".";
		}
		piece[c1p] ^= c2;//清除原位置c2
		piece[c1p] |= c1;//移動
		piece[0] ^= c1;//空格-c1
		piece[c2p] |= c2;//回原位置c2
		piece_count[c2p - 1]++;
		if (depth % 2 == 0)//max
		{
			if (weight[wp] > alpha)
			{
				alpha = weight[wp];
			}
			if (weight[wp] > best)
			{
				best = weight[wp];
				hashtable[hashindex].NextCurPiece[0] = tempc1;
				hashtable[hashindex].NextCurPiece[1] = tempc2;
				hashtable[hashindex].NextCurPiece[2] = tempc1p;
				hashtable[hashindex].NextCurPiece[3] = tempc2p;
				hashtable[hashindex].NextCurPie = tempNextCurPie;
				hashtable[hashindex].NextHashvalue = tempNextHashvalue;
			}
		}
		else//min
		{
			if (weight[wp] < beta)
			{
				beta = weight[wp];
			}
			if (weight[wp] < best)
			{
				best = weight[wp];
				hashtable[hashindex].NextCurPiece[0] = tempc1;
				hashtable[hashindex].NextCurPiece[1] = tempc2;
				hashtable[hashindex].NextCurPiece[2] = tempc1p;
				hashtable[hashindex].NextCurPiece[3] = tempc2p;
				hashtable[hashindex].NextCurPie = tempNextCurPie;
				hashtable[hashindex].NextHashvalue = tempNextHashvalue;
			}
		}
		if (beta <= alpha)
		{
			if ((depth & 1) == 0) {//max
				hashtable[hashindex].count = alpha;
				return alpha;
			}
			else {//min
				hashtable[hashindex].count = beta;
				return beta;
			}
		}
		wp++;
	}

	if (depth >= maxDepth)//到底 --------------------------------------------------------------------------------------------------------
	{
		int re = countAva(depth, tAEMi, tAOMi, tEAEMi, tEAOMi);
		hashtable[hashindex].count = re;
		return re;
	}

	for (int i = 0; i < tAOMi; i++)//純移動 --------------------------------------------------------------------------------------------------------
	{
		int deeper = depth + 1;
		int c1p = -1;
		unsigned int c1 = 1 << taOM[i][0];
		unsigned int c2 = 1 << taOM[i][1];
		for (int ii = 1; ii < 15; ii++) {//找兵種
			unsigned int check = piece[ii] & c1;
			if (check != 0) {
				c1p = ii;
				break;
			}
		}
		if (c1p == -1) {//errrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
#pragma omp critical
			{
				cout << endl;
				cout << "err2 c1: " << hex << c1 << " c2: " << c2 << endl;
				cout << "depth: " << depth << endl;

				// for (int j = 0; j < 16; j++) {
				// 	//cout << hex << piece[j] << dec << endl;
				// }
			}
			break;
		}

		piece[c1p] ^= c1;//清除原位置c1
		piece[c1p] |= c2;//移動
		piece[0] |= c1;//空格+c1
		piece[0] ^= c2;//空格-c2

		unsigned int tempc1 = c1;
		unsigned int tempc2 = c2;
		unsigned int tempc1p = c1p;
		int tempNextCurPie = -1;
		uint64_t tempNextHashvalue = hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c1p - 1][GetIndex(c2)];

		if (HashCheck == 1) {
			if (c1 == HashTempC1 && c2 == HashTempC2) {
				piece[c1p] ^= c2;//清除原位置c2
				piece[0] |= c2;//空格+c2
				piece[c1p] |= c1;//移動
				piece[0] ^= c1;//空格-c1
				continue;
			}
			else {
			}
		}

		weights.push_back(taOM[i][0]);
		weightd.push_back(taOM[i][1]);

		int tempweight = 0;
		tempweight = search(deeper, alpha, beta, hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c1p - 1][GetIndex(c2)]);
		weight.push_back(tempweight);
		if (depth == 0)
		{
			int r = rand() % 6;;
			string a[6] = { "⊙3⊙m","(--;)m","(〃ω〃)m","(’-_-`)m","|ω˙）m","(*≧艸≦)m" };
			cout << a[r] + ".";
		}
		piece[c1p] ^= c2;//清除原位置c2
		piece[0] |= c2;//空格+c2
		piece[c1p] |= c1;//移動
		piece[0] ^= c1;//空格-c1



		if ((depth & 1) == 0)//max
		{
			if (weight[wp] > best)
			{
				best = weight[wp];
				hashtable[hashindex].NextCurPiece[0] = tempc1;
				hashtable[hashindex].NextCurPiece[1] = tempc2;
				hashtable[hashindex].NextCurPiece[2] = tempc1p;
				hashtable[hashindex].NextCurPie = tempNextCurPie;
				hashtable[hashindex].NextHashvalue = tempNextHashvalue;
			}
			if (weight[wp] > alpha)
			{
				alpha = weight[wp];
			}
		}
		else//min
		{
			if (weight[wp] < best)
			{
				best = weight[wp];
				hashtable[hashindex].NextCurPiece[0] = tempc1;
				hashtable[hashindex].NextCurPiece[1] = tempc2;
				hashtable[hashindex].NextCurPiece[2] = tempc1p;
				hashtable[hashindex].NextCurPie = tempNextCurPie;
				hashtable[hashindex].NextHashvalue = tempNextHashvalue;
			}
			if (weight[wp] < beta)
			{
				beta = weight[wp];
			}
		}
		if (beta <= alpha)
		{
			if ((depth & 1) == 0) {//max
				hashtable[hashindex].count = alpha;
				return alpha;
			}
			else {//min
				hashtable[hashindex].count = beta;
				return beta;
			}
		}
		wp++;
	}

	if (piece[15] != 0)//先試翻棋 做完後call search 
	{
		for (int ssrc = 0; ssrc < 32; ssrc++) { //搜尋盤面上 32 個位置
			if (piece[15] & (1 << ssrc) && ch & (1 << ssrc) && depth <= noReDepth) { //若為未翻子 在未翻子的遮罩內 depth<=noReDepth 
				if (depth == 0)
				{
					int r = rand() % 6;;
					string a[6] = { "⊙3⊙f","(--;)f","(〃ω〃)f","(’-_-`)f","|ω˙）f","(*≧艸≦)f" };
					cout << a[r] + ".";
				}
				int a = 0;
				int tempweight = 0;
				weights.push_back(ssrc);
				weightd.push_back(ssrc);
#pragma omp parallel  for default(none) reduction(+:tempweight,a) num_threads(threads) copyin(piece,piece_count,DCount) firstprivate(depth,ssrc,hashvalue)
				for (int pID = 0; pID < 14; pID++) { //搜尋可能會翻出之子
					if (DCount[pID]) { //若該兵種可能被翻出
						a += DCount[pID];
						int deeper = depth + 1;
						unsigned int c = 1 << ssrc;
						int cpID = pID + 1;

						piece[cpID] |= c;//模擬該兵種翻出來
						piece[15] ^= c;
						DCount[pID]--;

						int ttempweight = 0;
						ttempweight = search(deeper, -999999, 999999, hashvalue ^ randtable[pID][ssrc] ^ randtable[14][ssrc]);
						DCount[pID]++;
						ttempweight *= DCount[pID];
						tempweight += ttempweight;

						piece[cpID] ^= c;//將模擬翻出的子復原
						piece[15] |= c;
					}
				}
				//if (weights.back() > 32)cout << weights.back() << endl;
				//if (weightd.back() > 32)cout << weightd.back() << endl;

				tempweight /= a;
				weight.push_back(tempweight);
				if ((depth & 1) == 0)//max
				{
					if (weight[wp] > alpha)
					{
						alpha = weight[wp];
					}
					if (weight[wp] > best)
					{
						best = weight[wp];
					}
				}
				else//min
				{
					if (weight[wp] < beta)
					{
						beta = weight[wp];
					}
					if (weight[wp] < best)
					{
						best = weight[wp];
					}
				}
				if (beta <= alpha)
				{
					if ((depth & 1) == 0) {//max
						hashtable[hashindex].count = alpha;
						return alpha;
					}
					else {//min
						hashtable[hashindex].count = beta;
						return beta;
					}
				}
				wp++;
			}
		}
	}
	if (piece[15] != 0 && depth > noReDepth)//可以走空步
	{

		weights.push_back(0);
		weightd.push_back(0);
		int tempweight = 0;
		tempweight = search(depth + 1, alpha, beta, hashvalue);
		weight.push_back(tempweight);
		wp++;
	}

	if (depth == 0)//root
	{
		string src, dst;
		int srci, dsti;
		if (TimeOut == 1) {
			return 0;
		}
		cout << endl << "------------------------------------" << endl;
		int recordi = 0;
		for (int i = wp - 1; i >= 0; i--)
		{
			if (depth == 0)
			{
				cout << weights[i] << " " << weightd[i] << " " << weight[i] << endl;
			}
			if (weight[i] == best)
			{
				recordi = i;
				srci = weights[i];
				dsti = weightd[i];
			}
		}

		if (draw == 1)
		{
			int loseOne = 0;
			unsigned int c1 = 1 << srci;
			for (int ii = 1; ii < 15; ii++) {//找兵種
				unsigned int check = piece[ii] & c1;
				if (check != 0) {
					if (ii < 8) {
						loseOne = Power[piece_count[7]][piece_count[8]][piece_count[9]][piece_count[10]][piece_count[11]][piece_count[12]][piece_count[13]][ii - 1];
					}
					else {
						loseOne = Power[piece_count[0]][piece_count[1]][piece_count[2]][piece_count[3]][piece_count[4]][piece_count[5]][piece_count[6]][ii - 8];
					}
					break;
				}
			}

			if (best - loseOne < 0); //可能輸 故意平手? 
			else if (srci == past_walk[1][1] && dsti == past_walk[1][0])
			{
				cout << "draw denied" << endl;
				weight[recordi] -= 999999;//可能會贏 選擇不平手? 
				best = weight[0];

				for (int ii = wp - 1; ii >= 0; ii--)//重新尋找 
				{
					if (weight[ii] > best)
					{
						best = weight[ii];
						srci = weights[ii];
						dsti = weightd[ii];
					}
				}
			}
		}
		IndexToBoard(srci, dsti, src, dst);
		createMovetxt(src, dst, srci, dsti);
	}
	if (wp == 0) best = countAva(depth, tAEMi, tAOMi, tEAEMi, tEAOMi);
	if (best == 9999999 || best == -9999999) best = countAva(depth, tAEMi, tAOMi, tEAEMi, tEAOMi);;
	hashtable[hashindex].count = best;
	return best;
}

int MyAI::searchnp(int depth, unsigned int curPiece[16], int curPie[14], int alpha, int beta, uint64_t hashvalue)
{

	// if(float(hashtablemap.size())/float(hashtablemap.max_size())>=0.35)
	// {
	// 	cout<<"use hashtablemap percent"<<float(hashtablemap.size())/float(hashtablemap.max_size())*100<<endl;
	// 	hashtablemap.clear();
	// }
	if (double(stop - start) > TimeLimit) {
		TimeOut = 1;
		return 0;
	}
	stop = clock();
	TotalSearch++;

	int lastcount = 0;
	for (int i = 0; i < 14; i++) {
		if (piece_count[i] > 0) {
			lastcount += piece_count[i];
		}
	}
	if (lastcount <= 2&&depth!=0) {
		test++;
		int got = countAvaEnd(depth, color, piece, piece_count);
		if (got > 0) {
			return 100000 - got * 1000;
		}
		else if (got < 0) {
			return -100000 + got * 1000;
		}
		else {
			return 0;
		}
	}

	uint64_t hashindex = hashvalue & (TABLE_SIZE-1);
	int tempscore = 0;
	int HashCheck = 0;
	unsigned int HashTempC1;
	unsigned int HashTempC2;
	try
	{	
		// Hash hit: hashtable[index].count != 0
		// HashHitSameTurn: Hash hit && (depth % 2 == hashtable[index].depth % 2)
		// Real Hit: HashHitSameTurn && 盤面相同
		// LessDepth: Real Hit && ((hashtablemap[hashindex].MaxDepth - hashtablemap[hashindex].depth) >= (maxDepth - depth)) && maxDepth % 2 == hashtablemap[hashindex].MaxDepth % 2
		// MoreDepth: Real Hit && depth > noReDepth && hashtablemap[hashindex].NextHashvalue
		// NoUseHash: Real Hit && ((hashtablemap[hashindex].MaxDepth - hashtablemap[hashindex].depth) >= (maxDepth - depth))
		// int board_score = hashtablemap[hashindex].count;
		// bool same_turn = (hashtablemap[hashindex].depth & 1 == depth & 1);
		// bool record_or_not = true;
		// bool search_depth = ((hashtablemap[hashindex].MaxDepth - hashtablemap[hashindex].depth) >= (maxDepth - depth));

		// for (int i = 0; i < 16; i++) {
		// 	if (piece[i] ^ hashtablemap[hashindex].piece[i]) { // 相同的話 xor == 0
		// 		record_or_not = false;
		// 		break;
		// 	}
		// }
		
		// if(board_score && same_turn && record_or_not && search_depth && maxDepth & 1 == hashtablemap[hashindex].MaxDepth & 1){
		// 	// state = REAL_HIT;
		// 	HashHit++;
		// 	HashHitSameTurn++;
		// 	RealHit++;
		// 	LessDepth++;
		// 	return hashtablemap[hashindex].count - (depth - hashtablemap[hashindex].depth);
		// }
		// else if(board_score && same_turn && record_or_not){
		// 	HashHit++;
		// 	HashHitSameTurn++;
		// 	RealHit++;
		// }
		// else if(board_score && same_turn && record_or_not && !search_depth && depth > noReDepth && hashtablemap[hashindex].NextHashvalue){
		// 	// state = DEPTH_SHALLOW;
		// 	HashHit++;
		// 	HashHitSameTurn++;
		// 	MoreDepth++;

		// 	HashCheck = 1;
		// 	int tempNextCurPie = hashtablemap[hashindex].NextCurPie;
		// 	int tempc1p = hashtablemap[hashindex].NextCurPiece[2];
		// 	int tempc2p = hashtablemap[hashindex].NextCurPiece[3];
		// 	if (tempNextCurPie != -1) {//sim
		// 		curPiece[hashtablemap[hashindex].NextCurPiece[2]] ^= hashtablemap[hashindex].NextCurPiece[0];//清除原位置c1
		// 		curPiece[hashtablemap[hashindex].NextCurPiece[2]] |= hashtablemap[hashindex].NextCurPiece[1];//移動
		// 		curPiece[0] |= hashtablemap[hashindex].NextCurPiece[0];//空格+c1
		// 		curPiece[hashtablemap[hashindex].NextCurPiece[3]] ^= hashtablemap[hashindex].NextCurPiece[1];//清除原位置c2
		// 		curPie[hashtablemap[hashindex].NextCurPie]--;
		// 	}
		// 	else {
		// 		curPiece[hashtablemap[hashindex].NextCurPiece[2]] ^= hashtablemap[hashindex].NextCurPiece[0];//清除原位置c1
		// 		curPiece[hashtablemap[hashindex].NextCurPiece[2]] |= hashtablemap[hashindex].NextCurPiece[1];//移動
		// 		curPiece[0] |= hashtablemap[hashindex].NextCurPiece[0];//空格+c1
		// 		curPiece[0] ^= hashtablemap[hashindex].NextCurPiece[1];//空格-c2
		// 	}

		// 	HashTempC1 = hashtablemap[hashindex].NextCurPiece[0];
		// 	HashTempC2 = hashtablemap[hashindex].NextCurPiece[1];
		// 	tempscore = searchnp(depth + 1, curPiece, curPie, alpha, beta, hashtablemap[hashindex].NextHashvalue);

		// 	if (tempNextCurPie != -1) {//unsim

		// 		curPiece[tempc1p] ^= HashTempC2;//清除原位置c2
		// 		curPiece[tempc1p] |= HashTempC1;//移動
		// 		curPiece[0] ^= HashTempC1;//空格-c1
		// 		curPiece[tempc2p] |= HashTempC2;//回原位置c2
		// 		curPie[tempNextCurPie]++;
		// 	}
		// 	else {
		// 		curPiece[tempc1p] ^= HashTempC2;//清除原位置c2
		// 		curPiece[0] |= HashTempC2;//空格+c2
		// 		curPiece[tempc1p] |= HashTempC1;//移動
		// 		curPiece[0] ^= HashTempC1;//空格-c1
		// 	}

		// 	if (depth % 2 == 0)//max
		// 	{
		// 		if (tempscore > alpha)
		// 		{
		// 			alpha = tempscore;
		// 		}

		// 	}
		// 	else//min
		// 	{
		// 		if (tempscore < beta)
		// 		{
		// 			beta = tempscore;
		// 		}
		// 	}
		// 	if (beta <= alpha)
		// 	{
		// 		hashtablemap[hashindex].depth = depth;
		// 		hashtablemap[hashindex].MaxDepth = maxDepth;
		// 		if (depth % 2 == 0) {//max
		// 			hashtablemap[hashindex].count = alpha;
		// 			return alpha;
		// 		}
		// 		else {//min
		// 			hashtablemap[hashindex].count = beta;
		// 			return beta;
		// 		}
		// 	}
		// }
		
		// else if(board_score && ((same_turn && !record_or_not) || !same_turn)){
		// 	// state = COLLISION;
		// 	HashHit++;
		// 	HashHitSameTurn++;
		// 	insertHash(piece, hashindex, depth);
		// }
		// else if(board_score){
		// 	// 新盤面
		// 	HashHit++;
		// 	insertHash(piece, hashindex, depth);
		// }
		// else{
		// 	// state = NOTHING;
		// 	insertHash(piece, hashindex, depth);
		// }
		if (hashtablemap[hashindex].count != 0) {
			HashHit++;
			if ((hashtablemap[hashindex].depth & 1) == (depth & 1)) {
				HashHitSameTurn++;
				int temp = 0;
				for (int i = 0; i < 16; i++) {
					if (curPiece[i] ^ hashtablemap[hashindex].piece[i]) {
						temp++;
						break;
					}
				}
				if (temp == 0) {
					RealHit++;
					if (((hashtablemap[hashindex].MaxDepth - hashtablemap[hashindex].depth) > (maxDepth - depth))) {
						if ((hashtablemap[hashindex].MaxDepth & 1) == (maxDepth & 1)) {
							//if (maxDepth == 7)cout << "!!" << endl;
							LessDepth++;
							return hashtablemap[hashindex].count - (depth - hashtablemap[hashindex].depth);
						}
						else {
							NoUseHash++;
						}
					}
					else {

						if (depth > noReDepth && hashtablemap[hashindex].NextHashvalue){
							MoreDepth++;

							if (0) {

							}
							else {

								HashCheck = 1;

								int tempNextCurPie = hashtablemap[hashindex].NextCurPie;
								int tempc1p = hashtablemap[hashindex].NextCurPiece[2];
								int tempc2p = hashtablemap[hashindex].NextCurPiece[3];
								if (tempNextCurPie != -1) {//sim

									curPiece[hashtablemap[hashindex].NextCurPiece[2]] ^= hashtablemap[hashindex].NextCurPiece[0];//清除原位置c1
									curPiece[hashtablemap[hashindex].NextCurPiece[2]] |= hashtablemap[hashindex].NextCurPiece[1];//移動
									curPiece[0] |= hashtablemap[hashindex].NextCurPiece[0];//空格+c1
									curPiece[hashtablemap[hashindex].NextCurPiece[3]] ^= hashtablemap[hashindex].NextCurPiece[1];//清除原位置c2
									curPie[hashtablemap[hashindex].NextCurPie]--;
								}
								else {

									curPiece[hashtablemap[hashindex].NextCurPiece[2]] ^= hashtablemap[hashindex].NextCurPiece[0];//清除原位置c1
									curPiece[hashtablemap[hashindex].NextCurPiece[2]] |= hashtablemap[hashindex].NextCurPiece[1];//移動
									curPiece[0] |= hashtablemap[hashindex].NextCurPiece[0];//空格+c1
									curPiece[0] ^= hashtablemap[hashindex].NextCurPiece[1];//空格-c2

								}


								HashTempC1 = hashtablemap[hashindex].NextCurPiece[0];
								HashTempC2 = hashtablemap[hashindex].NextCurPiece[1];

								unsigned int ctPiece[16];
								memcpy(ctPiece, curPiece, sizeof(ctPiece));
								int ctPie[14];
								memcpy(ctPie, curPie, sizeof(ctPie));
								tempscore = searchnp(depth + 1, ctPiece, ctPie, alpha, beta, hashtablemap[hashindex].NextHashvalue);

								if (tempNextCurPie != -1) {//unsim

									curPiece[tempc1p] ^= HashTempC2;//清除原位置c2
									curPiece[tempc1p] |= HashTempC1;//移動
									curPiece[0] ^= HashTempC1;//空格-c1
									curPiece[tempc2p] |= HashTempC2;//回原位置c2
									curPie[tempNextCurPie]++;
								}
								else {

									curPiece[tempc1p] ^= HashTempC2;//清除原位置c2
									curPiece[0] |= HashTempC2;//空格+c2
									curPiece[tempc1p] |= HashTempC1;//移動
									curPiece[0] ^= HashTempC1;//空格-c1
								}

								if ((depth & 1) == 0)//max
								{
									if (tempscore > alpha)
									{
										alpha = tempscore;
									}

								}
								else//min
								{
									if (tempscore < beta)
									{
										beta = tempscore;
									}
								}
								if (beta <= alpha)
								{
									hashtablemap[hashindex].depth = depth;
									hashtablemap[hashindex].MaxDepth = maxDepth;
									if ((depth & 1) == 0) {//max
										hashtablemap[hashindex].count = alpha;
										return alpha;
									}
									else {//min
										hashtablemap[hashindex].count = beta;
										return beta;
									}
								}
							}
						}
						else {
							NoUseHash++;
						}
					}
				}
				else {//hash collision
					hashtablemap[hashindex].NextHashvalue = 0;
					memcpy(hashtablemap[hashindex].piece, curPiece, sizeof(hashtablemap[hashindex].piece));
				}
			}
			else {//層數不同
				hashtablemap[hashindex].NextHashvalue = 0;
				memcpy(hashtablemap[hashindex].piece, curPiece, sizeof(hashtablemap[hashindex].piece));
			}
		}
		else {//新盤面
			memcpy(hashtablemap[hashindex].piece, curPiece, sizeof(hashtablemap[hashindex].piece));
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "searchnp" << e.what() << '\n';
	}

	chessnp(curPiece, depth);
	unsigned int taEM[100][2];//存可吃子的方法 0 src 1 dst 避免被往下搜尋時刷掉
	int tAEMi = AEMindex;//alleatmove index
	unsigned int taOM[100][2];//存可移動非吃子的方法 0 src 1 dst
	int tAOMi = AOMindex;//allonlymove index
	memcpy(taEM, allEatMove, sizeof(taEM));
	memcpy(taOM, allOnlyMove, sizeof(taOM));

	unsigned int weightU[200][2];//計算所有移動與翻棋的得分0src 1dst
	int weight[200];//計算所有移動與翻棋的得分
	int wp = 0;
	int best = -9999999;
	if ((depth & 1) == 1) {
		best = 9999999;
	}

	if (HashCheck) {
		weight[wp] = tempscore;
		best = tempscore;
		weightU[wp][0] = 100;
		weightU[wp][1] = 100;
		wp++;
	}

	if(tAEMi>100)
		tAEMi = 100;

	for (int i = 0; i < tAEMi; i++)//吃子
	{
		int deeper = depth + 1;
		int c1p, c2p = -1;
		unsigned int c1 = 1 << taEM[i][0];
		unsigned int c2 = 1 << taEM[i][1];
		for (int ii = 1; ii < 15; ii++) {//找到c1 放入c1p  
			unsigned int check = curPiece[ii] & c1;
			if (check != 0) {
				c1p = ii;
				break;
			}
		}
		for (int ii = 1; ii < 15; ii++) {//找c2
			unsigned int check = curPiece[ii] & c2;
			if (check != 0) {
				c2p = ii;
				break;
			}
		}
		if (c2p == -1 || c1p == -1) {//errrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
			cout << endl;
			cout << "err1 c1: " << hex << c1 << " c2: " << c2 << endl;
			/*for (int j = 0; j < 16; j++) {
				cout << hex << curPiece[j] << dec << endl;
			}*/
			cout << "depth: " << depth << endl;
			cout << endl;
			break;
		}
		curPiece[c1p] ^= c1;//清除原位置c1
		curPiece[c1p] |= c2;//移動
		curPiece[0] |= c1;//空格+c1
		curPiece[c2p] ^= c2;//清除原位置c2
		curPie[c2p - 1]--;
		unsigned int tempc1 = c1;
		unsigned int tempc2 = c2;
		unsigned int tempc1p = c1p;
		unsigned int tempc2p = c2p;
		int tempNextCurPie = c2p - 1;
		uint64_t tempNextHashvalue = hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c2p - 1][GetIndex(c2)] ^ randtable[c1p - 1][GetIndex(c2)];

		if (HashCheck == 1) {
			if (c1 == HashTempC1 && c2 == HashTempC2) {
				curPiece[c1p] ^= c2;//清除原位置c2
				curPiece[c1p] |= c1;//移動
				curPiece[0] ^= c1;//空格-c1
				curPiece[c2p] |= c2;//回原位置c2
				curPie[c2p - 1]++;
				continue;
			}
			else {
			}
		}
		weightU[wp][0] = taEM[i][0];
		weightU[wp][1] = taEM[i][1];
		unsigned int ctPiece1[16];
		memcpy(ctPiece1, curPiece, sizeof(ctPiece1));
		int ctPie1[14];
		memcpy(ctPie1, curPie, sizeof(ctPie1));
		weight[wp] = searchnp(deeper, ctPiece1, ctPie1, alpha, beta, hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c2p - 1][GetIndex(c2)] ^ randtable[c1p - 1][GetIndex(c2)]);
		if (depth == 0)
		{
			int r = rand() % 6;;
			string a[6] = { "⊙3⊙","(--;)","(〃ω〃)","(’-_-`)","|ω˙）","(*≧艸≦)" };
			cout << a[r] + ".";
		}

		curPiece[c1p] ^= c2;//清除原位置c2
		curPiece[c1p] |= c1;//移動
		curPiece[0] ^= c1;//空格-c1
		curPiece[c2p] |= c2;//回原位置c2
		curPie[c2p - 1]++;

		if ((depth & 1) == 0)//max
		{
			if (weight[wp] > alpha)
			{
				alpha = weight[wp];
			}
			if (weight[wp] > best)
			{
				best = weight[wp];
				hashtablemap[hashindex].NextCurPiece[0] = tempc1;
				hashtablemap[hashindex].NextCurPiece[1] = tempc2;
				hashtablemap[hashindex].NextCurPiece[2] = tempc1p;
				hashtablemap[hashindex].NextCurPiece[3] = tempc2p;
				hashtablemap[hashindex].NextCurPie = tempNextCurPie;
				hashtablemap[hashindex].NextHashvalue = tempNextHashvalue;
			}
		}
		else//min
		{
			if (weight[wp] < beta)
			{
				beta = weight[wp];
			}
			if (weight[wp] < best)
			{
				best = weight[wp];
				hashtablemap[hashindex].NextCurPiece[0] = tempc1;
				hashtablemap[hashindex].NextCurPiece[1] = tempc2;
				hashtablemap[hashindex].NextCurPiece[2] = tempc1p;
				hashtablemap[hashindex].NextCurPiece[3] = tempc2p;
				hashtablemap[hashindex].NextCurPie = tempNextCurPie;
				hashtablemap[hashindex].NextHashvalue = tempNextHashvalue;
			}
		}
		if (beta <= alpha)
		{
			hashtablemap[hashindex].depth = depth;
			hashtablemap[hashindex].MaxDepth = maxDepth;
			if ((depth & 1) == 0) {//max
				hashtablemap[hashindex].count = alpha;
				return alpha;
			}
			else {//min
				hashtablemap[hashindex].count = beta;
				return beta;
			}
		}
		wp++;
	}

	if (depth >= maxDepth)
	{
		int re = countAvanp(curPie, depth, curPiece);
		hashtablemap[hashindex].count = re;
		hashtablemap[hashindex].depth = depth;
		hashtablemap[hashindex].MaxDepth = maxDepth;
		return re;
	}
	if(tAOMi>100)
		tAOMi = 100;
	for (int i = 0; i < tAOMi; i++)//純移動 --------------------------------------------------------------------------------------------------------
	{
		int deeper = depth + 1;
		int c1p = -1;
		unsigned int c1 = 1 << taOM[i][0];
		unsigned int c2 = 1 << taOM[i][1];
		for (int ii = 1; ii < 15; ii++) {//找兵種
			unsigned int check = curPiece[ii] & c1;
			if (check != 0) {
				c1p = ii;
				break;
			}
		}

		if (c1p == -1) {//errrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr
			// cout << endl;
			cout << "err2 c1: " << hex << c1 << " c2: " << c2 << endl;
			// /*for (int j = 0; j < 16; j++) {
			// 	cout << hex << curPiece[j] << dec << endl;
			// }*/
			// cout << "depth: " << depth << endl;
			// cout << endl;
			break;
		}

		curPiece[c1p] ^= c1;//清除原位置c1
		curPiece[c1p] |= c2;//移動
		curPiece[0] |= c1;//空格+c1
		curPiece[0] ^= c2;//空格-c2

		unsigned int tempc1 = c1;
		unsigned int tempc2 = c2;
		unsigned int tempc1p = c1p;
		int tempNextCurPie = -1;
		uint64_t tempNextHashvalue = hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c1p - 1][GetIndex(c2)];


		if (HashCheck == 1) {
			if (c1 == HashTempC1 && c2 == HashTempC2) {
				curPiece[c1p] ^= c2;//清除原位置c2
				curPiece[0] |= c2;//空格+c2
				curPiece[c1p] |= c1;//移動
				curPiece[0] ^= c1;//空格-c1
				continue;
			}
			else {
			}
		}

		weightU[wp][0] = taOM[i][0];
		weightU[wp][1] = taOM[i][1];
		unsigned int ctPiece2[16];
		memcpy(ctPiece2, curPiece, sizeof(ctPiece2));
		int ctPie2[14];
		memcpy(ctPie2, curPie, sizeof(ctPie2));
		weight[wp] = searchnp(deeper, ctPiece2, ctPie2, alpha, beta, hashvalue ^ randtable[c1p - 1][GetIndex(c1)] ^ randtable[c1p - 1][GetIndex(c2)]);
		if (depth == 0)
		{
			int r = rand() % 6;;
			string a[6] = { "⊙3⊙","(--;)","(〃ω〃)","(’-_-`)","|ω˙）","(*≧艸≦)" };
			cout << a[r] + ".";
		}
		curPiece[c1p] ^= c2;//清除原位置c2
		curPiece[0] |= c2;//空格+c2
		curPiece[c1p] |= c1;//移動
		curPiece[0] ^= c1;//空格-c1


		if (depth % 2 == 0)//max
		{
			if (weight[wp] > best)
			{
				best = weight[wp];
				hashtablemap[hashindex].NextCurPiece[0] = tempc1;
				hashtablemap[hashindex].NextCurPiece[1] = tempc2;
				hashtablemap[hashindex].NextCurPiece[2] = tempc1p;
				hashtablemap[hashindex].NextCurPie = tempNextCurPie;
				hashtablemap[hashindex].NextHashvalue = tempNextHashvalue;
			}
			if (weight[wp] > alpha)
			{
				alpha = weight[wp];
			}
		}
		else//min
		{
			if (weight[wp] < best)
			{
				best = weight[wp];
				hashtablemap[hashindex].NextCurPiece[0] = tempc1;
				hashtablemap[hashindex].NextCurPiece[1] = tempc2;
				hashtablemap[hashindex].NextCurPiece[2] = tempc1p;
				hashtablemap[hashindex].NextCurPie = tempNextCurPie;
				hashtablemap[hashindex].NextHashvalue = tempNextHashvalue;
			}
			if (weight[wp] < beta)
			{
				beta = weight[wp];
			}
		}
		if (beta <= alpha)
		{
			hashtablemap[hashindex].depth = depth;
			hashtablemap[hashindex].MaxDepth = maxDepth;
			if (depth % 2 == 0) {//max
				hashtablemap[hashindex].count = alpha;
				// if(alpha < 0 && color == 0){
				// 	cout << "alpha\n"; 
				// 	print_piece();
				// }
				return alpha;
			}
			else {//min
				hashtablemap[hashindex].count = beta;
				// if(alpha < 0 && color == 0){
				// 	cout << "beta\n"; 
				// 	print_piece();
				// }
				return beta;
			}
		}
		wp++;
	}

	if (curPiece[15] != 0 && depth > noReDepth)//可以走空步
	{
		unsigned int ctPiece3[16];
		memcpy(ctPiece3, curPiece, sizeof(ctPiece3));
		int ctPie3[14];
		memcpy(ctPie3, curPie, sizeof(ctPie3));
		weightU[wp][0] = 0; weightU[wp][1] = 0; weight[wp] = searchnp(depth + 1, ctPiece3, ctPie3, alpha, beta, hashvalue);
		wp++;
	}

	if (depth == 0)//root
	{
		string src, dst;
		int srci, dsti;
		if (TimeOut == 1) {
			return 0;
		}
		cout << endl << "------------------------------------" << endl;
		int recordi = 0;
		for (int i = wp - 1; i >= 0; i--)
		{
			if (depth == 0)
			{
				cout << weightU[i][0] << " " << weightU[i][1] << " " << weight[i] << endl;
			}
			if (weight[i] == best)
			{
				recordi = i;
				srci = weightU[i][0];
				dsti = weightU[i][1];
			}
		}

		if (draw == 1)
		{
			int loseOne = 0;
			unsigned int c1 = 1 << srci;
			for (int ii = 1; ii < 15; ii++) {//找兵種
				unsigned int check = curPiece[ii] & c1;
				if (check != 0) {
					if (ii < 8) {
						loseOne = Power[curPie[7]][curPie[8]][curPie[9]][curPie[10]][curPie[11]][curPie[12]][curPie[13]][ii - 1];
					}
					else {
						loseOne = Power[curPie[0]][curPie[1]][curPie[2]][curPie[3]][curPie[4]][curPie[5]][curPie[6]][ii - 8];
					}
					break;
				}
			}

			if (best - loseOne < 0); //可能輸 故意平手? 
			else if (srci == past_walk[1][1] && dsti == past_walk[1][0])
			{
				cout << "draw denied" << endl;
				weight[recordi] -= 999999;//可能會贏 選擇不平手? 
				cout << weight[recordi] <<" "<< weightU[recordi][0] <<" "<< weightU[recordi][1] << "\n";
				best = weight[0];

				for (int ii = wp - 1; ii >= 0; ii--)//重新尋找 
				{
					if (weight[ii] > best)
					{
						best = weight[ii];
						srci = weightU[ii][0];
						dsti = weightU[ii][1];
					}
				}
			}
		}
		IndexToBoard(srci, dsti, src, dst);
		createMovetxt(src, dst, srci, dsti);
	}
	if (wp == 0) best = countAvanp(curPie, depth, curPiece);
	if (best == 9999999 || best == -9999999) best = countAvanp(curPie, depth, curPiece);
	hashtablemap[hashindex].count = best;
	hashtablemap[hashindex].depth = depth;
	hashtablemap[hashindex].MaxDepth = maxDepth;
	return best;
}

void MyAI::drawOrNot()
{
	draw = 0;
	int a = past_walk[0][0];
	int b = past_walk[0][1];
	int c = past_walk[1][1];
	int d = past_walk[1][0];
	if (a == past_walk[2][1] && a == past_walk[4][0] && a == past_walk[6][1] && b == past_walk[2][0] && b == past_walk[4][1] && b == past_walk[6][0] && c == past_walk[3][0] &&
		c == past_walk[5][1] && d == past_walk[3][1] && d == past_walk[5][0])
	{
		draw = 1;
		for (int i = 0; i < 7; i++)
		{
			printf("past_walk[%d]: %d %d\n", i, past_walk[i][0], past_walk[i][1]);
		}
		
	}	
	cout << draw << endl;
}

int MyAI::findPiece(int place, unsigned int curPiece[16])
{
	unsigned int bplace = 1 << place;
	for (int i = 1; i < 15; i++)//找14種棋中 誰在這個位置上 
	{
		if ((curPiece[i] & bplace) != 0)
		{
			i--;
			return i;//找到棋子回傳 
		}
	}
	return -1;//錯誤 
}

void MyAI::createMovetxt(string src, string dst, int srci, int dsti)//背景模式 
{
	src_out = src;
	dst_out = dst;
	cout << srci << "." << src << ":src " << dsti << "." << dst << ":dst " << endl;
}

void MyAI::IndexToBoard(int indexa, int indexb, string& src, string& dst)
{
	stringstream ss;
	string aa;
	int a = indexa / 4;//0~7
	int b = indexa % 4;//0~3 a~d
	if (b == 0) aa = "d";
	else if (b == 1) aa = "c";
	else if (b == 2) aa = "b";
	else if (b == 3) aa = "a";
	ss << 8 - a;
	src = aa + ss.str();
	//cout<<"src : "<<src<<endl;

	stringstream sss;
	a = indexb / 4;//0~7
	b = indexb % 4;//0~3 a~d
	if (b == 0) aa = "d";
	else if (b == 1) aa = "c";
	else if (b == 2) aa = "b";
	else if (b == 3) aa = "a";
	sss << 8 - a;
	dst = aa + sss.str();
	//cout<<"dst : "<<dst<<endl;
}

uint64_t MyAI::myhash(unsigned int tpiece[16]) {
	uint64_t hashvalue = 0;
	for (int i = 1; i < 16; i++) { //1~14 為雙方兵種 0空格 15 未翻
		uint64_t p = tpiece[i]; //取得棋子位置
		while (p) { //將1~15 號的子都搜尋一遍
			unsigned int mask = LS1B(p); //如果該棋子在多個位置,先取低位元的位置。
			p ^= mask; //除去位於最低位元的該兵種
			hashvalue ^= randtable[i - 1][GetIndex(mask)]; //將最低位元的兵種位置的隨機值xor hashtable
		}
	}
	return hashvalue;
}

int MyAI::factorial(int n) {
	return (n == 1 || n == 0) ? 1 : factorial(n - 1) * n;
}

int MyAI::combine(int N, int K)
{
	int count = 0;
	vector<char> bitmask;
	for (int i = 0; i < K; i++)
	{
		bitmask.push_back(1);
	}
	bitmask.resize(N, 0);
	// std::string bitmask(K, 1); // K leading 1's
	// bitmask.resize(N, 0); // N-K trailing 0's

	// print integers and permute bitmask
	do {
		for (int i = 0; i < N; ++i) // [0..N-1] integers
		{
			if (bitmask[i]) {
				//std::cout << " " << i;
				;
			}
		}
		//std::cout << std::endl;
		count++;
	} while (std::prev_permutation(bitmask.begin(), bitmask.end()));
	return count;
}


bool MyAI::isFileExists_ifstream(string& name) {
    ifstream f(name.c_str());
    return f.good();
}

int MyAI::findHash(unsigned int piece[16], uint64_t hashindex, int depth)
{
    hit_state state = checkHit(piece, hashindex, depth);
    if(state == REAL_HIT){
        cout << "real hit" << endl;
        return hashtable[hashindex].count - ((depth - hashtable[hashindex].depth));
    }
    else if(state == NOTHING){
        cout << "nothing" << endl;
    }
    else if(state == COLLISION){
        cout << "collision" << endl;
    }
    else if(state == DEPTH_SHALLOW){
        cout << "depth_shallow" << endl;
    }
    else {
        cout << "find wrong" << endl;
    }
    return 0;
}

hit_state MyAI::checkHit(unsigned int piece[16], uint64_t index, int depth)
{
    hit_state state;
    int board_score = hashtable[index].count;
    bool same_turn = (hashtable[index].depth % 2 == depth % 2);
    bool record_or_not = true;
    bool search_depth = ((hashtable[index].MaxDepth - hashtable[index].depth) >= (maxDepth - depth));

    for (int i = 0; i < 16; i++) {
		if (piece[i] ^ hashtable[index].piece[i]) { // 相同的話 xor == 0
            record_or_not = false;
			break;
		}
	}

    if(board_score && same_turn && record_or_not && search_depth){
        state = REAL_HIT;
    }
    else if(board_score && same_turn && record_or_not && !search_depth){
        state = DEPTH_SHALLOW;
    }
    else if(board_score && ((same_turn && !record_or_not) || !same_turn)){
        state = COLLISION;
    }
    else{
        state = NOTHING;
    }

    return state;
}

void MyAI::insertHash(unsigned int piece[16], uint64_t index, int depth)
{
    memcpy(hashtable[index].DCount, DCount, sizeof(DCount));
    memcpy(hashtable[index].piece, piece, sizeof(hashtable[index].piece));
    hashtable[index].depth = depth;
   	hashtable[index].MaxDepth = maxDepth;
}

void MyAI::initialHash()
{
    // memset(hashtable, 0, sizeof(hashtable));
    randtable[0][0] = 13058330919143008746ULL;
    randtable[0][1] = 3833225037332984403ULL;
    randtable[0][2] = 12990806369532008183ULL;
    randtable[0][3] = 16623699484139149084ULL;
    randtable[0][4] = 3636566626351378630ULL;
    randtable[0][5] = 363424178256930598ULL;
    randtable[0][6] = 937442682384648053ULL;
    randtable[0][7] = 1341587711808106729ULL;
    randtable[0][8] = 2528648021688166869ULL;
    randtable[0][9] = 5651333402785055355ULL;
    randtable[0][10] = 5813419925899044142ULL;
    randtable[0][11] = 3591688579815976383ULL;
    randtable[0][12] = 17024390052278267520ULL;
    randtable[0][13] = 3048690220529292369ULL;
    randtable[0][14] = 11539051077622549928ULL;
    randtable[0][15] = 3663855509500702043ULL;
    randtable[0][16] = 16160370132252570385ULL;
    randtable[0][17] = 6380499557626877674ULL;
    randtable[0][18] = 11984473119913524237ULL;
    randtable[0][19] = 3103222346078439235ULL;
    randtable[0][20] = 6575822191654178808ULL;
    randtable[0][21] = 1303905902914979286ULL;
    randtable[0][22] = 7652674858442427932ULL;
    randtable[0][23] = 2169399286442867980ULL;
    randtable[0][24] = 5523309156517855702ULL;
    randtable[0][25] = 7633818324726255780ULL;
    randtable[0][26] = 7409309509257671247ULL;
    randtable[0][27] = 14214208157014101143ULL;
    randtable[0][28] = 8843208416098906895ULL;
    randtable[0][29] = 5466114145255582917ULL;
    randtable[0][30] = 11645607645960288149ULL;
    randtable[0][31] = 4246149907460807828ULL;
    randtable[1][0] = 1305243549946091463ULL;
    randtable[1][1] = 5420140437650068908ULL;
    randtable[1][2] = 1062645313257992477ULL;
    randtable[1][3] = 10444530952931112561ULL;
    randtable[1][4] = 8546935033384224150ULL;
    randtable[1][5] = 10530415635933332457ULL;
    randtable[1][6] = 10742393449419720342ULL;
    randtable[1][7] = 14011161409547665580ULL;
    randtable[1][8] = 5167089109254438915ULL;
    randtable[1][9] = 10648754439877523841ULL;
    randtable[1][10] = 408642383553020275ULL;
    randtable[1][11] = 12756614674138773111ULL;
    randtable[1][12] = 16771093681176078535ULL;
    randtable[1][13] = 7597439052726201891ULL;
    randtable[1][14] = 3701021860919488554ULL;
    randtable[1][15] = 9661578602492371374ULL;
    randtable[1][16] = 14912699499106577189ULL;
    randtable[1][17] = 9082174819727309853ULL;
    randtable[1][18] = 17104978121234560555ULL;
    randtable[1][19] = 1748152704644123631ULL;
    randtable[1][20] = 1324406713838769308ULL;
    randtable[1][21] = 11356622182367812415ULL;
    randtable[1][22] = 11735155194336359606ULL;
    randtable[1][23] = 8170712658759645296ULL;
    randtable[1][24] = 8830447688263163576ULL;
    randtable[1][25] = 7957496822845435588ULL;
    randtable[1][26] = 12029358102086077008ULL;
    randtable[1][27] = 13652814329590019652ULL;
    randtable[1][28] = 5950942617482694940ULL;
    randtable[1][29] = 2737630323705049875ULL;
    randtable[1][30] = 10023645531327660089ULL;
    randtable[1][31] = 7670665603381051820ULL;
    randtable[2][0] = 3961748030674991114ULL;
    randtable[2][1] = 10222109058936580402ULL;
    randtable[2][2] = 5259538456057516250ULL;
    randtable[2][3] = 8629948670816320040ULL;
    randtable[2][4] = 15046086369240627025ULL;
    randtable[2][5] = 2654721274300991756ULL;
    randtable[2][6] = 14910187360444632156ULL;
    randtable[2][7] = 4386143315599250720ULL;
    randtable[2][8] = 2500402804588719190ULL;
    randtable[2][9] = 13146624956968033263ULL;
    randtable[2][10] = 16678153127559617474ULL;
    randtable[2][11] = 1966553134884888208ULL;
    randtable[2][12] = 14168099595037079203ULL;
    randtable[2][13] = 3835847793065818441ULL;
    randtable[2][14] = 17273467016987085845ULL;
    randtable[2][15] = 9335118089363201084ULL;
    randtable[2][16] = 2617421442179086331ULL;
    randtable[2][17] = 4642048539291633898ULL;
    randtable[2][18] = 14771078748791970714ULL;
    randtable[2][19] = 17324765234793704449ULL;
    randtable[2][20] = 16848773295515670953ULL;
    randtable[2][21] = 11310167420977779432ULL;
    randtable[2][22] = 17817435518705435379ULL;
    randtable[2][23] = 15133397041086871660ULL;
    randtable[2][24] = 2555272324416407518ULL;
    randtable[2][25] = 12188108146101869277ULL;
    randtable[2][26] = 11910352584911335033ULL;
    randtable[2][27] = 8078091072259284524ULL;
    randtable[2][28] = 11065182207593367756ULL;
    randtable[2][29] = 9851446044901034346ULL;
    randtable[2][30] = 12699174630969261824ULL;
    randtable[2][31] = 8466540665644714553ULL;
    randtable[3][0] = 7281233685170740575ULL;
    randtable[3][1] = 17732385302192388963ULL;
    randtable[3][2] = 8825608024257986160ULL;
    randtable[3][3] = 12439608764836459465ULL;
    randtable[3][4] = 11426818451870633905ULL;
    randtable[3][5] = 8288315290959969640ULL;
    randtable[3][6] = 9065840908703385215ULL;
    randtable[3][7] = 4020420957989961594ULL;
    randtable[3][8] = 474751579428870499ULL;
    randtable[3][9] = 13440845288441302252ULL;
    randtable[3][10] = 6487241249618108305ULL;
    randtable[3][11] = 11711180763097185595ULL;
    randtable[3][12] = 6035516951790548032ULL;
    randtable[3][13] = 10519321920467212913ULL;
    randtable[3][14] = 6364283773123077536ULL;
    randtable[3][15] = 10244572197571382318ULL;
    randtable[3][16] = 13810183562893643664ULL;
    randtable[3][17] = 1387787420074804384ULL;
    randtable[3][18] = 10082988203569013502ULL;
    randtable[3][19] = 15610646698223768964ULL;
    randtable[3][20] = 16272357913752173263ULL;
    randtable[3][21] = 8902144861089625434ULL;
    randtable[3][22] = 6954895762308041763ULL;
    randtable[3][23] = 3705414657936483774ULL;
    randtable[3][24] = 11799091117495017591ULL;
    randtable[3][25] = 13193761368988604446ULL;
    randtable[3][26] = 14235731995401386812ULL;
    randtable[3][27] = 15182472084153372660ULL;
    randtable[3][28] = 15959726282608010461ULL;
    randtable[3][29] = 18165772651259549128ULL;
    randtable[3][30] = 18083081227943331356ULL;
    randtable[3][31] = 16476895487031944975ULL;
    randtable[4][0] = 481189944271112055ULL;
    randtable[4][1] = 793580027807345073ULL;
    randtable[4][2] = 5678312190653750587ULL;
    randtable[4][3] = 5698967022343593883ULL;
    randtable[4][4] = 16767131024375155565ULL;
    randtable[4][5] = 12672626629625066214ULL;
    randtable[4][6] = 11046873820829645276ULL;
    randtable[4][7] = 9179709274810750888ULL;
    randtable[4][8] = 2042795143041095828ULL;
    randtable[4][9] = 15037955733069818519ULL;
    randtable[4][10] = 128438659443175854ULL;
    randtable[4][11] = 3948169102763718137ULL;
    randtable[4][12] = 1822400274654043421ULL;
    randtable[4][13] = 4359429690904738329ULL;
    randtable[4][14] = 8643641091925204286ULL;
    randtable[4][15] = 2784948350800310772ULL;
    randtable[4][16] = 13267123131418093145ULL;
    randtable[4][17] = 16585349926572385107ULL;
    randtable[4][18] = 61867933116442619ULL;
    randtable[4][19] = 3419406313111827225ULL;
    randtable[4][20] = 4269367494088191108ULL;
    randtable[4][21] = 14470441720875675640ULL;
    randtable[4][22] = 12295493969456278520ULL;
    randtable[4][23] = 3014107560631389032ULL;
    randtable[4][24] = 12571253828085035690ULL;
    randtable[4][25] = 1338600988702472939ULL;
    randtable[4][26] = 17202148300320785402ULL;
    randtable[4][27] = 392506613721589378ULL;
    randtable[4][28] = 2707541735850616705ULL;
    randtable[4][29] = 5411657176289375540ULL;
    randtable[4][30] = 7803991973106924050ULL;
    randtable[4][31] = 962723074621628653ULL;
    randtable[5][0] = 10613785153269777990ULL;
    randtable[5][1] = 3670338849979409221ULL;
    randtable[5][2] = 9393229948038993591ULL;
    randtable[5][3] = 15326209245539535905ULL;
    randtable[5][4] = 7767172624694222410ULL;
    randtable[5][5] = 5778596206382141567ULL;
    randtable[5][6] = 4099781297041464647ULL;
    randtable[5][7] = 6492734226824614967ULL;
    randtable[5][8] = 16914497638187704439ULL;
    randtable[5][9] = 16432675884508568740ULL;
    randtable[5][10] = 17498291908845183348ULL;
    randtable[5][11] = 15102517229495818770ULL;
    randtable[5][12] = 6901435531745276400ULL;
    randtable[5][13] = 1324681068614622205ULL;
    randtable[5][14] = 8151502434022710466ULL;
    randtable[5][15] = 13991754413987467232ULL;
    randtable[5][16] = 444169866464998059ULL;
    randtable[5][17] = 6687532940311101792ULL;
    randtable[5][18] = 1188770426751326336ULL;
    randtable[5][19] = 10162773588651728662ULL;
    randtable[5][20] = 10619083508494805481ULL;
    randtable[5][21] = 14071563196635326562ULL;
    randtable[5][22] = 2612573668741503942ULL;
    randtable[5][23] = 2635176830458088027ULL;
    randtable[5][24] = 15472080828354273581ULL;
    randtable[5][25] = 2779746775627318566ULL;
    randtable[5][26] = 6008443456194903232ULL;
    randtable[5][27] = 15512367835945949316ULL;
    randtable[5][28] = 4591812417881260450ULL;
    randtable[5][29] = 5259465064765512644ULL;
    randtable[5][30] = 8384827397093705670ULL;
    randtable[5][31] = 2172509263903057606ULL;
    randtable[6][0] = 11985426563282702052ULL;
    randtable[6][1] = 16477026954806080875ULL;
    randtable[6][2] = 6870579634012007358ULL;
    randtable[6][3] = 8774546840358026619ULL;
    randtable[6][4] = 6498894959244037270ULL;
    randtable[6][5] = 16792329624364579111ULL;
    randtable[6][6] = 10609489581114825365ULL;
    randtable[6][7] = 13983077344294535274ULL;
    randtable[6][8] = 7434739909594431155ULL;
    randtable[6][9] = 12343924426644335942ULL;
    randtable[6][10] = 15208873931894845427ULL;
    randtable[6][11] = 1683285385410527776ULL;
    randtable[6][12] = 11962055460958840417ULL;
    randtable[6][13] = 11499510282495685729ULL;
    randtable[6][14] = 10397632441227359694ULL;
    randtable[6][15] = 16118014350100958465ULL;
    randtable[6][16] = 16681509708156133291ULL;
    randtable[6][17] = 17066276617729603088ULL;
    randtable[6][18] = 14663006186265433794ULL;
    randtable[6][19] = 15494038518765414546ULL;
    randtable[6][20] = 11534383051572484590ULL;
    randtable[6][21] = 10939740082622888376ULL;
    randtable[6][22] = 3406875471691537008ULL;
    randtable[6][23] = 2421168959928094152ULL;
    randtable[6][24] = 15850494350130499803ULL;
    randtable[6][25] = 5861674846442712375ULL;
    randtable[6][26] = 12814314712403092597ULL;
    randtable[6][27] = 5506669693090498159ULL;
    randtable[6][28] = 17023801415976054433ULL;
    randtable[6][29] = 281383699728389687ULL;
    randtable[6][30] = 5237526895779581369ULL;
    randtable[6][31] = 6515601283101797762ULL;
    randtable[7][0] = 15810218499953724202ULL;
    randtable[7][1] = 133173818501966368ULL;
    randtable[7][2] = 14381597988246203201ULL;
    randtable[7][3] = 7977642253311527506ULL;
    randtable[7][4] = 16347320424067696653ULL;
    randtable[7][5] = 15779624783801180038ULL;
    randtable[7][6] = 12475667316257587897ULL;
    randtable[7][7] = 5467915513019938778ULL;
    randtable[7][8] = 12610299422566982571ULL;
    randtable[7][9] = 18207481030017173089ULL;
    randtable[7][10] = 829622024060471642ULL;
    randtable[7][11] = 11552902195603258352ULL;
    randtable[7][12] = 5328491054482196102ULL;
    randtable[7][13] = 14296714663966292623ULL;
    randtable[7][14] = 15157574654912572059ULL;
    randtable[7][15] = 4481509091880022198ULL;
    randtable[7][16] = 1045719511828802312ULL;
    randtable[7][17] = 12503958758253196206ULL;
    randtable[7][18] = 15342994858060074316ULL;
    randtable[7][19] = 16464632568773109725ULL;
    randtable[7][20] = 15332529645400703022ULL;
    randtable[7][21] = 14315437662597914929ULL;
    randtable[7][22] = 18027897910978798300ULL;
    randtable[7][23] = 1235485325783128562ULL;
    randtable[7][24] = 2959933723443586168ULL;
    randtable[7][25] = 4917316648030111597ULL;
    randtable[7][26] = 6123964747651831231ULL;
    randtable[7][27] = 8832614754395967178ULL;
    randtable[7][28] = 13080463628568645036ULL;
    randtable[7][29] = 16360620022845644192ULL;
    randtable[7][30] = 16515931753044352530ULL;
    randtable[7][31] = 11250738614035646787ULL;
    randtable[8][0] = 12933527767081091651ULL;
    randtable[8][1] = 12213122761488680258ULL;
    randtable[8][2] = 12713096776935834106ULL;
    randtable[8][3] = 4343740963408336970ULL;
    randtable[8][4] = 7624267962474455381ULL;
    randtable[8][5] = 7786757424349238064ULL;
    randtable[8][6] = 4267222626929328268ULL;
    randtable[8][7] = 5759472947092316343ULL;
    randtable[8][8] = 8941898520884817578ULL;
    randtable[8][9] = 316841584459622583ULL;
    randtable[8][10] = 2401402268158860331ULL;
    randtable[8][11] = 17888575682792358518ULL;
    randtable[8][12] = 5111453912392466809ULL;
    randtable[8][13] = 15728508058887236617ULL;
    randtable[8][14] = 5471711119053842333ULL;
    randtable[8][15] = 17420737817925199996ULL;
    randtable[8][16] = 15677887260993552682ULL;
    randtable[8][17] = 2480381142311255606ULL;
    randtable[8][18] = 4845192168778593790ULL;
    randtable[8][19] = 14477144457315490357ULL;
    randtable[8][20] = 9993126101958789385ULL;
    randtable[8][21] = 8892017806689312673ULL;
    randtable[8][22] = 5080088034940905171ULL;
    randtable[8][23] = 17465755095549768814ULL;
    randtable[8][24] = 17037098388863969945ULL;
    randtable[8][25] = 10849420917224880533ULL;
    randtable[8][26] = 1286183938529281042ULL;
    randtable[8][27] = 76908241875540449ULL;
    randtable[8][28] = 9350962329169718179ULL;
    randtable[8][29] = 17877210749491202595ULL;
    randtable[8][30] = 15060287160424812238ULL;
    randtable[8][31] = 18412894827895497333ULL;
    randtable[9][0] = 8276567229782422150ULL;
    randtable[9][1] = 11889564954994552178ULL;
    randtable[9][2] = 14937323683653286156ULL;
    randtable[9][3] = 2825751026251094827ULL;
    randtable[9][4] = 15616869407934554072ULL;
    randtable[9][5] = 10153983917580912712ULL;
    randtable[9][6] = 18073720611950400199ULL;
    randtable[9][7] = 146210821222324411ULL;
    randtable[9][8] = 13643405751809880215ULL;
    randtable[9][9] = 9402535892477777786ULL;
    randtable[9][10] = 8950297002224371063ULL;
    randtable[9][11] = 14331812423251658261ULL;
    randtable[9][12] = 4192415757912282360ULL;
    randtable[9][13] = 17212280242444951988ULL;
    randtable[9][14] = 15409026223830318978ULL;
    randtable[9][15] = 5296819004618765949ULL;
    randtable[9][16] = 7629378788107736079ULL;
    randtable[9][17] = 8540092492167546525ULL;
    randtable[9][18] = 13616709931488321765ULL;
    randtable[9][19] = 7933504273000213654ULL;
    randtable[9][20] = 15765328436829935858ULL;
    randtable[9][21] = 3462127016525742790ULL;
    randtable[9][22] = 14666604726842480301ULL;
    randtable[9][23] = 14548750338344142969ULL;
    randtable[9][24] = 49272119993976736ULL;
    randtable[9][25] = 16755644491141977901ULL;
    randtable[9][26] = 12732930165636647140ULL;
    randtable[9][27] = 5768030395688467657ULL;
    randtable[9][28] = 6054280016102826981ULL;
    randtable[9][29] = 7740339783244767990ULL;
    randtable[9][30] = 14422607050601809936ULL;
    randtable[9][31] = 11825729150171258085ULL;
    randtable[10][0] = 4184182582966499865ULL;
    randtable[10][1] = 7292934033817405568ULL;
    randtable[10][2] = 1971881298178867238ULL;
    randtable[10][3] = 13377305978404588354ULL;
    randtable[10][4] = 326746566242875688ULL;
    randtable[10][5] = 4211537526230201656ULL;
    randtable[10][6] = 479207539784995596ULL;
    randtable[10][7] = 7912920985497715888ULL;
    randtable[10][8] = 11804163326055813291ULL;
    randtable[10][9] = 16909969514483561077ULL;
    randtable[10][10] = 542650493327798452ULL;
    randtable[10][11] = 17194516794456897303ULL;
    randtable[10][12] = 9123832579727683486ULL;
    randtable[10][13] = 12416395054021917238ULL;
    randtable[10][14] = 4416800462536221086ULL;
    randtable[10][15] = 10389303536531317274ULL;
    randtable[10][16] = 15610476405095550732ULL;
    randtable[10][17] = 17353673448339122599ULL;
    randtable[10][18] = 3646317169238470320ULL;
    randtable[10][19] = 13548494327220609083ULL;
    randtable[10][20] = 7662172214150310667ULL;
    randtable[10][21] = 13610844705460755098ULL;
    randtable[10][22] = 16818021903928532656ULL;
    randtable[10][23] = 15234735346173699329ULL;
    randtable[10][24] = 14559847457365740858ULL;
    randtable[10][25] = 17369562645134288431ULL;
    randtable[10][26] = 12695272645653571027ULL;
    randtable[10][27] = 8641139419190952598ULL;
    randtable[10][28] = 5937954854473014205ULL;
    randtable[10][29] = 17557141811554457619ULL;
    randtable[10][30] = 10052895458699407411ULL;
    randtable[10][31] = 11240756814045364347ULL;
    randtable[11][0] = 5934772389533583098ULL;
    randtable[11][1] = 10326398104976038233ULL;
    randtable[11][2] = 1612906573330869575ULL;
    randtable[11][3] = 15469796176220774141ULL;
    randtable[11][4] = 17548100294189677143ULL;
    randtable[11][5] = 17476933730533557735ULL;
    randtable[11][6] = 14145515757531730278ULL;
    randtable[11][7] = 14415181877855392713ULL;
    randtable[11][8] = 14329532907986803880ULL;
    randtable[11][9] = 9582404533136591541ULL;
    randtable[11][10] = 5218159172320909323ULL;
    randtable[11][11] = 16706016621932732584ULL;
    randtable[11][12] = 13578605120206428815ULL;
    randtable[11][13] = 15383062254677872382ULL;
    randtable[11][14] = 12483221260814620077ULL;
    randtable[11][15] = 7934840213947878777ULL;
    randtable[11][16] = 2760336130614082241ULL;
    randtable[11][17] = 7722678963393545008ULL;
    randtable[11][18] = 1938106562023132172ULL;
    randtable[11][19] = 11141456864239080538ULL;
    randtable[11][20] = 8331296879350683002ULL;
    randtable[11][21] = 12738560035777734277ULL;
    randtable[11][22] = 4002397424819444978ULL;
    randtable[11][23] = 6301627990596005101ULL;
    randtable[11][24] = 10944741699293940111ULL;
    randtable[11][25] = 9472039515290129303ULL;
    randtable[11][26] = 9459913305980605003ULL;
    randtable[11][27] = 17539860353519124726ULL;
    randtable[11][28] = 12300791231500342541ULL;
    randtable[11][29] = 12513106169883927239ULL;
    randtable[11][30] = 2837651583572864365ULL;
    randtable[11][31] = 14350427291018338915ULL;
    randtable[12][0] = 13299529516443065128ULL;
    randtable[12][1] = 15832304828493931133ULL;
    randtable[12][2] = 12210365815576900664ULL;
    randtable[12][3] = 6260701303108518134ULL;
    randtable[12][4] = 216225744834077618ULL;
    randtable[12][5] = 12092610928924389460ULL;
    randtable[12][6] = 2811299573574614547ULL;
    randtable[12][7] = 9958095510734269743ULL;
    randtable[12][8] = 1555092455422279171ULL;
    randtable[12][9] = 874578740606328287ULL;
    randtable[12][10] = 8569405878687007152ULL;
    randtable[12][11] = 14234126420340920638ULL;
    randtable[12][12] = 6792603662650347877ULL;
    randtable[12][13] = 15489262172436851724ULL;
    randtable[12][14] = 8208021198942574287ULL;
    randtable[12][15] = 14311703726189790251ULL;
    randtable[12][16] = 3373290786923769552ULL;
    randtable[12][17] = 6662039310351954022ULL;
    randtable[12][18] = 3097685019266590096ULL;
    randtable[12][19] = 3355396001436082456ULL;
    randtable[12][20] = 15529139007404975459ULL;
    randtable[12][21] = 16388815781283157742ULL;
    randtable[12][22] = 8768774073571982327ULL;
    randtable[12][23] = 8479303585159231399ULL;
    randtable[12][24] = 16517027874581706660ULL;
    randtable[12][25] = 2833486257237493935ULL;
    randtable[12][26] = 10523581016554658028ULL;
    randtable[12][27] = 13907643420475274961ULL;
    randtable[12][28] = 18359448880680465791ULL;
    randtable[12][29] = 2913790423977353303ULL;
    randtable[12][30] = 9192008153873699321ULL;
    randtable[12][31] = 12810595947820547469ULL;
    randtable[13][0] = 1099347389010464221ULL;
    randtable[13][1] = 17799503232529155539ULL;
    randtable[13][2] = 1361285451059641199ULL;
    randtable[13][3] = 14787528218091333240ULL;
    randtable[13][4] = 3610184933912876764ULL;
    randtable[13][5] = 18433289169944928394ULL;
    randtable[13][6] = 3445562488666039023ULL;
    randtable[13][7] = 9845608414056839816ULL;
    randtable[13][8] = 8101963102995028615ULL;
    randtable[13][9] = 11443860904730634709ULL;
    randtable[13][10] = 13657622016138382585ULL;
    randtable[13][11] = 18026038712098468717ULL;
    randtable[13][12] = 3973903631085455869ULL;
    randtable[13][13] = 861727910135307765ULL;
    randtable[13][14] = 11742808770466882259ULL;
    randtable[13][15] = 11244092707692321367ULL;
    randtable[13][16] = 6104273681070332226ULL;
    randtable[13][17] = 10459241688290077800ULL;
    randtable[13][18] = 5188418318275805563ULL;
    randtable[13][19] = 14312515732481954946ULL;
    randtable[13][20] = 7640010578360404449ULL;
    randtable[13][21] = 3010115936850930326ULL;
    randtable[13][22] = 17890148828272411804ULL;
    randtable[13][23] = 5414648804924253377ULL;
    randtable[13][24] = 2871772345524047945ULL;
    randtable[13][25] = 5748870628001283836ULL;
    randtable[13][26] = 11189810846018889685ULL;
    randtable[13][27] = 4527012988848435258ULL;
    randtable[13][28] = 10733955204683513834ULL;
    randtable[13][29] = 9872000280137844862ULL;
    randtable[13][30] = 15850475474830423230ULL;
    randtable[13][31] = 9973955588997485423ULL;
    randtable[14][0] = 14939871281147262540ULL;
    randtable[14][1] = 3272387981065893504ULL;
    randtable[14][2] = 12746098451291790550ULL;
    randtable[14][3] = 10319978203904022183ULL;
    randtable[14][4] = 11098378575250184594ULL;
    randtable[14][5] = 2857893902370674337ULL;
    randtable[14][6] = 15909362709983522649ULL;
    randtable[14][7] = 17612815418224808130ULL;
    randtable[14][8] = 17959702703656842043ULL;
    randtable[14][9] = 14391711203754584950ULL;
    randtable[14][10] = 2762994672133086102ULL;
    randtable[14][11] = 8449114598598299826ULL;
    randtable[14][12] = 16190217053284788507ULL;
    randtable[14][13] = 968460599008038445ULL;
    randtable[14][14] = 13099147347978243951ULL;
    randtable[14][15] = 8077779902598405814ULL;
    randtable[14][16] = 8622777132289594737ULL;
    randtable[14][17] = 635914683952350312ULL;
    randtable[14][18] = 8320109581002661463ULL;
    randtable[14][19] = 13744486708990945403ULL;
    randtable[14][20] = 4989946775201336339ULL;
    randtable[14][21] = 15575118098595143528ULL;
    randtable[14][22] = 7593816854962059499ULL;
    randtable[14][23] = 8836613565438265561ULL;
    randtable[14][24] = 6844248575053626275ULL;
    randtable[14][25] = 11973232896903749912ULL;
    randtable[14][26] = 15543354967481307481ULL;
    randtable[14][27] = 4172552855253581109ULL;
    randtable[14][28] = 5742117526750088551ULL;
    randtable[14][29] = 11818918816134188825ULL;
    randtable[14][30] = 12230067171914521441ULL;
    randtable[14][31] = 813544324000426941ULL;
}