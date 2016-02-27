#include<iostream>
#include<fstream>
#include<cstdlib>
#include<cstring>

using namespace std;

const int numRanks =10;
const int maxRank = 70;
const int MAX_NUM_COMPLETE_GRAPHS = 500;
const int MAX_NUM_COVERS = 400000;
const int MAX_LINE_LENGTH = 3000;
long extendCalls = 0;

int goalRankSize[12] = {1, 1, 2, 3, 5, 7, 11, 16, 25, 37, 57,96};

int numKn = 0;
int vertex[numRanks][maxRank];
int rankSize[numRanks];
int edgeSize[numRanks-1];
bool covers[numRanks-1][maxRank][maxRank];
int numPosets = 0;
//bool tagged[maxRank];
//bool skipped[maxRank];
int deg[maxRank];
bool adj[maxRank][maxRank];
bool adjKn[MAX_NUM_COMPLETE_GRAPHS][MAX_NUM_COMPLETE_GRAPHS];
bool indCovers[MAX_NUM_COVERS][MAX_NUM_COMPLETE_GRAPHS];
int indCoverCount[numRanks];

void buildAdjacencyGraph(int r)
{
  //builds adjacency graph of nodes at rank r
  for(int i=0; i<maxRank; i++)
  for(int j=0; j<maxRank; j++)
    adj[i][j] = false;
  for(int i=0; i<maxRank; i++)
  for(int j=i+1; j<maxRank; j++)
  {
    //check if i and j are adjacent
    for(int k=0; k<maxRank && !adj[i][j]; k++)
      if(covers[r][k][i] && covers[r][k][j])
        adj[i][j] = true;
  }
/*  for(int i=0; i<maxRank; i++)
  for(int j=i+1; j<maxRank; j++)
    if(adj[i][j])
      cout << i << " and " << j << " are adjacent" << endl;
  cout << "finished finding adjacencies" << endl;*/
  return;
};

struct Node
{
  int* verts;
  int numVerts;
  Node* next;
};

void getAllCompleteGraphs2(int[], Node*, int);
void outputNode(Node*);

Node* getAllCompleteGraphs()
{
  //uses adj[][] (must buildAdjacencyGraph() before call to getAllComleteGraphs())
  //returns an array of pointers to all of the complete graphs of adj[][]
  numKn = 0;
  Node* r = NULL;
  for(int i=0; i<maxRank; i++)
  {
    Node* kG = new Node;
    kG->numVerts = 0;
    kG->verts = NULL;
    kG->next = NULL;
    //find all complete graphs with i
    int includedVerts[maxRank];
    for(int j=0; j<maxRank; j++)
      includedVerts[j] = -1;
    includedVerts[0] = i;
    getAllCompleteGraphs2(includedVerts,kG,1);
    //add kG to r
//    outputNode(kG);
//    cout << "finished kG" << endl;
    if(kG->next != NULL)
    {
      if(r == NULL)
        r = kG->next;
      else
      {
        Node* lastr = r;
        while(lastr->next != NULL)
          lastr = lastr->next;
        lastr->next = kG->next;
      }
    }
    delete kG;
  }
//  cout << "returning r" << endl;
  //reorder r by size
  bool foundSwap = true;
  while(foundSwap)
  {
    foundSwap = false;
    Node* ptr = r;
    Node* last = NULL;
    Node* ptr2 = r->next;
    while(ptr2 != NULL)
    {
      if(ptr->numVerts < ptr2->numVerts)
      {
        //swap *ptr and *ptr2
        foundSwap = true;
        ptr->next = ptr2->next;
        ptr2->next = ptr;
        if(last == NULL)
          r = ptr2;
        else
          last->next = ptr2;
        ptr = ptr2;
        ptr2 = ptr2->next;
      }
      last = ptr;
      ptr = ptr2;
      ptr2 = ptr->next;
    }
  }
        

  return r;
};

void getAllCompleteGraphs2(int iV[], Node* kG, int num)
{
//cout << "kG is: " << endl;
//outputNode(kG);
  //adds all complete graphs including the vertices in iV to the list kG
  for(int i=iV[num-1]; i<maxRank; i++)
  {
    bool flag = true;
    for(int j=0; j<num && flag; j++)
      if(!adj[iV[j]][i])
        flag = false;
    if(flag)
    {
      iV[num] = i;
      numKn++;
      if(numKn > MAX_NUM_COMPLETE_GRAPHS)
      {
        cout << "ERROR: MAX_NUMBER_COMPLETE_GRAPHS is too small" << endl;
        exit(1);
      }
/*cout << "found complete graph: ";
for(int k=0; k<num+1; k++)
cout << iV[k] << " ";
cout << endl;*/
      Node* tmpNode = new Node;
      tmpNode->verts = new int[num+1];
      tmpNode->numVerts = num+1;
      for(int j=0; j<=num; j++)
        tmpNode->verts[j] = iV[j];
      tmpNode->next = NULL;
      Node* t = kG;
      while(t->next != NULL)
        t = t->next;
      t->next = tmpNode;
      getAllCompleteGraphs2(iV, kG, num+1);
    }
  }
  return;
};

