FROM debian:bullseye-20211201

COPY . /workdir
RUN cd /workdir && ./install.sh docker
WORKDIR /workdir
ENTRYPOINT ["./start_openplc.sh"]
