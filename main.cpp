#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "main.h"
#include "cportfel.h"
#include "get_wig20.h"
#include "get_nasdaq.h"

CPortfel *m_portfels;
struct SIndexes indexes = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

char g_buf[BUFSIZE];
int interval = 10;

char* gettime()
{
    struct tm *pczas;
    time_t czas;
	
	(void) time(&czas);
	pczas = localtime(&czas);

	sprintf( g_buf, "%4d.%.2d.%.2d_%.2d.%.2d:%.2d",
			 pczas->tm_year + 1900,
			 pczas->tm_mon+1,
			 pczas->tm_mday,
			 pczas->tm_hour,
			 pczas->tm_min,
			 pczas->tm_sec );

	return g_buf;
}

bool init()
{
	//currently WIG20 only
	CPortfel *prev,*cur;
	m_portfels = NULL;
	cur = new CPortfel("WIG20");
	//cur->GetCashFromFile((char*)"");
	if(m_portfels == NULL)
	{
		m_portfels = prev = cur;
	}
	else
	{
		prev->next = cur;
		prev = cur;
	}	
	return true;
}

// never called
void Destroy()
{
	CPortfel *cur = m_portfels;
	CPortfel *next = m_portfels;

	while( cur != NULL )
	{
		next = cur->next;
		delete cur;
		cur = next;
	}

}

int update_wig20()
{
	FILE* plik = NULL;
	CPortfel *cur = m_portfels;
	int ret = 0;

	while( cur != NULL)
	{
		if(cur->GetName() == "WIG20")
		{
			plik = fopen("files/wig20.txt","r");
			if( plik )
			{
				char buf[BUFSIZE];
				//	fgets(buf,BUFSIZE,plik);
				while(!feof(plik))
				{
					fgets(buf,BUFSIZE,plik);
					if( cur->Update( buf ) )
					{
						ret = 1;            // something has changed
					}
				}
				fclose(plik);
			}
			cur->Print();
		}
		cur=cur->next;
	}
	
	return ret;
}

void save_wig20()
{
	CPortfel *cur = m_portfels;
	if(cur->GetName() == "WIG20")
	{
		cur->Save();
		DPF( "WIG20 SAVED!\n" );
	}
	else
	{
		printf( "Unknown portfel: %s!\n", cur->GetName().c_str());
	}
}

void read_config()
{
	char buf[BUFSIZE] = "";
	int value = 0;
	int line = 0;

	FILE* plik = fopen( "./config/config.txt","r");
	if( plik )
	{
		while( !feof( plik ) )
		{
			++line;
			fgets( buf, BUFSIZE, plik );
			if( strlen(buf) )
			{
				value = atoi( buf );
				switch( line )
				{
				case 1:
					interval = value; 
					break;
				default: break;
				}
			}
		}
		fclose(plik);
	}
	else
	{
		printf( "cannot read file config.txt\n" );
	}
	printf( "sleep time is %d min\n", interval );
}

void copy_wig20(struct tm *pczas)
{
	char buf[BUFSIZE];

	printf("czas: %4d.%.2d.%.2d %.2d:%.2d wday: %d\n",
		   pczas->tm_year + 1900,
		   pczas->tm_mon+1,
		   pczas->tm_mday,
		   pczas->tm_hour,
		   pczas->tm_min,
		   pczas->tm_wday );
	
	sprintf( buf, "cp files/wig20.txt notowania/%4d.%.2d.%.2d_%.2d.%.2d_WIG20.txt\n",
			 pczas->tm_year + 1900,
			 pczas->tm_mon+1,
			 pczas->tm_mday,
			 pczas->tm_hour,
			 pczas->tm_min );
	
	printf( "doing: %s\n", buf);
	system( buf );
}

int start_of_day()
{
 	log((char*)"start of day: %s\n", gettime() );

	double nasdaq_change = 0.0;
	
	nasdaq_change = get_indexes((SIndexes*) &indexes );
 	log((char*)"nasdaq_change: %.2f\n", nasdaq_change );

	print_indexes(indexes);

	CPortfel *por = m_portfels;

	while(por!=NULL)
	{
		por->Changed();
		por->StartOfDay( nasdaq_change );
		por=por->next;
	}

	return 0;
}

int end_of_day()
{
 	log((char*)"end of day: %s\n", gettime() );

	CPortfel *por = m_portfels;
	while(por!=NULL)
	{
		por->Changed();
		por->EndOfDay();
		por=por->next;
	}

	return 0;
}

void au(int sig)
{
    logf((char*) "files/signals.txt", (char*)"\nSignal %d received at %s\n", sig, gettime() );
    printf("\nSignal %d received\n", sig );
	
	int c = getchar();
	
	switch(c)
	{
	case 's' : 
		update_wig20();
		start_of_day(); 
		break;
	case 'e' : 
		end_of_day(); 
		break;
	}
	printf( "pressed %c\n", c );
}

int main()
{
    struct tm *pczas;
    time_t czas;
	struct sigaction act;
	
	log((char*) "\n\nStart %s\n", gettime() );
    system( "mkdir -p files" );
    system( "mkdir -p notowania" );

	// 	start_of_day();
	
    act.sa_handler = au;
    sigemptyset( &act.sa_mask );
    act.sa_flags = 0;
	
    sigaction(SIGINT, &act, 0);

	if( !init() )
	{
		logf( (char*)"files/errors.txt", 
			  (char*)"cannot init application, missed file %s, time %s exiting...\n",
			  "config/portfels.txt",
			  gettime() );
		return 0 ;
	}

	read_config();

	get_wig20();
	update_wig20( );
	get_indexes((SIndexes*) &indexes );	
	bool market_closed = true;

    while(1)
    {
		(void) time(&czas);
		pczas = localtime(&czas);
		
		// get wig20 every 10 min between 9 and 16 h
		// every weekday
		if( pczas->tm_wday != 0  &&
			pczas->tm_wday != 6  &&
			pczas->tm_hour >= 9  &&
			pczas->tm_hour <= 16 )
		{
			log( (char*)"getting wig20 from pb,: %s\n", gettime() );
			get_wig20();
// 			copy_wig20( pczas );
			if( update_wig20() )
			{
				if( market_closed )
				{
					printf("STARTOFDAY\n");
					start_of_day();
					market_closed = false;
				}
			}
		}
		else
		{
		    printf( "not copying file wday: %.2d hour %.2d, min: %.2d, min %% 10: %.2d\n",
			pczas->tm_wday, pczas->tm_hour, pczas->tm_min, pczas-> tm_min%10);
			market_closed = true;
		}

		// END OF DAY
 		if( pczas->tm_wday != 0  &&
 			pczas->tm_wday != 6  &&
 			pczas->tm_hour == 16 &&
 			pczas->tm_min >= 30 &&
 			pczas->tm_min <= 39 )
		{
			printf("ENDOFDAY\n");
			update_wig20();
			end_of_day();          // sell and go away
		}
		
		save_wig20();
        sleep( 60 * interval ); // sleep for few min (10 or read from config)
    }
    pause();
	Destroy();
    printf("QUIT...\n");
    return 0;
}
