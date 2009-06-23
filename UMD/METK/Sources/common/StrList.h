# ifndef __STRLIST_HEADER__
# define __STRLIST_HEADER__


#pragma warning(disable:4251)
#ifdef WIN32
#ifndef common_EXPORTS
#define __COMMON_IMPORTEXPORT __declspec(dllimport)
#else
#define __COMMON_IMPORTEXPORT __declspec(dllexport) 
#endif
#else
#define __COMMON_IMPORTEXPORT
#endif

# include <string>
# include <list>

using namespace std;

class __COMMON_IMPORTEXPORT StrList
{

public:

	StrList()
	{
	}


	~StrList()
	{		
	}



	void add(string value)
	{
		stringList.push_back(value);		
	}

	void remove(string value)
	{
		list<string>::iterator iter;
		for (iter = stringList.begin(); iter!=stringList.end(); iter++)
		{
			if ((string)(*iter) == value)
			{
				stringList.erase(iter);
				return;
			}
		}
	}

	int getSize()
	{
		return stringList.size();
	}


private:
	list<string> stringList;

};

#endif

