#include "stdafx.h"
#include "BTree.h"
#include <iostream>
#include <string>
#include <vector>
#include <strstream>
using namespace std;

#if 1
// for the high-resolution timer
#include <windows.h>

typedef BTree<string,string> StringMap;
typedef StringMap::Node Node;
typedef StringMap::Element Element;

int testBtree(int argc, _TCHAR* argv[])
{
   // the main function is just some code to test the b-tree.  it inserts 100,000 elements,
   // then searches for each of them, then deletes them in reverse order (also tested in
   // forward order) and searches for all 100,000 elements after each deletion to ensure that
   // all remaining elements remain accessible.

   __int64 frequency, start, end, total;
   QueryPerformanceFrequency( (LARGE_INTEGER *)&frequency );


   StringMap tracker;  // maintains a pointer to the current root of the b-tree
   vector<string> search_vect;
   // prepare the key strings
   search_vect.resize (100000);
   int search_count = 0;
   for (int i=0; i<100000; i++) {
      strstream stm;
      stm << i;
      stm >> search_vect[search_count++];
   }
   //string s;
   //cout << "finished preparing key strings\n";
   QueryPerformanceCounter ( (LARGE_INTEGER *)&start);
   for (int i=0; i<100000; i++) {
      tracker.insert(search_vect[i], search_vect[i]+" hi you");
   }
   //cout << "finished inserting elements\n";
   for (int i=0; i<100000; i++) {
      Element& result = tracker.search(search_vect[i]);
   }
   //cout << "finished searching for elements\n";
   for (int i=99999; i>=0; i--) {
      tracker.remove(search_vect[i]);
   }

   QueryPerformanceCounter ( (LARGE_INTEGER *)&end);
   total = (end-start)/(frequency/1000);
   //cout << "total millisec for 100000 elements: " << (int)total << endl;

   //cout << "after deletion" << endl;
   //getchar();
   return 0;
}

#endif