#pragma once

#define BUFSIZE 255

#define WOLUMENS 2

struct SIndexes{
		double nasdaq;
		double djia;
		double s_p_500;
		double nikkei;
		double hangseng;
		double cac_40;
		double ftse;
};

//log to file files/makler_log.txt
void log( const char* fmt, ... );

//log to file files/makler_log.txt and print to stdout
void logp(const char* fmt, ... );

//log to file filename
void logf( const char* filename, const char* fmt, ... );

//log to file filename and print to stdout
void logfp( const char* filename, const char* fmt, ... );

//replace each occurence of char a in string str with char b
void replace(char *str, char a, char b);

//replace each occurence of char a in string str with char b
void remove(char *str, char a);

// cut from str substring starting with char, ending with char b
void cut(char *str, char a, char b);

char* gettime();

#ifdef DEBUG
#define DPF printf
#else
#define DPF //printf 
#endif
