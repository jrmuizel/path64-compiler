To run the validation, go into 'testing' directory and in order launch:

--> To test SX core builtins
make data
make gensrc
make reference
make cbuilt
make check

--> To test X3 builtins
make data MYTEST=X3
make gensrc MYTEST=X3
make reference MYTEST=X3
make cbuilt MYTEST=X3
make check MYTEST=X3