bool checkAdj(Node* n1, Node* n2)
{
  int numSame = 0;
  for(int i=0; i<n1->numVerts && numSame<2; i++)
  for(int j=0; j<n2->numVerts && numSame<2; j++)
  {
    if(n1->verts[i] == n2->verts[j])
      numSame++;
  }
  if(numSame >= 2)
    return true;
  return false;  
};

void buildKnGraph(Node* n)
{
  for(int i=0; i<numKn; i++)
  for(int j=0; j<numKn; j++)
    adjKn[i][j] = false;
  Node* nPtr = n;
  int i = 0;
  while(nPtr != NULL)
  {
    int j = i+1;
    Node* nPtr2 = nPtr->next;
    while(nPtr2 != NULL)
    {
      if(checkAdj(nPtr,nPtr2))
      {
        adjKn[i][j] = true;
        adjKn[j][i] = true;
      }
      nPtr2 = nPtr2->next;
      j++;
    }
    nPtr = nPtr->next;
    i++;
  }
/*cout << "complete graph adjacencies: " << endl;
for(int i=0; i<numKn; i++)
{
for(int j=0; j<numKn; j++)
if(adjKn[i][j])
cout << "1 ";
else
cout << "0 ";
cout << endl;
}*/
  return;
};

void fIC(int* coverSet, int sizeCoverSet, int nextCover, int avail[],int r)
{
  if(nextCover == sizeCoverSet)
  {
//cout << "Checking for independent cover: " << endl;
/*for(int i=0; i<numKn; i++)
if(avail[i]==1)
cout << i << " ";
cout << endl;*/
    //check if we have an independent cover
    for(int i=0; i<numKn; i++)
    for(int j=0; j<numKn; j++)
      if(avail[i] == 1 && avail[j] == 1)
        if(adjKn[i][j])
          return;
    for(int i=0; i<sizeCoverSet; i++)
    {
      int numCoveredBy = 0;
      for(int j=0; j<numKn; j++)
      {
        if(adjKn[coverSet[i]][j] || coverSet[i]==j)
          if(avail[j] == 1)
            numCoveredBy++;
        if(numCoveredBy > 1)
          return;
      }
      if(numCoveredBy != 1)
        return;
    }

    //now we have an independent cover
    for(int i=0; i<numKn; i++)
      if(avail[i] == 1)
        indCovers[indCoverCount[r]][i] = true;
      else
        indCovers[indCoverCount[r]][i] = false;
    indCoverCount[r]++;
    if(indCoverCount[r] > MAX_NUM_COVERS)
    {
      cout << "ERROR: MAX_NUM_COVERS is too small" << endl;
      exit(1);
    }
    return;
  }

//cout << "next cover is: " << nextCover << endl;
//cout << "coverSet is: ";
//for(int i=0; i<sizeCoverSet
  if(avail[coverSet[nextCover]] == -1)
  {
    fIC(coverSet,sizeCoverSet, nextCover+1, avail,r);
    return;
  }

  for(int i=0; i<numKn; i++)
  {
    if((adjKn[i][coverSet[nextCover]] && avail[i]==0) || i==coverSet[nextCover])
    {
      int oldAvail[numKn];
      for(int j=0; j<numKn; j++)
        oldAvail[j] = avail[j];
      avail[i] = 1;
      for(int j=0; j<numKn; j++)
        if(adjKn[i][j])
          avail[j] = -1;
      fIC(coverSet, sizeCoverSet, nextCover+1, avail,r);
      for(int j=0; j<numKn; j++)
        avail[j] = oldAvail[j];
    }
  }
  return;
};

void findIndependentCovers(int* coverSet, int sizeCoverSet,int r)
{
  int avail[numKn];
  for(int i=0; i<numKn; i++)
    avail[i] = 0;
  for(int i=0; i<MAX_NUM_COVERS; i++)
  for(int j=0; j<numKn; j++)
    indCovers[i][j] = false;
  indCoverCount[r] = 0;
  fIC(coverSet, sizeCoverSet, 0, avail,r);
  return;
};

