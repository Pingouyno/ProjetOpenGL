PROJECT_NAME='ProjetOpenGL'
cmake -S . -B build
make -C build
if [ $? -eq 0 ]; then
    ./build/${PROJECT_NAME}
else
    echo "**ERREUR : COMPILATION ARRÊTÉE**"
    exit 1
fi 
