FROM ubuntu:22.04 AS build

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    libtiff-dev \
    libpng-dev \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DWITH_QT=OFF \
    -DWITH_OPENCL=OFF \
    -DWITH_PHASESHIFTS=OFF \
    -DINSTALL_DOC=OFF \
    -DBUILD_TESTING=OFF

RUN cmake --build build --parallel
RUN cmake --install build --prefix /opt/cleed

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libtiff6 \
    libpng16-16 \
    zlib1g \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/cleed/ /usr/local/

ENTRYPOINT ["cleed_nsym"]
CMD ["-h"]

