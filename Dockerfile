# Stage 1: build with musl toolchain → fully static binary
FROM alpine:3.20 AS builder

RUN apk add --no-cache cmake make g++ musl-dev

WORKDIR /src
COPY CMakeLists.txt .
COPY src/ ./src/

# force static linking via musl
RUN cmake -B build \
      -DCMAKE_EXE_LINKER_FLAGS="-static" \
      -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build -j$(nproc)

# Stage 2: scratch — nothing but the binary
FROM scratch

COPY --from=builder /src/build/nitip /nitip

WORKDIR /data
EXPOSE 6379

ENTRYPOINT ["/nitip"]
