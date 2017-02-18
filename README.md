# 64-bits Processor topology

## Build
 1- Download or clone the source code into a working directory.
 
 2- Build the programs.
```
make
```

## Run (as root)

 3- Load the kernel module.
```
modprobe topology
```
 _or_
```
insmod topology.ko
```
 4- Print the topology.
```
dmesg
```

### Unload

 5- Unload the kernel module with the ```rmmod``` command
```
rmmod topology.ko
```

## Screenshots
![alt text](http://blog.cyring.free.fr/images/topology.png "topology")  

# About
[CyrIng](https://github.com/cyring)

Copyright (C) 2017 CYRIL INGENIERIE
 -------
