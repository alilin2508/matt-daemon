FROM debian:latest

RUN apt update && apt upgrade
RUN apt install -y	make \
					gcc \
					g++ \
					procps \
					netcat-traditional \
					netcat-openbsd

COPY . /root/matt-daemon
WORKDIR /root/matt-daemon
