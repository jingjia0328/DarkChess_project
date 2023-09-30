#include "Protocol.h"

Protocol::Protocol()
{ 
} // end Protocol()

bool Protocol::init_protocol(const char* ip,const int port)
{
	if(!this->m_csock.InitSocket(ip, port))
		return false;
	return true;
} // end init_protocol(const char* ip,const int port)

Protocol::~Protocol(void)
{
} // end ~Protocol(void)

void Protocol::init_board(int piece_count[14], char current_position[32], struct History& history, int &time)
{ 
	char *buffer= NULL; 
	if(this->m_csock.Recieve(&buffer)){
		if( strstr(buffer, "/start") != NULL){
			Start(&buffer[strlen("/start")+1], piece_count, current_position, history, time);	
		}
	}
	this->m_csock.Send("/start");
	if(buffer != NULL) free(buffer); buffer = NULL;
} // end init_board(int piece_count[14], int current_position[32], History& history, int &time)


void Protocol::Start(const char* state, int piece_count[], char current_position[], History& history,int &time)
{ 
	const int  darkPiece[14] = {1, 2, 2, 2, 2, 2, 5, 1, 2, 2, 2, 2, 2, 5};
	char *tmp = NULL, *token = NULL;  

	if(state != NULL){
		tmp = (char*)malloc(sizeof(char)*strlen(state)+1);
		strcpy(tmp, state);
		// initial board
		token = strtok(tmp, ",") ;
		current_position[28] = (*token);  
		for(int i = 1; i < 32; ++i){
			token = strtok(NULL, ",");
			current_position[(7-i/4)*4+i%4] = (*token); 
		}	 
		// initial alive piece
		for(int i = 0; i < 14; ++i){
			token = strtok(NULL, ",");
			piece_count[i] = atoi(token); 
		}	 
	}else{
		for(int i = 0; i < 32; ++i) current_position[i] = 'X'; // DarkChess
		for(int i = 0; i < 14; ++i) piece_count[i] = darkPiece[i];
	} 
	// moveRecord
	token = strtok(NULL, ",");
	history.number_of_moves = atoi(token);  
	if(history.number_of_moves){
		history.move = (char**)malloc(sizeof(char*)*history.number_of_moves); 
		for(int i = 0; i < history.number_of_moves; ++i){
			history.move[i] = (char*)malloc(sizeof(char)*6);
			token = strtok(NULL, ","); 
			if(token[2] == '-') 
				sprintf(history.move[i], "%c%c-%c%c",token[0], token[1], token[3], token[4]);
			else
				sprintf(history.move[i], "%c%c(%c)",token[0], token[1], token[3]); 
			history.move[i][5] = '\0'; 
			//fprintf(stderr, "[%d.%s, %d-%d, %s]\n", i, history.move[i], src, dst, token); 
		} 
	} 
	//回合制時間
	token = strtok(NULL, ",");
	if(token != NULL) time = atoi(token);
	fprintf(stderr, "time: %d (ms)\n",time); 
} // end Start(const char* state, int piece_count[], char current_position[], History& history,int &time)

void Protocol::get_turn(bool &turn, PROTO_CLR &color)
{
	char *buffer = NULL;
	if(this->m_csock.Recieve(&buffer)){
		if( strstr(buffer, "/turn") !=NULL ){
			turn  = (buffer[6] == '0')?false:true;
			color = (buffer[8] == '0')?PCLR_RED:((buffer[8]=='1')?PCLR_BLACK:PCLR_UNKNOW);
		}
	} 
	this->m_csock.Send("/turn");
	if(buffer != NULL) free(buffer); buffer = NULL;
} // end get_turn(int &turn, int &color)

void Protocol::send(const char src[3], const char dst[3])
{ 
	char buffer[1 + BUFFER_SIZE] = {0}; 
	if(strcmp(src, dst)) 
		sprintf(buffer,"/move %c%c-%c%c", src[0], src[1], dst[0], dst[1]);
	else
		sprintf(buffer,"/flip %c%c", src[0], src[1]); 
	//fprintf(stderr, "send=%s\n", buffer);
	this->m_csock.Send(buffer);
} // end send(const char src[3], const char dst[3])

void Protocol::send(const char move[6])
{ 
	char buffer[1 + BUFFER_SIZE] = {0};  
	if(move[0] == move[3] && move[1] == move[4]) // Flip src = dst
		sprintf(buffer, "/flip %c%c", move[0], move[1]);
	else
		sprintf(buffer, "/move %c%c-%c%c", move[0], move[1], move[3], move[4]);
	//fprintf(stderr, "send=%s\n", buffer);
	this->m_csock.Send(buffer);
} // end send(const char move[6])

void Protocol::recv(char move[6], int &time, bool &isLast)
{
	char *buffer = NULL;
	char srcX, srcY, dstX, dstY, fin;
  int flag;
	if(this->m_csock.Recieve(&buffer)){
		//fprintf(stderr, "recv=%s\n", buffer);
		if(strstr(buffer, "/move") != NULL){
      sscanf(buffer,"%*s %c%c%*c%c%c %d %d", &srcX, &srcY, &dstX, &dstY, &time, &flag); 
      sprintf(move,"%c%c-%c%c", srcX, srcY, dstX, dstY);
      isLast = (flag ? true : false);
      fprintf(stderr, "remaining time = %d ms\n",time);
    }else if(strstr(buffer, "/flip") != NULL){
      sscanf(buffer, "%*s %c%c%*c%c%*c %d %d", &srcX, &srcY, &fin, &time, &flag); 
      sprintf(move,"%c%c(%c)", srcX, srcY, fin);
      isLast = (flag ? true : false);
      fprintf(stderr, "remaining time = %d ms\n",time);
    }else if(strstr(buffer, "/exit") != NULL){
      if(buffer != NULL) free(buffer); buffer = NULL;
      fprintf(stderr, "Receive Exit Signal.\n"); 
      exit(EXIT_SUCCESS);
    }
	}else{
    fprintf(stderr, "Socket Receive Error.\n");
    exit(EXIT_FAILURE);
  }
	if(buffer != NULL) free(buffer); buffer = NULL;
} // end recv(char mov[6])

PROTO_CLR Protocol::get_color(const char move[6])
{
	static const char skind[]={'K', 'G', 'M', 'R', 'N', 'C', 'P', 'k', 'g', 'm', 'r', 'n', 'c', 'p'};
	if (move[2] != '(') {fprintf(stderr, "get_color error\n"); return PCLR_UNKNOW;}
	for (int i = 0; i < 14; ++i) {
		if (move[3] == skind[i])
			return (i<7)?PCLR_RED:PCLR_BLACK;
	}
	return PCLR_UNKNOW;
} // end get_color(char move[6])
