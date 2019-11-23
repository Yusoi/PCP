# PCP 
## [TP1 (Shared Memory Optimization)](TP1/)
## Problem: Running a simulation of heat dispersion in a matrix

***ALL OUTPUTS ARE SAVED IN [TP1/Outputs](TP1/Outputs) AS output.txt***

### Optimizations:
#### Optimization flag
To test, manually change the optimization flag in the compiler.
The following tests were made:
- O0
- O1
- O2
- O3
- Ofast (not done yet)

#### Heap Memory vs Stack Memory
To test ***heap memory***, uncomment ***one*** of the ***BIG_HEAP***, ***MEDIUM_HEAP*** or ***SMALL_HEAP***.
If all are commented, ***stack memory*** will be tested.

#### Result matrix switching instead of copying
To test, uncomment the ***SWITCH*** flag on the Makefile

#### Inverse division
(Not Implemented yet)

### TODO List:
- Testing Optimization flags and find the best one for the algorithm. (done)
- Testing the same size Heap Memory vs Stack Memory, in this case 1023x1023.
- Test different memory block sizes in _mm_malloc_.
- Improve the algorithm by matrix switching instead o copying.
- See how scalable the algorithm feels by increasing the matrix size.
- (Maybe) Implement inverse division.
