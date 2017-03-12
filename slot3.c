#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#define NUM 5                            //NUM is the number of reels(columns)
#define ROW 3
#define LINE 5
#define SPAN 20

uint_least8_t payline[ROW][NUM]={{0x09, 0x01, 0x11, 0x01, 0x09},
								 {0x02, 0x1A, 0x02, 0x1A, 0x02},
								 {0x14, 0x04, 0x0C, 0x04, 0x14}};

char question(char *pstr);

void set_slot(uint_least8_t [][NUM]);

void animate_slot(uint_least8_t, uint_least8_t [][NUM]);

void print_slot(uint_least8_t [][NUM]);

void show_winnings(uint_least8_t [][NUM]);

int main(void)
{
	char spin, cycle=0, slow=0;

	uint_least8_t next=0, slot[ROW][NUM];

	srand(time(NULL));

	#ifdef _WIN32
	system("cls");
	#else
	system("clear");
	#endif

	spin=question("Hello! Welcome to my slot game!\n\nAre you ready to play? Yes/No: ");

	while(cycle<8*NUM && spin != 'n' && spin != 'N')
	{
		#ifdef _WIN32
		system("cls");
		#else
		system("clear");
		#endif

		puts("Let's play:");

		if(cycle==0)
			set_slot(slot);
		else
			animate_slot(next, slot);

		if(cycle==8*NUM-1)
			show_winnings(slot);
		else
		{
			print_slot(slot);
		}

		++cycle;
		if(cycle%8==0)
			++next;

		if(cycle==8*NUM)
		{
			spin=question("Spin again? yes/no: ");

			if(spin=='y'||spin=='Y'||spin=='\n')
			{
				cycle=0;
				next=0;
				spin=0;
				slow=0;
			}
		}
		else
		{
		fflush(stdout);
		++slow;
		usleep(slow*12100);
		}
	}

	printf("\nGood Bye");
	fflush(stdout);
	sleep(2);
	#ifdef _WIN32
	system("cls");
	#else
	system("clear");
	#endif

	return(0);
}

char question(char *pstr)
{
	char spin='\0', f=0;

	while(spin!='y' && spin!='n' && spin!='Y' && spin!='N' && spin!='\n')
	{
		if(f)
			puts("\nYour response is invalid. Please try again.\n");

		printf("%s", pstr);
		scanf("%c", &spin);

		if(spin!='\n')
			while(getchar()!='\n')
				continue;

		f=1;
	}

	return spin;	
}

void set_slot(uint_least8_t slot[][NUM])
{
	int_least8_t n, m, x, y, count;

	for(m=ROW-1; m>-1; m--)
	{
		for(n=0; n<NUM; n++)
		{
			slot[m][n]=rand()%SPAN+1;
			for(x=m, count=0; x>0; x--)
			{
				for(y=n; y>0; y--)
				{
					if(slot[x-1][y-1]==slot[m][n])
					{
						++count;
						if(count<5)
						{
							x=1;
							y=1;
							count=0;
							m=ROW-1;
							n=0;
						}
					}
				}
			}
		}
	}
}

void animate_slot(uint_least8_t next, uint_least8_t slot[][NUM])
{
	int_least8_t n, m;

	for(m=ROW-1; m>0; m--)
	{
		for( n=0; n<NUM; n++ )
		{
			if(n<next)
				n=next;
 
			slot[m][n]=slot[m-1][n];
			if(m==1)
			{
				slot[m-1][n]=rand()%SPAN+1;
			}
		}
	}
}

void print_slot(uint_least8_t slot[][NUM])
{
	int_least8_t m, n;

	for( m=0; m<ROW; m++)
	{
		puts("\n");
		for( n=0; n<NUM; n++ )
			printf("\t %2d ", slot[m][n]);
		puts("\n\n");
	}
}

void unique_wilds(uint_least8_t *wilds)
{
	int_least8_t n, x, b=1;

	for(n=0; n<NUM; n++)
	{
		wilds[n]=rand()%SPAN+1;
		if(n>0)
		{
			for(x=n; x>0; x--)
			{
				if(wilds[x-1]==wilds[n])
				{
					wilds[n]=rand()%SPAN+1;
					x=n+1;
				}
			}
		}
	}

	while(b)
	{
		b=0;
		for(n=0; n<NUM-1; n++)
		{
			if(wilds[n]>wilds[n+1])
			{
				x=wilds[n];
				wilds[n]=wilds[n+1];
				wilds[n+1]=x;
				b=1;
			}
		}
	}
}

void print_dollar(int_least8_t l, int_least8_t m, int_least8_t n, uint_least8_t *start, uint_least8_t *match)
{
	if(n>(start[l]-1) && n<(start[l]+match[l]) && ((payline[m][n]>>l)&1))
	{
		printf("$");
	}
	else
	{
		printf(" ");
	}
}

int_least8_t unique_slots(uint_least8_t *unique, uint_least8_t *wilds, uint_least8_t slot[][NUM])
{
	int_least8_t m, n, x, w, y=0;

	for(m=0; m<ROW; m++)
	{
		for(n=0; n<NUM; n++, y++)
		{
			unique[y]=slot[m][n];
			if(y==0)
			{
				for(w=0; w<NUM; w++)
				{
					if(unique[0]==wilds[w])
					{
						w=NUM;
						--y;
					}
				}
			}
			else
			{
				for(x=y; x>0; x--)
				{
					for(w=0; w<NUM; w++)
					{
						if(unique[x-1]==unique[y]||wilds[w]==unique[y])
						{
							x=0;
							w=NUM;
						}
					}
					if(x==0)
						--y;
				}
			}
		}
	}

	return y;
}

