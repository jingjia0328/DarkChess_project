////////////////////////////////////////////////////////////////////////////
// Edited Time: 2019/12/06                                                //
// 1. add isLast flag to prevent it from generating more one step         //
// 2. terminate process when an error occur during receiving socket data  //
////////////////////////////////////////////////////////////////////////////
#include <cstdio>
#include <cstdlib> 
#include "Protocol.h" 
#include "MyAI.h"
#include "windows.h"
#define DIV 1024
#define WIDTH 7

void atexit_handler(){
  // do something before exit
  fprintf(stderr, "Closing.\n"); 
  fflush(stderr);
}

int main(int argc , char **argv)
{
	Protocol protocol;  
	switch(argc){
  case 2:
	case 3:  
		if(!protocol.init_protocol(argv[argc-2], atoi(argv[argc-1]))) return 0;
		break; 
	} 
	
	MyAI myai; 
	char current_position[32], move[6];
	int  piece_count[14];
	PROTO_CLR color;
	History history;
	bool turn;  
  	bool isLast; // if this step is the last step
	int time; //表示剩餘時間 單位為毫秒(ms)

	if(argc==3 || argc==2)
	{  
    atexit(atexit_handler);
    
		protocol.init_board(piece_count, current_position, history,time);

		if(history.number_of_moves != 0)
		{
			puts("-----------Record-----------");
			for(int i=0;i<history.number_of_moves;i++)
			{
				printf("%d:%s\n",i,history.move[i]);
			}	
		} 

		myai.initialBoard(piece_count, current_position);   
		protocol.get_turn(turn, color);
		myai.Color(color);  
		if(turn)                           // my turn is First
		{
			myai.generateMove(move);
			protocol.send(move);           // my flip = "b6-b6" or move = "a2-a3"
			protocol.recv(move,time,isLast);	   // my flip = "b6(K)" or move = "a2-a3";			time = 剩餘時間(ms)		
			if(color == PCLR_UNKNOW)       // all pieces are unrevealed/hidden
			{
				color = protocol.get_color(move);
				myai.Color(color); 
			}
			myai.MakeMove(move);	
			protocol.recv(move,time,isLast);      // opponent flip = "c8(g)" or move = "c4-c5";	time = 剩餘時間(ms)		
			myai.MakeMove(move);
		}
		else                               // my turn is Second
		{ 
			protocol.recv(move,time,isLast);      // opponent flip = "c8(g)" or move = "c4-c5"		time = 剩餘時間(ms)		
			if(color == PCLR_UNKNOW)       // all pieces are unrevealed/hidden
			{ 
				color = (protocol.get_color(move)==PCLR_BLACK)?PCLR_RED:PCLR_BLACK; 
				myai.Color(color);
			}
			myai.MakeMove(move);
		}
    
    // To prevent it from generating more one step after receiving the last step
    // add isLast flag to avoid this situation
		while(!isLast)
		{
			/*MEMORYSTATUSEX statex;
 
			statex.dwLength = sizeof (statex);
			
			GlobalMemoryStatusEx (&statex);
			
			printf ("There is  %*ld percent of memory in use.\n",
					WIDTH, statex.dwMemoryLoad);
			printf ("There are %*I64d total Kbytes of physical memory.\n",
					WIDTH, statex.ullTotalPhys/DIV);
			printf ("There are %*I64d free Kbytes of physical memory.\n",
					WIDTH, statex.ullAvailPhys/DIV);
			printf ("There are %*I64d total Kbytes of paging file.\n",
					WIDTH, statex.ullTotalPageFile/DIV);
			printf ("There are %*I64d free Kbytes of paging file.\n",
					WIDTH, statex.ullAvailPageFile/DIV);
			printf ("There are %*I64d total Kbytes of virtual memory.\n",
					WIDTH, statex.ullTotalVirtual/DIV);
			printf ("There are %*I64d free Kbytes of virtual memory.\n",
					WIDTH, statex.ullAvailVirtual/DIV);
			
			// Show the amount of extended memory available.
			
			printf ("There are %*I64d free Kbytes of extended memory.\n",
					WIDTH, statex.ullAvailExtendedVirtual/DIV);*/
			myai.generateMove(move);   
			protocol.send(move);           // my flip = "b6-b6" or move = "a2-a3"
			protocol.recv(move,time,isLast);      // my flip = "b6(K)" or move = "a2-a3"			time = 剩餘時間(ms)		
			myai.MakeMove(move);           // ponder 
			if(isLast) break;
			protocol.recv(move,time,isLast);	   //												time = 剩餘時間(ms)		
			myai.MakeMove(move);           // opponent flip = "c8(g)" or move = "c4-c5"
		}  
	} 
	return 0;
} // end main(int argc , char **argv)
