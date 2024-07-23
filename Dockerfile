FROM debian:bullseye-20211201
COPY . /workdir
WORKDIR /workdir
RUN mkdir /persistent
VOLUME /persistent
RUN ./install.sh docker
RUN touch /persistent/mbconfig.cfg
RUN touch /persistent/persistent.file
RUN mkdir /persistent/st_files
RUN cp /workdir/webserver/openplc.db /persistent/openplc.db
RUN mv /workdir/webserver/openplc.db /workdir/webserver/openplc_default.db
RUN cp /workdir/webserver/dnp3.cfg /persistent/dnp3.cfg
RUN mv /workdir/webserver/dnp3.cfg /workdir/webserver/dnp3_default.cfg
RUN cp -r /workdir/webserver/st_files/ /persistent/st_files/
RUN mv /workdir/webserver/st_files /workdir/webserver/st_files_default
RUN cp /workdir/webserver/active_program /persistent/active_program
RUN mv /workdir/webserver/active_program /workdir/webserver/active_program_default
RUN ln -s /persistent/mbconfig.cfg /workdir/webserver/mbconfig.cfg
RUN ln -s /persistent/persistent.file /workdir/webserver/persistent.file
RUN ln -s /persistent/openplc.db /workdir/webserver/openplc.db
RUN ln -s /persistent/dnp3.cfg /workdir/webserver/dnp3.cfg
RUN ln -s /persistent/st_files /workdir/webserver/st_files
RUN ln -s /persistent/active_program /workdir/webserver/active_program

ENTRYPOINT ["./start_openplc.sh"]