void findVertexCovers(Node* n, int r)
{
  //takes a NodeList n and returns an array of all sets of complete
  //  graphs that "cover the K_2's"
  
  //build a list of just the K_2's
  Node* ptr = n;
  int k2Count = 0;
  while(ptr!=NULL)
  {
    if(ptr->numVerts == 2)
      k2Count++;
    ptr = ptr->next;
  }
  int* k2s = new int[k2Count];
  ptr = n;
  int ind = 0;
  k2Count = 0;
  while(ptr!=NULL)
  {
    if(ptr->numVerts == 2)
    {
      k2s[k2Count] = ind;
      k2Count++;
    }
    ptr = ptr->next;
    ind++;
  }  
  buildKnGraph(n);
  findIndependentCovers(k2s, k2Count,r);
  delete[] k2s;
/*  cout << "independent covers: " << endl;
  for(int i=0; i<indCoverCount[r]; i++)
  {
    for(int j=0; j<numKn; j++)
      if(indCovers[i][j])
        cout << j << " ";
    cout << endl;
  }*/
  return;
};

void outputPoset(int);
bool isUpDown(int r);

void deleteNodeList(Node* n)
{
  if(n == NULL)
    return;
  Node* tmp = n->next;
  while(tmp != NULL)
  {
    delete[] n->verts;
    delete n;
    n = tmp;
    tmp = tmp->next;
  }
  if(n != NULL)
  {
    delete[] n->verts;
    delete n;
  }
  return;
};

void outputPoset2(int);

void extendCovers2(int r)
{
//cout << "starting call to extendCovers2" << endl;
//cout << "covers is: " << endl;
//outputPoset2(r-1);
  if(r == numRanks-1)
  {
//cout << "attempting to output a poset" << endl;
    outputPoset(r-1);
    return;
  }
//cout << "covers is: " << endl;
//outputPoset2(r);
  for(int i=0; i<MAX_NUM_COVERS; i++)
  for(int j=0; j<MAX_NUM_COMPLETE_GRAPHS; j++)
    indCovers[i][j] = false;
  buildAdjacencyGraph(r-1);
  Node* n = getAllCompleteGraphs();
//cout << "numKn is: " << numKn << endl;
//  outputNode(n);
  findVertexCovers(n,r);
  int locNumKn = numKn;
  int locIndCoverCount = indCoverCount[r];
  int** locIndCovers = new int* [indCoverCount[r]];
  for(int i=0; i<indCoverCount[r]; i++)
    locIndCovers[i] = new int [numKn];
//  int locIndCovers[indCoverCount[r]][numKn];
  for(int i=0; i<indCoverCount[r]; i++)
  for(int j=0; j<locNumKn; j++)
    locIndCovers[i][j] = indCovers[i][j];
  
  //now update covers
//cout << "indCoverCount is: " << indCoverCount[r] << endl;
  for(int i=0; i<indCoverCount[r]; i++)
  {
//cout << "checking indCover:" << endl;
    bool flag = true;
    for(int j=0; j<maxRank; j++)
    for(int k=0; k<maxRank; k++)
      covers[r][j][k] = false;
    Node* ptr = n;
    int count = 0;
    for(int j=0; j<locNumKn && ptr!=NULL; j++)
    {
      if(locIndCovers[i][j])
      {
        for(int k=0; k<ptr->numVerts; k++)
          covers[r][ptr->verts[k]][count] = true;
        count++;
      }
      ptr = ptr->next;
    }
    for(int i=0; i<rankSize[r]; i++)
    {
      int numUpTo = 0;
      int numUpFrom = 0;
      for(int j=0; j<maxRank; j++)
        if(covers[r][i][j])
          numUpFrom++;
      for(int j=0; j<maxRank; j++)
        if(covers[r-1][j][i])
          numUpTo++;
      if(numUpFrom == numUpTo)
      {
//cout << "adding2 cover from " << count << " to " << i << endl;
        covers[r][i][count] = true;
        count++;
        numUpFrom++;
      }
      if(numUpFrom != numUpTo+1)
      {
//cout << "failed extension, covers is: " << endl;
//outputPoset2(r);
//cout << "rank is: " << r << endl;
        flag = false;
//        return;
      }
    }
    if(flag)
    {
      rankSize[r+1]=count;
//  cout << "rankSize[r+1] is: " << rankSize[r+1] << endl;
      if(isUpDown(r) /*&& count == goalRankSize[r+1]*/)
        extendCovers2(r+1);
    }
  }
  for(int i=0; i<locIndCoverCount; i++)
    delete[] locIndCovers[i];
  delete[] locIndCovers;   
  deleteNodeList(n);  
  return;
};

