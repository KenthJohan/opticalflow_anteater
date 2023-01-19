Compile different motion estimator

```
$ make
$ make oflow=oflow_default.cpp
$ make oflow=oflow_farneback.cpp
$ make oflow=oflow_lucaskanade.cpp
$ make oflow=oflow_phasecorr.cpp
```

Compile and run command
```
$ make && ./build/demo1
$ make oflow=oflow_default.cpp && ./build/demo1
$ make oflow=oflow_farneback.cpp && ./build/demo1
$ make oflow=oflow_lucaskanade.cpp && ./build/demo1
$ make oflow=oflow_phasecorr.cpp && ./build/demo1
```