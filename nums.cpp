#include "seq.hh"
#include "arrayseq.hh"
#include "dlinkseq.hh"

#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cctype>
using namespace std;

void printErrExit(int)
{
	cout << "usage: ./nums [-a] INTS_FILE_PATH" << endl;
	exit(EXIT_FAILURE);
}
void printErrExit(char *msg)
{
	cout << "cannot read " << msg << ": No such file or directory." << endl;
	exit(EXIT_FAILURE);
}

//similar to overloaded << function in Seq<E> class;
template<typename E>
void printFirstLast(SeqPtr<E>& Ptr)
{
	ConstIterPtr<E> begin = Ptr->cbegin();
	ConstIterPtr<E> end = Ptr->cend();
	for(ConstIter<E> &b = *begin, &e = *end; (b || e); ++b, --e)
		cout << *b << endl << *e << endl;
}

void setup(int argc, char **argv)
{
	if(argc < 2 || argc > 3)
		printErrExit(EXIT_FAILURE);	
	else if(argc == 3 && string(argv[1]).compare("-a") != 0)
		printErrExit(EXIT_FAILURE);

	ifstream file;
	if(argc == 2)
		file.open(argv[1]);
	else if(argc == 3)
		file.open(argv[2]);
	else
		printErrExit(EXIT_FAILURE);

	if(!file.is_open())
	{
		if(argc == 2)
			printErrExit(argv[1]);
		else
			printErrExit(argv[2]);
	}

	SeqPtr<string> list;
	//creation of doubly linked list;
	if(argc == 2)
		list = make_unique<DLinkSeq<string>>();
	//creation of fixed array list;
	else if(argc == 3)
		list = ArraySeq<string>::make();

	for(; !file.eof(); )
	{
		string num;
		file >> num;
		for(unsigned int i = 0; i < num.length(); i++)
		{
			//make sure the input is a number;
			if(isdigit(num[i]))
				list->push(num);
			else
				break;
		}
	}

	printFirstLast(list);
	list->clear();

	file.close();
}

int main(int argc, char **argv)
{
	setup(argc, argv);
	return EXIT_SUCCESS;

// testing purposes;
//	Seq<int>* d = new DLinkSeq<int>;
/*	DLinkSeq<int>* d = new DLinkSeq<int>;
	d->push(100);
	d->push(103);
	d->push(999);
	d->print(); cout << endl;

	d->clear();
	d->print(); cout << endl;
*/
/* testing ADT iterators work;
	ConstIterPtr<int> ptr = d->cbegin();
	cout << "cbegin(): " << **d->cbegin() << "|cend(): " << **d->cend() << endl;
	cout << "!    " << *(*ptr) << endl;
	++(*ptr);
	cout << "!    " << *(*ptr) << endl;
	++(*ptr);
	cout << "!    " << *(*ptr) << endl;
	++(*ptr);
	cout << "!    " << *(*ptr) << endl;
*/
}
