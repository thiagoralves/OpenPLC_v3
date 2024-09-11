FROM debian:bullseye-20240722

COPY . /workdir

WORKDIR /workdir

RUN mkdir /persistent

VOLUME /persistent

RUN ./install.sh docker \
    && touch /persistent/mbconfig.cfg \
    && touch /persistent/persistent.file \
    && mkdir /persistent/st_files \
    && cp /workdir/webserver/openplc.db /persistent/openplc.db \
    && mv /workdir/webserver/openplc.db /workdir/webserver/openplc_default.db \
    && cp /workdir/webserver/dnp3.cfg /persistent/dnp3.cfg \
    && mv /workdir/webserver/dnp3.cfg /workdir/webserver/dnp3_default.cfg \
    && cp -r /workdir/webserver/st_files/ /persistent/st_files/ \
    && mv /workdir/webserver/st_files /workdir/webserver/st_files_default \
    && cp /workdir/webserver/active_program /persistent/active_program \
    && mv /workdir/webserver/active_program /workdir/webserver/active_program_default \
    && ln -s /persistent/mbconfig.cfg /workdir/webserver/mbconfig.cfg \
    && ln -s /persistent/persistent.file /workdir/webserver/persistent.file \
    && ln -s /persistent/openplc.db /workdir/webserver/openplc.db \
    && ln -s /persistent/dnp3.cfg /workdir/webserver/dnp3.cfg \
    && ln -s /persistent/st_files /workdir/webserver/st_files \
    && ln -s /persistent/active_program /workdir/webserver/active_program

ENTRYPOINT ["./start_openplc.sh"]
