# Technology Mapping
Implement a two-stage algorithm for technology mapping in FPGA design.
* Minimum Level Two-input Decomposition
* Delay Optimal FPGA Technology Mapping

## How to Compile
In "Technology_Mapping/src/", enter the following command:
```
$ make
```
It will generate the executable file "map" in "Technology_Mapping/bin/".

If you want to remove it, please enter the following command:
```
$ make clean
```

## How to Run
Usage:
```
$ ./<exe> -k <k-LUT> <input file> <output file>
```

e.g.
```
$ ./map -k 4 ../testcases/10aoi_sample01.blif ../output/10aoi_sample01.4.blif
```

## How to Test
In "Technology_Mapping/src/", enter the following command:
```
$ make test ${name}
```
It will test on ${name} with k=4.

e.g. test on "10aoi_sample01" with k=4
```
$ make test 10aoi_sample01
```
