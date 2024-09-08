g++ -fdiagnostics-color=always -g ./main/src/*.cpp ./include/glad/*.c ./include/imgui/*.cpp -o ./bin/trajectoryBuilder -I ./include -I ./main/src -L ./bin/ -l :libglfw.so -O2 -s
