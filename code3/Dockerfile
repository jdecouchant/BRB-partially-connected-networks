# our local base image
FROM ubuntu:20.04 

#EXPOSE 80/tcp
#EXPOSE 8760/tcp
#EXPOSE 8761/tcp
#EXPOSE 8762/tcp
#EXPOSE 8763/tcp

# install build dependencies 

ENV DEBIAN_FRONTEND=noninteractive

# install some requirements
RUN apt-get update \
    && apt-get install -y apt-utils git wget iptables libdevmapper1.02.1 \
    && apt-get install -y build-essential ocaml ocamlbuild automake autoconf libtool python-is-python3 libssl-dev git cmake perl \
    && apt-get install -y libcurl4-openssl-dev protobuf-compiler libprotobuf-dev debhelper reprepro unzip build-essential python \
    && apt-get install -y lsb-release software-properties-common \
    && apt-get install -y pkg-config libuv1-dev python3-matplotlib \
    && apt-get install -y emacs vim \
    && apt-get install -y python3-pip net-tools iputils-ping iproute2 iputils-ping
    
RUN pip install networkx

# install a newer version of openssl
#RUN wget https://www.openssl.org/source/openssl-1.1.1i.tar.gz \
#    && tar -xvzf openssl-1.1.1i.tar.gz \
#    && cd openssl-1.1.1i \
#    && ./config --prefix=/usr no-mdc2 no-idea \
#    && make \
#    && make install
    
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY Makefile       /app
COPY test.py /app
COPY App            /app/App
COPY salticidae     /app/salticidae
COPY server         /app/
COPY log            /app/log
COPY addresses      /app
COPY topology.txt   /app

CMD ["/bin/bash"]




