One needs to make use of the CDSChecker
(CDSChecker link: http://plrg.eecs.uci.edu/software_page/42-2 )
Follow the installation steps of model-checker. Compile the code using g++ and 
link the dynamic library 'libmodel.so'

One should be able to compile our code with the command (or refer to the Makefile):
g++ -Wall -std=c++11 staticTreeBarrier.cc -L. -lmodel -lstdc++ -o staticTreeBarrier

The need for a staticTreeBarrier is simple. When we are in a multi-threaded environment and want to synchronize
several threads by means of some kind of an atomic flag (a shared memory space for the threads) mechanism,
whose value shall monitor the progress of the threads or serve as a barrier mechanism for all threads to reach to a certain
limit of progress condition; we are looking at a readers-writer problem with tremendous contention on the poor flagged memory.

Thus, to solve it, we have a tree-structured Barrier in order that the progress condition or the barrier condition percolates
differently with the intuition of tree hierarchy.
