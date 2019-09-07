#include<iostream>
#include<bits/stdc++.h>
#include<cstring>
#include<dirent.h>

using namespace std;

class trie
{
	    public:

		unordered_map<char,trie*> m;
		int end;

		trie()
		{
			this->end = 0;
		}

		void insert(string k)
		{
			trie* trav = this;

			for(int i=0 ;i<k.length() ;i++)
			{
				
				if( trav->m.find(k[i]) == trav->m.end())  
				{
					trav->m[k[i]] = new trie(); 
				}

				trav = trav->m[k[i]];
			}


			trav->end = 1;
		}

		int search(string k)
		{
			trie *trav = this;

			for(int i=0 ;i<k.size() ;i++)
			{
				trav = trav->m[k[i]];

				if( trav == nullptr )
				{
					return 0;
				}
			}

			return 1;
		}

		void print_all(trie *trav ,string k)
		{
			if( trav->end == 1 )
			{
				cout<<k<<endl;
			}

			if( trav->m.size() == 0 )
			{
				return ;
			}

			unordered_map<char,trie*>::iterator it;

			for(it=trav->m.begin() ;it!=trav->m.end() ;it++)
			{
				//cout<<endl<<it->first<<endl;
				
	
				print_all( trav->m[it->first], k + it->first);
			}
		}

		int print_subtree(string k)
		{
			trie *trav = this;

			for(int i=0 ;i<k.size() ;i++)
			{
				if( trav == nullptr )
				{
					cout<<"Nothing here"<<endl;
					return 0;
				}

				trav = trav->m[k[i]];
			}

			int f1 = 0;

			if(trav->end == 1)
			{
				f1 = 1;
			}

			int f2 = 0;

			if( trav->m.size() == 0)
			{
				f2 = 1;
			}

			if( f1 == 1 && f2 == 1)
			{
				cout<<k<<endl;
				return 0;
			}

			if( f2 == 0 )
			{
				print_all(trav ,k);
				return 0;
			}

		}
};

/*main()
{
    trie *root = new trie(); 
  
  	vector<string> v = {"/usr/local/sbin" ,"/usr/local/bin" ,"/usr/sbin" ,"/usr/bin" ,"/sbin" ,"/bin" ,"/usr/games" ,"/usr/local/games"};

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
  			cout<<d->d_name<<" "<<endl;
  			root->insert(string(d->d_name));
  		}
  		
  	}

    //trie* root = new trie();

    root->insert("hello"); 
    root->insert("dog"); 
    root->insert("hell"); 
    root->insert("cat"); 
    root->insert("a"); 
    root->insert("hel"); 
    root->insert("help"); 
    root->insert("helps"); 
    root->insert("helping"); 
    
    cout<<root->search("hell");
    root->print_subtree("he"); 

  	
    return 0; 
}*/