void outputNode(Node* n)
{
  while(n != NULL)
  {
    for(int i=0; i<n->numVerts; i++)
      cout << n->verts[i] << " ";
    cout << endl;
    n = n->next;
  }
  return;
};

bool isUpDown(int r)
{
  //checks if covers[r][][] satisfies up down stuff for vertices in rank r
  for(int i=0; i<maxRank; i++)
  {
    int DU[maxRank];
    int UD[maxRank];
    for(int j=0; j<maxRank; j++)
    {
      DU[j] = 0;
      UD[j] = 0;
    }
    //fill in DU numbers
    for(int j=0; j<maxRank; j++)
    {
      if(covers[r-1][j][i])
      {
        // so i covers j
        for(int k=0; k<maxRank; k++)
        {
          if(covers[r-1][j][k])
            DU[k] = DU[k]+1;
        }
      }
    }
    //fill in the UD numbers
    for(int j=0; j<maxRank; j++)
    {
      if(covers[r][i][j])
      {
        // so j covers i
        for(int k=0; k<maxRank; k++)
        {
          if(covers[r][k][j])
            UD[k] = UD[k]+1;
        }
      }
    }
/*cout << "UD is: ";
for(int j=0; j<maxRank; j++)
cout << UD[j] << " ";
cout << endl;
cout << "DU is: ";
for(int j=0; j<maxRank; j++)
cout << DU[j] << " ";
cout << endl;*/
    for(int j=0; j<maxRank; j++)
    {
      if(j!=i)
        if(UD[j] != DU[j])
          return false;
    }
  }

  //check to see if reflection makes smaller
  int map[numRanks][maxRank];
  for(int i=0; i<numRanks; i++)
  for(int j=0; j<maxRank; j++)
    map[i][j] = -1;
  map[0][0] = 0;
  map[1][0] = 1;
  map[1][1] = 0;
  for(int i=2; i<r; i++)
  {
    //create map[i][]
    for(int j=0; j<maxRank; j++)
    {
      bool flag3 = false;
      for(int k=0; k<maxRank && !flag3; k++)
        if(covers[i][k][j])
          flag3 = true;
      if(flag3)
      {
      bool c[maxRank];
      for(int k=0; k<maxRank; k++)
        c[k] = false;
      for(int k=0; k<maxRank; k++)
        if(covers[i][k][j])
          c[map[i-1][k]]= true;
      bool flag2 = true;
      for(int t=0; t<maxRank && flag2; t++)
      {
        bool flag = true;
        for(int k=0; k<maxRank && flag; k++)
        {
          if(c[k] != covers[i][k][t])
            flag = false;
        }
        if(flag)
        {
          map[i][j] = t;
          flag2 = false;
        }
      }
//      if(flag2)
//        cout << "AAAAAAAAAAAAAAAAAAA" << endl;
      }
    }
  }
/*cout << "covers is: " << endl;
outputPoset2(r);

cout << "map is: " << endl;
for(int i=0; i<r; i++)
{
for(int j=0; j<maxRank; j++)
cout << map[i][j] << " ";
cout << endl;
}*/

  int d[maxRank];
  int d2[maxRank];
  for(int i=0; i<maxRank; i++)
    d[i] = 0;
  for(int i=0; i<maxRank; i++)
    d2[i] = 0;
  bool c[maxRank][maxRank];
  for(int i=0; i<maxRank; i++)
  {
    for(int j=0; j<maxRank; j++)
     c[i][j] = false;
    for(int j=0; j<maxRank; j++)
    {
      if(covers[r][j][i])
        d2[i] = d2[i]+1;
      if(covers[r][map[r-1][j]][i])
      {
        c[i][j] = true;
        d[i] = d[i]+1;
      }
    }
  }
/*  for(int i=0; i<maxRank; i++)
    if(d[i] != d2[i])
    {
      cout << i << "AAAAAAAAAA" << endl;
      exit(1);
    }*/
  bool flag = true;
  while(flag)
  {
    flag = false;
    for(int i=1; i<maxRank; i++)
    {
      if(d[i] == d[i-1])
      {
        bool flag2 = true;
        for(int j=0; j<maxRank && flag2; j++)
        {
          if(c[i][j] && !c[i-1][j])
          {
            flag = true;
            flag2 = false;
            for(int k=0; k<maxRank; k++)
            {
              bool tmp = c[i][k];
              c[i][k] = c[i-1][k];
              c[i-1][k] = tmp;
            }
          }
          if(!c[i][j] && c[i-1][j])
            flag2 = false;  
        }
      }
    }
  }
  for(int i=0; i<maxRank; i++)
  {
    for(int j=0; j<maxRank; j++)
    {
      if(c[i][j] && !covers[r][j][i])
{
//cout << "BBB" << endl;
        return false;
//outputPoset2(r);
//return true;
}
      if(!c[i][j] && covers[r][j][i])
        return true;
    }
  }
      

  //now check for canonical
/*  int d[maxRank];
  for(int i=0; i<maxRank; i++)
    d[i] = 0;
  for(int i=0; i<maxRank; i++)
  {
    for(int j=0; j<maxRank; j++)
      if(covers[r][j][i])
        d[i] = d[i] + 1;
  }
cout << "d[] is: ";
for(int i=0; i<maxRank; i++)
cout << d[i] << " ";
cout << endl;
  for(int i=1; i<maxRank; i++)
    if(d[i] > d[i-1])
      return false;
//cout << "got here" << endl;
  for(int i=1; i<maxRank; i++)
  {
    if(d[i] == d[i-1])
    {
      bool flag = true;
      for(int j=0; j<maxRank && flag; j++)
      {
        if(covers[r][j][i] && !covers[r][j][i-1])
          return false;
        if(!covers[r][j][i] && covers[r][j][i-1])
          flag = false;
      }
    }
  }       */
  return true;
};

