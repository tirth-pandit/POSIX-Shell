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
		//cout<<"after";
		str[strlen(str)-1]='\0';
	}

	if( str[0]==' ' || str[0]=='\n') 
	{
		//cout<<"before";
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

int count_pipes(char *buf)
{
	int c = 0;

	for(int i=0 ;i<strlen(buf) ;i++)
	{
		if( buf[i] == '|' )
		{
			c++;
		}
	}

	return c;
}

void piped(char **para ,int para_count ,int no_of_pipes)
{
	int fd1[2]; 
    int fd2[2];

    for(int i=0 ;i<para_count;i++)
    {
    	char *args[100];
    	int argc;

    	make_command(para[i] ,args ,&argc," ");

    	if( i%2 == 0 )
    	{
    		pipe(fd2);
    	}
    	else
    	{
    		pipe(fd1);
    	}

    	int pid = fork();

    	if(pid==-1)
    	{			
            if (i != para_count - 1)
            {
                if (i % 2 != 0)
                {
                    close(fd1[1]); 
                }
                else
                {
                    close(fd2[1]); 
                } 
            }			
            printf("Child process could not be created\n");
            return;
        }

    	if( pid == 0 )
    	{
    		if( i==0 )
    		{
    			dup2(fd2[1] ,STDOUT_FILENO);
    		}
    		else if( i==para_count-1)
    		{
    			if( para_count%2 != 0 )
    			{
    				dup2(fd1[0] ,STDIN_FILENO);
    			}
    			else
    			{
    				dup2(fd2[0] ,STDIN_FILENO);
    			}
    		}
    		else
    		{
    			if( i%2 != 0 )
    			{
    				dup2(fd2[0] ,STDIN_FILENO);
    				dup2(fd1[1] ,STDOUT_FILENO);
    			}
    			else
    			{
    				dup2(fd1[0] ,STDIN_FILENO);
    				dup2(fd2[1] ,STDOUT_FILENO);

    			}
    		}

    		execvp(args[0] ,args);
    	}

    	if( i==0 )
    	{
    		close(fd2[1]);
    	}
    	else if( i==para_count - 1 )
    	{
    		if( para_count%2 != 0 )
    		{
    			close(fd1[0]);
    		}
    		else
    		{
    			close(fd2[0]);
    		}
    	}
    	else
    	{
    		if( i%2 != 0 )
    		{
    			close(fd2[0]);
    			close(fd1[1]);
    		}
    		else
    		{
    			close(fd1[0]);
    			close(fd2[1]);
    		}
    	}

    	waitpid(pid,NULL,0);
    }
	/*--------------------------------------------------------------------------------
	 int fd[2] ;

	 if( pipe(fd) == -1 )
	 {
	 	cout<<"Pipe Error"<<endl;
	 	return;
	 }

	 char *args1[100] ,*args2[100];
	 int argc1 ,argc2;

	 make_command(para[1] ,args1 ,&argc1," ");
	 make_command(para[0] ,args2 ,&argc2," ");

	 int pid2 = fork();
	 //ls
	 if( pid2 == 0 )
	 {
	 	close(fd[0]);

	 	dup2(fd[1] ,1 );
	 	close(fd[0]);
	 	close(fd[1]);

	 	execvp(args2[0] ,args2);
	 }

	 int pid1 = fork();
	 //grep
	 if( pid1 == 0 )
	 {
	 	close(fd[1]);
	 	dup2( fd[0] ,0 );

	 	close( fd[0]);
	 	close( fd[1]);

	 	execvp(args1[0],args1);
	 }

	 close(fd[0]);
	 close(fd[1]);

	  while (wait(NULL) > 0);
    ------------------------------------------------------------------------------------------------*/

	/*int fd[para_count][2],i ,pc;

	for(i=0;i<para_count;i++)
	{
		char *args[100];
	    int argc;
		make_command(para[i],args,&argc," ");

		if(i!=para_count-1){
			if(pipe(fd[i])<0){
				perror("pipe creating was not successfull\n");
				return;
			}
		}
		if(fork()==0)
		{//child1
			if(i!=para_count-1){
				dup2(fd[i][1],1);
				//close(fd[i][0]);
				close(fd[i][1]);
			}

			if(i!=0)
			{
				dup2(fd[i-1][0],0);
				close(fd[i-1][1]);
				close(fd[i-1][0]);
			}
			
			execvp(args[0],args);
			perror("invalid Pipe ");
			exit(1);//in case exec is not successfull, exit
		}
		//parent
		if(i!=0)
		{//second process
			close(fd[i-1][0]);
			close(fd[i-1][1]);
		}
		wait(NULL);
	}
	--------------------------------------------------------------------
	*/


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

		if( strstr(buf ,"|"))
		{
			int pipes = count_pipes(buf);
			cout<<" Pipes : "<<pipes<<endl;

			make_command(buf ,args ,&argc ,"|");
			piped(args ,argc ,pipes);
		}
		else if( strstr(buf ,">>"))
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
		else if( strstr(buf ,">"))
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