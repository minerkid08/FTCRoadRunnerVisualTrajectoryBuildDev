clear
echo "building"
rm ./out.exe
g++ -Iinclude -Imain/src -g main/src/*.cpp include/glad/*.c include/imgui/*.cpp -Lbin -lglfw3 -o out.exe

if [ -f "./out.exe" ]; then
  echo "running"
  ./out
fi
