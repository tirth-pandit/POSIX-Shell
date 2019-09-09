#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <string>
#include "trie.cpp"

using namespace std;

#define DEL (127)
#define ASCII_ESC 27

void show(trie *root ,char* buffer,int top,string msg)
{
  cout<<endl;
  root->print_subtree(string(buffer));

	write(STDOUT_FILENO, msg.c_str(),msg.size());
	write(STDOUT_FILENO,buffer,top);
}

void dis(struct termios initial_state) 
{
  tcsetattr(STDIN_FILENO, TCSADRAIN, &initial_state);
}

struct termios ena() 
{
	struct termios init;
  tcgetattr(STDIN_FILENO, &init);
  
  struct termios r = init;
  
  r.c_lflag &= ~(ECHO | ICANON);
  r.c_cc[VMIN] = 0;
  r.c_cc[VTIME] = 1;
  
  tcsetattr(STDIN_FILENO, TCSADRAIN, &r);
  return init;
}

void check(trie *root ,int c ,int &top ,int &f1,int &f2 ,string msg ,char* buf)
{ 
    if( c == DEL )
    {
        if( top > 0 )
        {
            top--;
            write(STDOUT_FILENO, "\033[1D",4);
            write(STDOUT_FILENO, "\033[0K",4);
        }

        f1 = 0;
    }
    else if( c == '\t')
    {
        //trie 
        show(root ,buf ,top ,msg);
        f1 = 0;
    }
    else if( c== '\n')
    {
        write(STDOUT_FILENO ,"\n" ,1);
        f1 = 0;
        f2 = 0;
    }
    else{}
}

int get_key(trie *root ,char* buf,int &top, string msg) 
{
  char c;
  int flag=1;
  int e=1;

  while ( read(STDIN_FILENO, &c, 1) != 1){}

  check(root ,c ,top ,flag ,e ,msg ,buf);
  
  if(flag != 0)
  {
  	  write(STDOUT_FILENO,&c, 1);
  	  buf[top++]=c;
  }

  if(!e)
  {		
      buf[top]='\0';
  }
  
  return e;
}

void sendinput(trie *root ,char* buf,string msg)
{
	int i=1;
	int top=0;

	write(STDOUT_FILENO, msg.c_str(),msg.size());
  
  while (i) 
  {
		i = get_key(root ,buf,top,msg);
	}
}