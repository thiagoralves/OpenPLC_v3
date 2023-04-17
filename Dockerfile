FROM debian:bullseye-20211201
COPY . /workdir
WORKDIR /workdir
RUN mkdir /persistent
VOLUME /persistent
RUN mkdir /libexternal
RUN apt-get update && \
    apt-get -y install git build-essential gcc pkg-config cmake python python3 make 
RUN git clone https://github.com/open62541/open62541.git
WORKDIR /workdir/open62541
COPY webserver/openplc.db /workdir/open62541/webserver/openplc.db
RUN mkdir build
WORKDIR /workdir/open62541/build
RUN cmake .. -DBUILD_SHARED_LIBS=ON -D CMAKE_BUILD_TYPE=Release -DUA_LOGLEVEL=100 
RUN make
RUN make install
WORKDIR /workdir
RUN cp /workdir/open62541/build/bin/libopen62541.so.1 /workdir/webserver/core/libopen62541.so.1
RUN ./install.sh docker
RUN touch /persistent/mbconfig.cfg
RUN touch /persistent/persistent.file
RUN mkdir /persistent/st_files
RUN cp /workdir/webserver/openplc.db /persistent/openplc.db
RUN mv /workdir/webserver/openplc.db /workdir/webserver/openplc_default.db
RUN cp /workdir/webserver/dnp3.cfg /persistent/dnp3.cfg
RUN mv /workdir/webserver/dnp3.cfg /workdir/webserver/dnp3_default.cfg
RUN cp /workdir/webserver/st_files/* /persistent/st_files
run mv /workdir/webserver/st_files /workdir/webserver/st_files_default
RUN ln -s /persistent/mbconfig.cfg /workdir/webserver/mbconfig.cfg
RUN ln -s /persistent/persistent.file /workdir/webserver/persistent.file
RUN ln -s /persistent/openplc.db /workdir/webserver/openplc.db
RUN ln -s /persistent/dnp3.cfg /workdir/webserver/dnp3.cfg
RUN ln -s /persistent/st_files /workdir/webserver/st_files

ENTRYPOINT ["./start_openplc.sh"]
