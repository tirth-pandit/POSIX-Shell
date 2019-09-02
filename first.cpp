#include<iostream>
#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h>
#include<bits/stdc++.h>
#include <unistd.h> 
#include <fcntl.h> 

using namespace std;

#define clear() printf("\033[H\033[J") 

void start_shell()
{
	clear();


	sleep(1);
	clear();
}

void prompt()
{
   string shell;
  
   char pd[500] ;
   getcwd(pd, sizeof(pd))	;
   
   char hostname[100];
   char username[100];

   gethostname(hostname, 100);
   getlogin_r(username, 100); 

   cout<<username<<"@"<<hostname<<" :~ "<<pd<<" $ ";	
}

string  read_command()
{
	string temp;
	getline(cin,temp);
	return temp;
}

void execute_com(char **args)
{
	int pid = fork();

	if ( pid == 0 ) 
	{
		execvp( args[0], args );
		exit(1);
	}
	else if( pid > 0 )
	{
		wait(&pid);
	}
	else
	{
		cout<<"Process Can't be created "<<endl;
	}
}


void clean_str(char *str)
{
	if( str[strlen(str)-1]==' ' || str[strlen(str)-1]=='\n' )
	{	
		cout<<"after";
		str[strlen(str)-1]='\0';
	}

	if( str[0]==' ' || str[0]=='\n') 
	{
		cout<<"before";
		memmove(str, str+1, strlen(str));
	}
}

void make_command(char *buf ,char **args ,int *argc ,const char *del)
{
	char *word;
	word = strtok(buf ,del);

	int count = 0;

	while(word != NULL)
	{
		args[count] = (char *)malloc( sizeof(word) + 1);
		strcpy(args[count] ,word);

		//cout<<args[count]<<" "<<strlen(args[count])<<endl;
		clean_str(args[count]);
		word = strtok(NULL,del);
		count++;
	}


	args[count] = NULL;

	*argc = count;
}

void redirect_append(char **para ,int count)
{
	int para_count;

	char *args[100];

	clean_str(para[1]);
	clean_str(para[0]);

	cout<<para[1]<<" "<<strlen(para[1])<<endl;
	make_command(para[0],args ,&para_count ," ");

	int pid = fork();

	if( pid == 0 )
	{
		int fd = open(para[1] ,O_CREAT || O_WRONLY || O_APPEND );

		if( fd < 0 )
		{
			cout<<"Cannot open file ";
			return;
		}

		
		//A tricky use of dup2() system call: As in dup2(), in place of newfd any file descriptor can be put. 
		//Below is a C implementation in which the file descriptor of Standard output (stdout) is used. 
		//This will lead all the printf() statements to be written in the file referred by the old file descriptor.
		
		dup2(fd,1);

		execvp(args[0] ,args);

		cout<<"Invalid Input"<<endl;
		exit(1);
	}

	wait(NULL);
}

void redirect(char **para ,int count)
{
	int para_count;

	char *args[100];

	clean_str(para[1]);
	clean_str(para[0]);

	cout<<para[1]<<" "<<strlen(para[1])<<endl;
	make_command(para[0],args ,&para_count ," ");

	int pid = fork();

	if( pid == 0 )
	{
		int fd = open(para[1] ,O_CREAT | O_WRONLY);

		if( fd < 0 )
		{
			cout<<"Cannot open file ";
			return;
		}

		
		//A tricky use of dup2() system call: As in dup2(), in place of newfd any file descriptor can be put. 
		//Below is a C implementation in which the file descriptor of Standard output (stdout) is used. 
		//This will lead all the printf() statements to be written in the file referred by the old file descriptor.
		
		dup2(fd,1);

		execvp(args[0] ,args);

		cout<<"Invalid Input"<<endl;
		exit(1);
	}

	wait(NULL);
}

main()
{
	start_shell();

	char buf[1000] ;
	char *args[100] ;
	int argc =0;

	while(1)
	{
		prompt();

		fgets(buf,1000 ,stdin);

		if( strstr(buf ,">>"))
		{
			make_command(buf ,args ,&argc ,">>");

			if( argc == 2 )
			{
				redirect_append(args ,argc );
			}
			else
			{
				cout<<"Redirection Error"<<endl;
			}
		}
		if( strstr(buf ,">"))
		{
			make_command(buf ,args ,&argc ,">>");

			if( argc == 2 )
			{
				redirect(args ,argc);
			}
			else
			{
				cout<<"Redirection Error"<<endl;
			}
		}
		else
		{
			make_command(buf ,args ,&argc ," ");
        
			if( strcmp(args[0],"cd") == 0 )
			{
				chdir(args[1]);
			}
			else
			{
				execute_com(args);
			}
		}
	}
}