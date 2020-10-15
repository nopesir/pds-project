FROM ubuntu:bionic

RUN apt-get update
RUN apt-get install -y wget tar xz-utils libcurl4 libcurl4-openssl-dev libglib2.0-0
WORKDIR /home/ubuntu
RUN wget "https://github.com/nopesir/pds-project/releases/download/v1.0.3/server_v1.0.3-linux-x64.tar.xz"
RUN tar -xf server_v1.0.3-linux-x64.tar.xz
WORKDIR /home/ubuntu/server_v1.0.3-linux-x64
RUN ls
RUN chmod +x run.sh
EXPOSE 63505
CMD ["/bin/bash", "./run.sh"]


