#include<iostream>
#include<fstream>

using namespace std;
const int RR = 9;

int main(int argc, char* argv[])
{
  const int MAX_LINE_LENGTH = 1000;
  const long MAX_RANKS = 100000;
  ifstream fin;
//  ifstream fin(argv[1]);
  fin.open(argv[1]);
  char* str = new char[MAX_LINE_LENGTH];
  fin.getline(str,MAX_LINE_LENGTH);

  int numRanks = 0;
  int ranks[MAX_RANKS][RR];
  while(fin)
  {
    fin.getline(str,MAX_LINE_LENGTH);
    char* left = strpbrk(str,"[");
    char * s = strtok(left,",][:");
    int ind=0;
    int tmpRank[RR];
    
    while(s != NULL)
    {
      tmpRank[ind] = atoi(s);
      ind++;
      if(ind > RR)
      {
        cout << "ERROR: ind got too big" << endl;
        exit(1);
      }
      s = strtok(NULL,",][:");
    }
    //check if tmpRank is in ranks
    bool found = false;
    if(tmpRank[RR-3]-tmpRank[RR-4] > 4)
      found = true;
    for(int i=0; i<numRanks && !found; i++)
    {
      bool flag = false;
      for(int j=0; j<RR && !flag; j++)
      {
        if(ranks[i][j] != tmpRank[j])
          flag = true;
      }
      if(!flag)
      {
        //found an existing rank sequnce
        found = true;
      }
    }
    if(!found)
    {
      for(int i=0; i<RR; i++)
        cout << tmpRank[i] << " ";
      cout << endl;
      //found a new rank sequence
      for(int j=0; j<RR; j++)
        ranks[numRanks][j] = tmpRank[j];
      numRanks++;
      if(numRanks == MAX_RANKS)
      {
        cout << "ERROR: too many distinct rank sequences" << endl;
        exit(1);
      }
    }
  }

  int maxDelta[RR-1];
  int minDelta[RR-1];
  for(int i=0; i<RR-1; i++)
  {
    minDelta[i] = ranks[0][i+1]-ranks[0][i];
    maxDelta[i] = ranks[0][i+1]-ranks[0][i];
    for(int j=1; j<numRanks; j++)
    {
      if(ranks[j][i+1] - ranks[j][i] > maxDelta[i])
        maxDelta[i] = ranks[j][i+1]-ranks[j][i];
      if(ranks[j][i+1] - ranks[j][i] < minDelta[i])
        minDelta[i] = ranks[j][i+1]-ranks[j][i];
    }    
  }
  for(int i=0; i<RR-1; i++)
    cout << "maxDelta[" << i << "] is: " << maxDelta[i] << endl;
  for(int i=0; i<RR-1; i++)
    cout << "minDelta[" << i << "] is: " << minDelta[i] << endl;

  return 0;
}
