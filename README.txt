# Branch-Prediction-Simulator
A program simulating the accuracy of different branch predictors.
usage: ./predictors TRACE_FILE OUTPUT_FILE
  TRACE FILE is a text file with a branch instruction address, followed by a T or NT to indicate taken or not taken,
    and a target instruction address. (sizes of the predictor tables are shown in the function calls in predictors.cpp)
  OUTPUT_FILE is a text file containing the simplified results of the predictor (which is also outputted to the console).

A line in the output with more than 1 entry means the same predictor is being tested but with a different predictor table size
  for each entry.

sample usage:
tman1@remote02:~/Branch-Prediction-Simulator$ ./predictors sample_trace1.txt output.txt
# correct predictions,# total branches;
787970,2000000; <- Always taken predictor
1212030,2000000; <- Always not taken predictor
1272056,2000000; 1271610,2000000; 1635134,2000000; 1756616,2000000; 1999124,2000000; 1999353,2000000; 1999526,2000000; <- Single bit bimodal predictor
1472734,2000000; 1475763,2000000; 1800746,2000000; 1876864,2000000; 1999472,2000000; 1999581,2000000; 1999665,2000000; <- Two bit saturating bimodal predictor
1999630,2000000; 1939177,2000000; 1999563,2000000; 1999597,2000000; 1999592,2000000; 1999601,2000000; 1999579,2000000; 1999510,2000000; 1999561,2000000; <- Gshare predictor
1999679,2000000; <- Tournament predictor
787434,788032; <- Branch target buffer predictor
