# crazyflie-matlab-client

Following steps are for Ubuntu OS:

->Clone Library (CF_lib)

->Open Matlab and change workspace to the folder containing the CF_lib 

->Setup C++ compiler by typing the command  mex -setup C++

->In the make.m file edit the pathToLibCFLIE to the actual path where you stored the libclfie.so.  mex -v CFLAGS='$CFLAGS -fPIC' 
-LpathToLIBCFLIE -lcflie -lusb-1.0 sfun_cflie_simple.cp

->Once compiled successfully you should be able to run the simulink test_simple.slx