void outputPoset(int r)
{
  if(!isUpDown(r))
  {
//cout << "AAAAAAAA" << endl;
    return;
  }
  else
    numPosets++;
  int map[numRanks][maxRank];
  for(int i=0; i<numRanks; i++)
  for(int j=0; j<maxRank; j++)
    map[i][j] = j;
  for(int i=1; i<=r+1; i++)
  {
    //try and change the ordering of the vertices at rank i to 
    //reduce the number of crossings
    bool reduced = true;
    while(reduced)
    {
      reduced = false;
      for(int j=0; j<maxRank; j++)
      for(int k=j+1; k<maxRank; k++)
      {
        int numCross=0;
        for(int s=0; s<maxRank; s++)
        for(int t=s+1; t<maxRank; t++)
        {
          if(covers[i-1][map[i-1][s]][map[i][k]] && covers[i-1][map[i-1][t]][map[i][j]])
            numCross++;
        }
        
        //swap j and k
        int tmp = map[i][j];
        map[i][j] = map[i][k];
        map[i][k] = tmp;
        int numCross2 = 0;
        for(int s=0; s<maxRank; s++)
        for(int t=s+1; t<maxRank; t++)
        {
          if(covers[i-1][map[i-1][s]][map[i][k]] && covers[i-1][map[i-1][t]][map[i][j]])
            numCross2++;
        }
        if(numCross2 < numCross)
          reduced = true;
        else
        {
          //swap j and k back
          tmp = map[i][j];
          map[i][j] = map[i][k];
          map[i][k] = tmp;
        }
      }
    }
  }

  cout << "P" << numPosets << ":={";
  bool first = true;
  int rCount[numRanks];
  for(int i=0; i<numRanks; i++)
    rCount[i] = 0;
  for(int i=0; i<=r; i++)
  {
    for(int j=0; j<maxRank; j++)
    {
      bool flag = false;
    for(int k=0; k<maxRank; k++)
    {
      if(covers[i][map[i][k]][map[i+1][j]])
      {
        if(!first)
          cout << ",";
        else
          first = false;
        cout << "[" << (char)(65+i) << k << "," << (char)(66+i) << j << "]";
        flag = true;
      }
    }
      if(flag)
        rCount[i]=rCount[i]+1;
    }
  }
  cout << "}:" << endl;
  cout << "r" << numPosets << ":=[";
  first = true;
  for(int i=0; i<=r; i++)
  {
    if(!first)
      cout << ",";
    else
      first = false;
    cout << rCount[i];
  }
  cout << "]:" << endl;
/*    for(int j=0; j<=r; j++)
    {
      bool first = true;
      cout << "M" << numPosets << "_" << j << ":=matrix([";
      for(int s=0; s<maxRank; s++)
      {
        if(!first)
          cout << ",";
        else
          first = false;
        bool first2 = true;
        if(!first2)
          cout << ",";
        else
          first2 = false;
        cout << "[";
        for(int t=0; t<maxRank; t++)
        {
          if(covers[j][s][t])
            cout << 1;
          else
            cout << 0;
          if(t!=maxRank-1)
            cout << ",";
        }
        cout << "]";
      }
      cout << "]):" << endl;
    }
  
    for(int i=1; i<=r; i++)
    {
      cout << "evalm(M" << numPosets << "_" << i << " &* transpose(M" << numPosets << "_" << i << ")-transpose(M" 
           << numPosets << "_" << i-1 << ") &* M" << numPosets << "_" << i-1 << ");" << endl;
    }*/
/*  cout << "---------------------------------------------------------" << endl;
    //output the poset determined by covers up to rank r;
  for(int i=0; i<=r; i++)
  {
    cout << "Rank " << i+1 << "'s covers:" << endl;
    for(int j=0; j<maxRank; j++)
    for(int k=0; k<maxRank; k++)
    {
      if(covers[i][k][j])
        cout << j << " covers " << k << endl;
    }
  }
  cout << "---------------------------------------------------------" << endl;*/
  return;
}; 

