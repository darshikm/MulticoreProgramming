/* These need to be compiled using the -I flag with gcc/clang..*/
#include "librace.h"
#include "stdatomic.h"
#include <threads.h>

using namespace std;

class Barrier //abstract class serving as an interface
{
 public:  
 virtual void await(int) = 0;
};

struct sense 
{
 atomic_bool mysense; 
};

/********   NODE CLASS *********/
class Node
{
 protected:
  atomic_int childCount;
  Node *parent;
  volatile bool threadSense; 
  int children;

 public:
  Node (Node *myParent, int count, bool globalSense)
  {
   children = count;
   atomic_init(&childCount, count);
   parent = myParent;
   threadSense = !(globalSense);
  }
     
  void childDone() 
  { 
   atomic_fetch_sub_explicit(&childCount, 1, memory_order_acq_rel); 
  }

  void await(struct sense *globalSenseState) 
  {
   while ((atomic_load_explicit(&childCount, memory_order_acquire)) > 0) { thrd_yield(); }
   atomic_store_explicit(&childCount, children, memory_order_release);
   if (parent != NULL) 
   { 
    parent->childDone(); 
    while (threadSense != atomic_load_explicit(&globalSenseState->mysense, memory_order_acquire)) { thrd_yield(); }
   } 
   else 
   {
    atomic_exchange_explicit(&globalSenseState->mysense, threadSense, memory_order_release);
    printf("\n*** Parent now toggled globalSense. Sync Done! ***\n");
   }
   threadSense = !threadSense;
  }
};

/********   STATIC TREE BARRIER CLASS *********/
class staticTreeBarrier : public Barrier
{
 protected:
  int radix, nodes;
  struct sense *globalSense; 
  
 public:
  Node **node;
  staticTreeBarrier(int , int );
  void await(int);
  void build(Node *thisparent, int depth);
};

staticTreeBarrier :: staticTreeBarrier(int size, int myRadix)
{
 radix = myRadix;
 nodes = 0;
 node = (Node **)malloc(size * sizeof(Node *));
 int depth = 0;

 globalSense = (struct sense *)malloc(sizeof(struct sense));
 atomic_init(&globalSense->mysense, false);
 
 if(radix == 1) depth = size - radix;
 else
 {
  while (size > 1) 
  {
   depth++;
   size = size/radix;
  }
 } 
 build(NULL, depth); 
}

void staticTreeBarrier :: build(Node *thisParent, int depth)
{
 bool senseVal = atomic_load_explicit(&globalSense->mysense, memory_order_acquire);
 if (depth == 0) { node[nodes] = new Node(thisParent, 0, senseVal); nodes++; }
 else
 {
  Node *newNode = new Node(thisParent, radix, senseVal); 
  node[nodes] = newNode;
  nodes++;
  for (int i = 0; i < radix; i++) build(newNode, depth - 1);
 }  
}

void staticTreeBarrier :: await(int id) 
{
 printf("thread %d is awaiting sync..\n", id); 
 node[id]->await(globalSense);
}
