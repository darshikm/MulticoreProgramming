# MulticoreProgramming-BoundedStack-

Coursework assignments on Datastructure implementations of:
Concurrent Stack
Static Tree Barrier

The idea/motivation is to get acquainted to C/C++ memory model and to use the weakest memory orderings that provide
correctness for the datastructure implementations.

To help debug the data structure, we make use of the CDSChecker.
CDSChecker is a model checker for C11/C++11 which exhaustively explores the behaviors of code under the
C/C++ memory model. It uses partial order reduction as well as a few other novel techniques to eliminate time
spent on redundant execution behaviors and to significantly shrink the state space. The model checking algorithm is
described in more detail in an OOPSLA 2013 paper.

http://plrg.eecs.uci.edu/software_page/42-2/

Do the steps of installing CDS Checker and linking libmodel.so 
