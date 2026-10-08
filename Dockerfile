FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
	build-essential cmake git \
	libgl1-mesa-dev libx11-dev libxrandr-dev \
	libxinerama-dev libxcursor-dev libxi-dev && \
	rm -rf /var/lib/apt/lists/* 

RUN git clone --depth 1 --branch 5.5 https://github.com/raysan5/raylib.git /raylib && \
	cmake -S /raylib -B /raylib/build -DBUILD_EXAMPLES=OFF && \
	cmake --build /raylib/build && \
	cmake --install /raylib/build && \
	rm -rf /raylib


WORKDIR /app
COPY main.cpp pdb_file_parser.h shader_commands.h .


RUN g++ main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o app
COPY molecules molecules
ENTRYPOINT ["./app"]
CMD ["molecules/LEU.cif"]
