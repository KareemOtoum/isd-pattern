
docker run --rm -v "$PWD":/work -w /work gcc:16 \
  bash -c "g++ -Iinclude --std=c++26 -freflection src/main.cpp -o main && ./main"