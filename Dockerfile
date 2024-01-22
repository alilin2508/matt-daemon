FROM debian:latest

RUN apt update && apt upgrade
RUN apt install -y	make \
					gcc \
					g++ \
					procps \
					netcat-traditional \
					netcat-openbsd \
					net-tools \
					util-linux 

COPY . /root/matt-daemon
WORKDIR /root/matt-daemon
