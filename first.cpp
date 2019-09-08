#include<iostream>
#include<stdio.h>
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h>
#include<bits/stdc++.h>
#include<fcntl.h> 
#include<time.h> 
#include<fstream>
#include <sstream>  
#include <time.h> 
#include <chrono>
#include <string>
#include <termios.h>
#include<dirent.h>
#include <typeinfo>
#include "getinput.cpp"

using namespace std;

#define clear() printf("\033[H\033[J") 

map<string ,string> m;
map<string ,string> alias;
map<string ,string> ext;

trie *root = new trie();

int e_code;
int exit_code;

string script_file;
int scr_flag = 0;

vector<long long int> alarms;

vector<string> split (string s, string delimiter) 
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find (delimiter, pos_start)) != string::npos) 
    {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

void start_shell()
{
	clear();
	sleep(1);
	clear();
}

void write_rc()
{
	string path = m["HOME"] + "/myrc.txt";

	ofstream ot;
	ot.open(path);

	ot<<m["USER"]<<endl;
	ot<<m["USERID"]<<endl;
	ot<<m["HOME"]<<endl;
	ot<<m["PATH"]<<endl;
	ot<<m["PS1"]<<endl;

	ot.close();
}

void read_env()
{
	ifstream in;

	in.open("/etc/passwd");
	string line;

	int uid = getuid();

	ostringstream str1; 
  
    str1 << uid;
  
    string u = str1.str(); 
    
    std::vector<string> res;

	while(in)
	{
		getline(in ,line);

		if( strstr(line.c_str() ,u.c_str()))
		{
			res = split(line ,":");
			break;
		}
	}


	m.insert(make_pair(string("USER") ,res[0]));
	m.insert(make_pair(string("USERID") , res[2]));
	m.insert(make_pair(string("HOME") ,res[5]));

	in.close();

	in.open("/etc/environment");
	getline(in,line);

	line = line.substr(6,line.size()-7);

	cout<<line<<endl;

	m.insert(make_pair(string("PATH") ,line));

	char hn[1000];
	gethostname(hn,1000);

	m.insert(make_pair(string("PS1"),string("$")));
}

int find(string s)
{
	for(int i=0 ;i<s.size() ;i++)
	{
		if( s[i] == '=')
		{
			return 1;
		}
	}

	return 0;
}

void read_rc()
{
	ifstream in;
	char username[100];

	getlogin_r(username, 100); 
	
	string path = "/home/" + string(username) + "/myrc.txt";

	vector<string> v{"USER","USERID","HOME","PATH","PS1"};
	
	string line;

	in.open(path);

	int i=0;
	while(i!=5)
	{
		getline(in ,line);
		m.insert( make_pair(v[i] ,line));
		i++;
	}

	vector<string> s;

	while(1)
	{
		getline(in,line);
		
		if( find(line) == 0 )
		{
			break;
		}

		s = split(line ,"=");

		if( s.size() != 2)
		{
			break;
		}
		//cout<<s.size()<<endl;
		alias.insert(make_pair(s[0],s[1]));
	}

	while(in)
	{
		s = split(line," ");

		ext.insert(make_pair(s[0],s[1]));

		getline(in ,line);
	}

	in.close();

}

void make_rc()
{
	ifstream in;
	char username[100];

	getlogin_r(username, 100); 
	
	string path = "/home/" + string(username) + "/myrc.txt";

	in.open(path);

	if( in.good() == true )
	{
		read_rc();
	}
	else
	{
		read_env();
		write_rc();
	}
}

void execute_com(char **args ,char *buf)
{
	
	int pid = fork();

	if ( pid == 0 ) 
	{
		if( scr_flag == 1 )
		{
			ofstream out;
			out.open(script_file ,ofstream::app);

			out<<m["PS1"]<<" :"<<buf<<endl;	

			out.close();
		
			int fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

			if( fd < 0 )
			{
				write(STDOUT_FILENO ,"FAILED" ,6);
				//return;
			}

			write(STDOUT_FILENO ,"HELLO" ,5);
			dup2(fd ,STDOUT_FILENO);
		}	

		execvp( args[0], args );
	}

	waitpid(pid ,&e_code ,0);
	exit_code =  WEXITSTATUS(e_code);

	if( scr_flag == 1 )
	{
		ifstream in;
		in.open("temp.txt");

		ofstream out;
		out.open(script_file,ofstream::app);

		string line;

		while( in )
		{
			getline(in ,line);

			out<<line<<endl;
			
			cout<<line<<endl;
		}

		remove("temp.txt");
	}
}

void print_exit_status()
{
	cout<<exit_code<<endl;	
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
 
	make_command(para[0],args ,&para_count ," ");
	if( alias.find( args[0]) != alias.end() )
	{
		string temp = alias[ args[0]];
		strcpy(args[0] ,temp.c_str());
	}

	int pid = fork();

	if( pid == 0 )
	{
		int fd = open(para[1] ,O_CREAT | O_RDWR | O_APPEND ,S_IRWXU );

		if( fd < 0 )
		{
			cout<<"Cannot open file ";
			return;
		}

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

	make_command(para[0],args ,&para_count ," ");
	
	if( alias.find( args[0]) != alias.end() )
	{
		string temp = alias[ args[0]];
		strcpy(args[0] ,temp.c_str());
	}

	int pid = fork();

	if( pid == 0 )
	{
		int fd = open(para[1] ,O_CREAT | O_WRONLY ,S_IRWXU);

		if( fd < 0 )
		{
			cout<<"Cannot open file ";
			return;
		}
	
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

    	if( alias.find( args[0]) != alias.end() )
		{
			string temp = alias[ args[0]];
			strcpy(args[0] ,temp.c_str());
		}

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

}

int populate_trie()
{
	vector<string> v = split(m["PATH"] ,":");

  	for(int i=0 ; i<v.size() ;i++)
  	{
  		string path = v[i];

  		struct dirent *d;

  		DIR *dir = opendir(path.c_str());

  		if( dir == NULL )
  		{
  			cout<<"Error in opening :"<<endl;
  			return 0;
  		}

  		while( (d = readdir(dir)) != NULL )
  		{
  			//cout<<d->d_name<<" "<<endl;
  			root->insert(string(d->d_name));
  		}
  		
  	}
}

void replace_home(char *buf)
{
	string home = m["HOME"];
	int i=0;

	char res[1000];

	while( buf[i] != '~')
	{
		res[i] = buf[i];
		i++;
	}

	int temp = i+1;

	int j=0;

	while(j<home.size())
	{
		res[i++] = home[j++];
	}

	while(buf[temp] != '\0')
	{
		res[i++] = buf[temp++]; 
	}

	strcpy(buf ,res);
}

void change_ps1(string buf)
{
	vector<string> v = split(buf,"=");
	m["PS1"] = v[1];
}

void show_history()
{
	ifstream in;
	in.open("history.txt");

	string line;

	int count = 1;

	while(in)
	{
		getline(in ,line);

		auto c = std::to_string(count);
		c += ". ";
		write(STDOUT_FILENO ,c.c_str() ,c.size());		
		write(STDOUT_FILENO ,line.c_str() ,line.size());
		write(STDOUT_FILENO ,"\n" ,1);

		count++;
	}

	in.close();
}

void creat_alarm(int t)
{
    //alarms.push_back(t);

    //time_t seconds = time(NULL);

    //long long int x = static_cast<int>(seconds);

    //x =x +t;
    
    //alarms.push_back(x);

    int pid = fork();

    if( pid == 0 )
    {
    	
    	while(t>0)
    	{
    		sleep(1);
    		t--;
    	}

		cout<<"Alarm Notification"<<endl;
		exit(0);
    }

    /*std::vector<long long int>::iterator it;

    for(it=alarms.begin() ;it<=alarms.end() ;it++)
    {
    	if( *it == x )
    	{
    		alarms.erase(it);
    	}
    }*/

}

/*void invoke_alarms()
{
	ifstream in;
	in.open("alarms");

	while(in)
	{
		long long int x;
		in>>x;

		time_t seconds = time(NULL);
    
	    long long int cur = static_cast<int>(seconds);

	    if( cur > x )
	    {
	    	cout<<"Missed"<<endl;
	    }
	}
}*/

main()
{
	make_rc();

	populate_trie();

	//invoke_alarms();

	start_shell();
	char buf[1000] ;
	char *args[100] ;
	int argc =0;

	ofstream out;
	out.open("history.txt",ofstream::app);

	char u[100];

	getlogin_r(u,100);


	while(1)
	{

		string ps = m["PS1"] + string(u) + " :";

		struct termios initial_state = ena();
		sendinput(root ,buf,ps);
		dis(initial_state);

		out<<buf<<endl;

		if( strstr(buf ,"~"));
		{
			replace_home(buf);
		}

		if( strstr(buf ,"$?"))
		{
			
			print_exit_status();
		}
		else if( strstr(buf ,"PS1"))
		{
			
			change_ps1(string(buf));
		}
		else if( strstr(buf ,"$$") )
		{
			
			cout<<getpid()<<endl;
		}
		else if( strstr(buf ,"|"))
		{
			
			int pipes = count_pipes(buf);
	
			make_command(buf ,args ,&argc ,"|");
			piped(args ,argc ,pipes);
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
		else if( strstr(buf ,"alias"))
		{
			
			char *al[100];
			int c;
			make_command(buf ,args ,&argc ," ");

			make_command(args[1] ,al,&c,"=");

			alias.insert( make_pair(al[0] ,al[1]));

		}
		else if( strstr(buf ,"run"))
		{
			vector<string> v = split(string(buf) ," ");

			vector<string> s = split(v[1] ,".");

			string temp = ext[s[1]];
	

			char *comm[3];

			char b[100];

			strcpy(b ,temp.c_str());
			clean_str(b);
			comm[0] = b;

			char c[100];
			strcpy(c ,v[1].c_str());
			clean_str(c);

			comm[1] = c;

			comm[2] = NULL;

			execute_com(comm,buf);
		}
		else if( strstr(buf ,"history"))
		{
			show_history();
		}
		else if( strstr(buf ,"script"))
		{
			write(STDOUT_FILENO ,"Script Started\n",15);
			make_command(buf, args ,&argc ," ");

			script_file = args[1];
			scr_flag = 1;
		}
		else if( strstr(buf ,"end"))
		{
			scr_flag = 0;
			write(STDOUT_FILENO ,"Script ended\n",13);			
		}
		else if( strstr(buf ,"exit"))
		{/*
			ofstream out;
			out.open("alarms");

			for(int i=0 ;i<alarms.size() ;i++)
			{
				out<<alarms[i]<<endl;
			}*/

			return 0;
		}
		else if( strstr(buf ,"alarm"))
		{
			make_command(buf,args ,&argc," ");

    		char temp[100];
    		strcpy(temp ,args[1]);

    		int val = 0;

    		for(int i=0 ;i<strlen(temp) ;i++)
    		{
    			val = val*10 + ( temp[i] - '0');
    		}

			//cout<<val<<endl;

			creat_alarm(val);
			
		}
		else
		{
			
			make_command(buf ,args ,&argc ," ");

			if( alias.find( args[0]) != alias.end() )
			{
				string temp = alias[ args[0]];
				strcpy(args[0] ,temp.c_str());
			}


			if( strcmp(args[0],"cd") == 0 )
			{
				chdir(args[1]);
			}
			else if( strcmp(args[0] ,"echo") == 0 )
			{
				if( strcmp(args[1],"$PATH") == 0 )
				{
					cout<<m["PATH"];
				}

				execute_com(args ,buf);
			}
			else
			{
				execute_com(args ,buf);
			}
		}
	}
}