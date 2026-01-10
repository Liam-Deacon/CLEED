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

RUN cmake --build build --parallel="$(nproc)" && \
    cmake --install build --prefix /opt/cleed

FROM ubuntu:22.04 AS development

RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    ninja-build \
    libtiff-dev \
    libpng-dev \
    zlib1g-dev \
    gdb \
    && rm -rf /var/lib/apt/lists/*

RUN useradd --system --uid 1000 --create-home --home-dir /home/cleed --shell /usr/sbin/nologin cleed

USER cleed
WORKDIR /home/cleed

FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libtiff6 \
    libpng16-16 \
    zlib1g \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /opt/cleed/ /usr/local/

# Update library cache instead of using LD_LIBRARY_PATH
RUN ldconfig

RUN useradd --system --uid 1000 --create-home --home-dir /home/cleed --shell /usr/sbin/nologin cleed \
    && chown -R cleed:cleed /usr/local

USER cleed
WORKDIR /home/cleed

ENTRYPOINT ["cleed_nsym"]
CMD ["-h"]