void outputPoset2(int r)
{
//  if(!isUpDown(r))
//  {
//cout << "AAAAAAAA" << endl;
//    return;
//  }
//  else
//    numPosets++;
  cout << "---------------------------------------------------------" << endl;
    //output the poset determined by covers up to rank r;
  for(int i=0; i<=r; i++)
  {
    cout << "Rank " << i+1 << "'s covers:" << endl;
    for(int j=0; j<maxRank; j++)
    for(int k=0; k<maxRank; k++)
    {
      if(covers[i][k][j])
        cout << j << " covers " << k << endl;
    }
  }
  cout << "---------------------------------------------------------" << endl;

//  buildAdjacencyGraph(r);
// Node* n = getAllCompleteGraphs();
//  outputNode(n);
//  findVertexCovers(n);
//  deleteNodeList(n);  
  return;
};

void extendCovers(int rank, int numEdges, int nextEdgeA, int nextEdgeB)
{
extendCalls++;
//cout << "numPosets is: " << numPosets << endl;
//cout << "starting extend cover: " << rank << " " << numEdges << " " << nextEdgeA << " " << nextEdgeB << endl;
cout << "covers is: " << endl;
outputPoset2(rank);
//cout << "deg is: ";
//for(int i=0; i<maxRank; i++)
//cout << deg[i] << " ";
//cout << endl;
  if(rank > numRanks-2 || nextEdgeA >= maxRank || nextEdgeB >= maxRank || numEdges > 
rankSize[rank]+edgeSize[rank-1])
{
//cout << "CC" << endl;
    return;
}
  int edgesLeft = rankSize[rank]+edgeSize[rank-1]-numEdges;
  int rightMax[maxRank];
  rightMax[maxRank-1] = deg[maxRank-1];
  for(int i=maxRank-2; i>=0; i--)
  {
    if(deg[i] > rightMax[i+1])
      rightMax[i] = deg[i];
    else
      rightMax[i] = rightMax[i+1];
  }
  int edgesNeeded = 0;
  for(int i=0; i<maxRank; i++)
    edgesNeeded = edgesNeeded + rightMax[i]-deg[i];
cout << "edgesNeed is: " << edgesNeeded << endl;
cout << "edgesLeft is: " << edgesLeft << endl;
/*cout << "deg is: ";
for(int i=0; i<maxRank; i++)
cout << deg[i] << " ";
cout << endl;
cout << "------" << endl;*/
  if(edgesNeeded > edgesLeft)
    return;  
  if(nextEdgeB == 0 && nextEdgeA != 0)
  {
    //check if vertex of nextEdgeA-1 is empty
    bool flag = true;
    for(int i=0; i<maxRank && flag; i++)
    {
      if(covers[rank][nextEdgeA-1][i])
        flag = false;
    }
    if(flag)
{
//cout << "DD" << endl;
      return;
}
  }

  if(numEdges == rankSize[rank] + edgeSize[rank-1])
  {
    if(rank == numRanks-2)
      outputPoset(rank);
    else
    {
      int numVert = 0;
      for(int i=0; i<maxRank; i++)
      {
        bool flag = true;
        for(int j=0; j<maxRank && flag; j++)
        {
          if(covers[rank][j][i])
          {
            flag = false;
            numVert++;
          }
        }
      }
      int numVertB = 0;
      bool flag2 = true;
      for(int i=0; i<maxRank && flag2; i++)
      {
        bool flag = true;
        for(int j=0; j<maxRank && flag; j++)
        {
          if(covers[rank][j][i])
          {
            flag = false;
            numVertB++;
          }
        }
        if(flag)
          flag2 = false;
      }
      if(numVertB == numVert)
      {
      if(isUpDown(rank))
      {
      for(int i=0; i<maxRank; i++)
      for(int j=0; j<maxRank; j++)
        covers[rank+1][i][j] = false;
      edgeSize[rank] = numEdges;

      rankSize[rank+1] = numVert;
//outputPoset(rank);
//cout << "numVert is: " << numVert << endl;
/*      for(int i=0; i<maxRank; i++)
      {
        tagged[i] = false;
        skipped[i] = false;
      }*/
      for(int i=0; i<maxRank; i++)
        deg[i] = 0;
      extendCovers(rank+1,0,0,0);
      }
      }
    }
//cout << "EE" << endl;
    return;
  }

  bool flag2 = false;
  if(nextEdgeB == 0 && nextEdgeA != 0)
  {
    flag2 = true;
    nextEdgeA--;
  }
  //find out how many edges go up to nextEdgeA
  int numUpTo = 0;
  for(int i=0; i<maxRank; i++)
  {
    if(covers[rank-1][i][nextEdgeA])
      numUpTo++;
  }

  //count how many edges already go up from nextEdgeA
  int numUpFrom = 0;
  int t = nextEdgeB;
  if(flag2)
    t = maxRank;
  for(int i=0; i<t; i++)
  {
    if(covers[rank][nextEdgeA][i])
      numUpFrom++;
  }
  if(flag2)
  {
    if(numUpFrom != numUpTo+1)
    {
//cout << "FF" << endl;
      return;
    }
    nextEdgeA++;
  }
  if(numUpFrom > numUpTo+1)
{
//cout << "AA" << endl;
    return;
}
//cout << "numUpFrom: " << numUpFrom << endl;
//cout << "numUpTo: " << numUpTo << endl;
  if(numUpFrom+maxRank-nextEdgeB < numUpTo+1)
{
//cout << "BB" << endl;
    return;
} 
  //add and delete edge from nextEdgeA to nextEdgeB
//cout << "checking for posets without " << nextEdgeA << " " << nextEdgeB<< endl;


/*  bool oldTagged[maxRank];
  bool oldSkipped[maxRank];
  for(int i=0; i<maxRank; i++)
  {
   oldTagged[i] = tagged[i];
   oldSkipped[i] = skipped[i];
  }
  if(!tagged[nextEdgeB])
    skipped[nextEdgeA] = true;*/

  int oldDeg[maxRank];
  for(int i=0; i<maxRank; i++)
    oldDeg[i] = deg[i];
  bool oldCovers[numRanks-1][maxRank][maxRank];
  for(int i=0; i<numRanks-1; i++)
  for(int j=0; j<maxRank; j++)
  for(int k=0; k<maxRank; k++)
    oldCovers[i][j][k] = covers[i][j][k];
  if(nextEdgeB == maxRank-1)
  {
    extendCovers(rank,numEdges,nextEdgeA+1,0);
  }
  else
    extendCovers(rank,numEdges,nextEdgeA,nextEdgeB+1);
  for(int i=0; i<maxRank; i++)
    deg[i] = oldDeg[i];
//  cout << "finished COVERS without " << nextEdgeA << " " << nextEdgeB << endl;

//  if(!skipped[nextEdgeA])
  {
/*    for(int i=0; i<maxRank; i++)
    {
       tagged[i] = oldTagged[i];
       skipped[i] = oldSkipped[i];
    }
    tagged[nextEdgeB] = true;*/
//cout << "checking for posets with " << nextEdgeA << " " << nextEdgeB << endl;
//  if(nextEdgeA != 0)
//  {
//    if(deg[nextEdgeA] == deg[nextEdgeA-1])
//      return;
//  }
    for(int i=0; i<maxRank; i++)
      oldDeg[i] = deg[i];
    deg[nextEdgeB] = deg[nextEdgeB] +1;
  for(int i=0; i<numRanks-1; i++)
  for(int j=0; j<maxRank; j++)
  for(int k=0; k<maxRank; k++)
    covers[i][j][k] = oldCovers[i][j][k];
  covers[rank][nextEdgeA][nextEdgeB] = true;
  if(nextEdgeB == maxRank-1)
  {
    extendCovers(rank,numEdges+1,nextEdgeA+1,0);
  }
  else
    extendCovers(rank,numEdges+1,nextEdgeA,nextEdgeB+1);
//  cout << "finished COVERS with " << nextEdgeA << " " << nextEdgeB << endl;
    for(int i=0; i<maxRank; i++)
      deg[i] = oldDeg[i];
  }
  return;
};

