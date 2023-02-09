FROM debian:bullseye-20211201
COPY . /workdir
WORKDIR /workdir
RUN mkdir /persistent
VOLUME /persistent
RUN ./install.sh docker
RUN touch /persistent/mbconfig.cfg
RUN ln -s /persistent/mbconfig.cfg /workdir/webserver/mbconfig.cfg
RUN touch /persistent/persistent.file
RUN ln -s /persistent/persistent.file /workdir/webserver/persistent.file
ENTRYPOINT ["./start_openplc.sh"]
