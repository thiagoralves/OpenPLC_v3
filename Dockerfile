FROM debian

COPY . /workdir
RUN cd /workdir && ./install.sh docker
WORKDIR /workdir
ENTRYPOINT ["./start_openplc.sh"]