int main(int argc, char* argv[])
{
  cout << "with(posets);" << endl;
//  cout << "with(linalg);" << endl;
  for(int i=0; i<maxRank; i++)
  for(int j=0; j<maxRank; j++)
  for(int k=0; k<numRanks-1; k++)
    covers[k][i][j] = false;
  for(int i=0; i<maxRank; i++)
    rankSize[i] = 0;
  if(argc <2)
  {
  covers[0][0][0] = true;
  covers[1][0][0] = true;
  covers[1][0][1] = true;
  covers[2][0][0] = true;
  covers[2][1][0] = true;
  covers[2][0][1] = true;
  covers[2][1][2] = true;
  covers[3][0][0] = true;
  covers[3][1][0] = true;
  covers[3][0][1] = true;
  covers[3][2][1] = true;
  covers[3][0][2] = true;
  covers[3][1][3] = true;
  covers[3][2][4] = true;
  rankSize[0] = 1;
  rankSize[1] = 1;
  rankSize[2] = 2;
  rankSize[3] = 3;
  rankSize[4] = 5;
  edgeSize[0] = 1;
  edgeSize[1] = 2;
  edgeSize[2] = 4;
  edgeSize[3] = 7;
/*rankSize[5] = 7;
edgeSize[4] = 12;
covers[4][0][0] = true;
covers[4][0][1] = true;
covers[4][1][1] = true;
covers[4][1][2] = true;
covers[4][2][2] = true;
covers[4][1][3] = true;
covers[4][3][3] = true;
covers[4][2][4] = true;
covers[4][3][4] = true;
covers[4][3][5] = true;
covers[4][4][5] = true;
covers[4][4][6] = true;
rankSize[6] = 11;
edgeSize[5] = 19;
covers[5][0][0]=true;
covers[5][0][1]=true;
covers[5][1][1]=true;
covers[5][1][10]=true;
covers[5][2][2]=true;
covers[5][1][3]=true;
covers[5][2][3]=true;
covers[5][3][3]=true;
covers[5][2][4]=true;
covers[5][4][4]=true;
covers[5][3][5]=true;
covers[5][4][5]=true;
covers[5][4][6]=true;
covers[5][5][6]=true;
covers[5][3][7]=true;
covers[5][5][7]=true;
covers[5][5][8]=true;
covers[5][6][8]=true;
covers[5][6][9]=true;*/
  }
  else
  {
    ifstream fin;
    fin.open(argv[1]);
    while(fin)
    {
      bool flag = false;
//cout << "starting new line:" << endl;
      char* str = new char[MAX_LINE_LENGTH];
      fin.getline(str,MAX_LINE_LENGTH);
// cout << str << endl;
      cout << "# starting poset" << endl;
      char* left = strpbrk(str,"{");
      char* right = strpbrk(str,"}");
      int bigTop = 0;
      if(left != NULL && right != NULL)
      {
        char* s = strtok(left,"[],}{:= ");
        while(s != NULL)
        {
          int bot = (int)(s[0])-65;
          int botNum = atoi(&(s[1]));
          s = strtok(NULL,"[],}{:= ;");
          if(s == NULL)
            break;
          int top = (int)(s[0])-65;
          if(top > bigTop)
            bigTop = top;
          int topNum = atoi(&(s[1]));
          if(topNum+1 > rankSize[top])
            rankSize[top] = topNum+1;
          if(botNum+1 > rankSize[bot])
            rankSize[bot] = botNum+1;
//cout << "top is: " << top << ", topNum is: " << topNum << ", bot is: " 
//<< bot << ", botNum is: " << botNum << endl;
          covers[bot][botNum][topNum] = true;
          flag = true;
          s = strtok(NULL,"[],}{:= ;");
        }
      }
//      for(int i=0; i<=bigTop; i++)
//        cout << "rankSize[" << i << "] is: " << rankSize[i] << endl;
//cout << "covers is: " << endl;
//outputPoset2(bigTop-1);
      if(flag)
      {
        for(int i=0; i<maxRank; i++)
          deg[i] = 0;
        extendCovers2(bigTop);
      }
  for(int i=0; i<maxRank; i++)
  for(int j=0; j<maxRank; j++)
  for(int k=0; k<numRanks-1; k++)
    covers[k][i][j] = false;
  for(int i=0; i<maxRank; i++)
    rankSize[i] = 0;
    }
    return 0;
  }
/*  for(int i=0; i<maxRank; i++)
  {
    tagged[i] = false;
    skipped[i] = false;
  }*/
  for(int i=0; i<maxRank; i++)
    deg[i] = 0;
//  extendCovers(3,0,0,0);
  extendCovers2(4);
//  cout << "found " << numPosets << " posets" << endl;
//  cout << "made " << extendCalls << " calls to extendCovers" << endl;
/*  for(int i=1; i<=numPosets; i++)
  for(int j=i+1; j<=numPosets; j++)
    cout << "if isom(P" << i << ",P" << j << ") then " << i << "," << j << " fi;" << endl;
  for(int i=1; i<=numPosets; i++)
    cout << "plot_poset(P" << i << ",labels,proportional);" << endl;*/
//  cout << "plot_poset(P11,labels,proportional);" << endl;
//  cout << "plot_poset(P13,labels,proportional);" << endl;
  return 0;
}