uint_least16_t track_wilds(uint_least16_t track, uint_least8_t *wilds, uint_least8_t slot[][NUM])
{
	int_least8_t m, n, w;

	for(m=(ROW-1); m>-1; m--)
	{
		for(n=(NUM-1); n>-1; n--)
		{
			for(w=0; w<NUM; w++)
			{
				if(slot[m][n]==wilds[w])
				{
					track|=1;
				}
			}
			track<<=1;
		}
	}
	track>>=1;

	return track;
}

void set_line(uint_least8_t l, uint_least8_t u, uint_least16_t t, uint_least8_t *line, uint_least8_t slot[][NUM])
{
	uint_least8_t m, n;

	for(n=0; n<NUM; n++)
	{
		for(m=0; m<ROW; m++)
		{
			if((payline[m][n]>>l)&1)
			{
				if((t>>(n+m*5))&1)
					line[n]=u;
				else
					line[n]=slot[m][n];
			}
		}
	}
}

void match_line(int_least8_t l, uint_least8_t *line, uint_least8_t *start, uint_least8_t *match)
{
	int_least8_t n, s, t;

	for(n=0, s=0, t=1; n<NUM-1; n++)
	{
		if(match[l]<NUM)
		{
			if(line[n]!=line[n+1])
			{
				t=1;
				s=n+1;
			}
			else
			{
				++t;
				if(t>match[l] && t>2)
				{
					match[l]=t;
					start[l]=s;
				}
			}
		}
	}
}

void match_slots(uint_least8_t *start, uint_least8_t *match, uint_least8_t *wilds, uint_least8_t slot[][NUM])
{
	int_least8_t l, x, y;

	uint_least8_t line[NUM], unique[ROW*NUM];

	uint_least16_t track=0;

	unique_wilds(wilds);
	track=track_wilds(track, wilds, slot);
	y=unique_slots(unique, wilds, slot);

	for(x=0; x<y; x++)
	{
		for(l=0; l<LINE; l++)
		{
			set_line(l, unique[x], track, line, slot);
			match_line(l, line, start, match);
		}
	}
}

void print_payline(int_least8_t y, uint_least8_t *start, uint_least8_t *match)
{
	int_least8_t m, n, l;

	for(m=0; m<ROW; m++)
	{
		for(l=0; l<y+1; l++)
		{
			if(match[l])
			{
				for(n=0; n<NUM; n++)
				{
					if(payline[m][n]>>l&1)
					{
						if(n>(start[l]-1) && n<(start[l]+match[l]))
							printf("$");
						else
							printf("0");
					}
					else
						printf("-");
				}
				printf(" ");
			}
		}
		printf("\n\t  ");
	}
}

void show_winnings(uint_least8_t slot[][NUM])
{
	uint_least8_t l, m, n, score=0, track, match[LINE]={0}, start[LINE]={0}, wilds[NUM];
	
	static int_least64_t total=0;


	match_slots(start, match, wilds, slot);
	
	for(l=0; l<LINE; l++)
	{
		if(match[l])
		{
			score+=match[l];
			track=l;
		}
	}
	total+=score;

	if(score)
	{
		for(l=0; l<LINE; l++)
		{
			if(match[l])
			{
				for(m=0; m<ROW; m++)
				{
					puts("");
					for(n=0; n<NUM; n++)
					{
						if(n>(start[l]-1) && n<(start[l]+match[l]) && ((payline[m][n]>>l)&1))
							printf("\t $$");
						else
							printf("\t");
					}
					puts("");
					for(n=0; n<NUM; n++)
					{
						printf("\t");
						print_dollar(l, m, n, start, match);
						printf("%2d", slot[m][n]);
						print_dollar(l, m, n, start, match);
					}
					if(m==1)
						printf("\tThis line pays: $%d.00", match[l]);
					if(m==2)
						printf("\t      WINNINGS: $%d.00", score);
					puts("");
					for(n=0; n<NUM; n++)
					{
						if(n>(start[l]-1) && n<(start[l]+match[l]) && ((payline[m][n]>>l)&1))
							printf("\t $$");
						else
							printf("\t");
					}
					if(m==2)
						puts("");
					else
						puts("\n");			
				}
				printf("\t\t\t\t\t\t  TOTAL PAYOUT: $%lld.00\n", total);
				
				printf("Paylines: ");
				
				print_payline(l, start, match);
				
				puts("");						
			
				printf("   Wilds: ");
				for(n=0; n<NUM-1; n++)
				{
					printf("%2d, ", wilds[n]);
				}
				printf("%2d\n\n", wilds[NUM-1]);
				if(l<track)
				{
					fflush(stdout);
					sleep(2);
					#ifdef _WIN32
					system("cls");
					#else
					system("clear");
					#endif
					puts("Let's play:");
				}
			}
		}
	}
	else
	{
		print_slot(slot);
		
		printf("\nYour wilds are: ");
		for(n=0; n<NUM-1; n++)
		{
			printf("%2d, ", wilds[n]);
		}
		printf("%2d", wilds[NUM-1]);
		
		printf("\t\tYou have no winnings.\n\n");
		
		printf("TOTAL PAYOUT: $%lld.00\n\n", total);
	}
}
