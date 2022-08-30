#!/bin/bash

docker container run -it brachadolev ./server 0 topology.txt 10 3 100 5000000 0 500 True True True True True True True True True True True False 1 -p 127.0.0.1:8760:80 & 

docker container run -it brachadolev ./server 1 topology.txt 10 3 100 5000000 0 500 True True True True True True True True True True True False 1 -p 127.0.0.1:8761:80 &  

docker container run -it brachadolev ./server 2 topology.txt 10 3 100 5000000 0 500 True True True True True True True True True True True False 1 -p 127.0.0.1:8762:80 & 

docker container run -it brachadolev ./server 3 topology.txt 10 3 100 5000000 0 500 True True True True True True True True True True True False 1 -p 127.0.0.1:8763:80 & 


