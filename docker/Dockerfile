FROM debian:buster

########################################################################
#
# install
#
RUN apt update -yq && \
    apt install -yq \
        unzip \
        build-essential \
        curl \
        # ccmix deps
        libcrypto++-dev \
    && \
    mkdir /src && \
    curl -Ls https://github.com/AubreyHewes/ccmix/archive/master.zip -o /src/ccmix-master.zip && \
    unzip /src/ccmix-master.zip -d /src && \
    cd /src/ccmix-master && \
    # fix narrowing errors on make
    OPTFLAGS="-Wno-narrowing" \
        make && \
    make install && \
    # fix for incorrect PATH destination
    mv /usr/local/lib/ccmix /usr/local/bin/ccmix

########################################################################
#
# clean up
# todo apt cleanup (will do when all known deps are fixed)
RUN \
    rm -rf /src

########################################################################
# run it

# container user (first debian/ubuntu/etc created user)
# else run with -u 100x
# else run with -u 0 (root)
USER 1000

# cmd.. todo replace with entrypoint that checks for available "global mix database.dat"
CMD ["ccmix"]
