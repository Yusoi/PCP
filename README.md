# PCP 
## TP1 (Shared Memory Optimization) ./TP1/
### Problem: Running a simulation of heat dispersion in a matrix
#### Optimizations:
##### Optimization flag
To test, manually change the optimization flag in the compiler.
The following tests were made:
- O0
- O1
- O2
- O3
- Ofast (not done yet)

##### Result matrix switching instead of copying
To test, uncomment the ***SWITCH*** flag on the Makefile

##### Heap Memory vs Stack Memory
To test ***heap memory***, uncomment ***one*** of the ***BIG_HEAP***, ***MEDIUM_HEAP*** or ***SMALL_HEAP***.
If all are commented, ***stack memory*** will be tested.

##### Inverse division
(Not Implemented yet)