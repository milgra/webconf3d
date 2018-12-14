framework=`find ../../../framework -name "*.c"`
sources=`find ../../sources -name "*.c"`
wrapper="../../../wrappers/asmjs/main.c"

echo $framework
echo $sources
echo $wrapper

emcc -O3 -DASMJS $framework $sources $wrapper -o main.html -s ALLOW_MEMORY_GROWTH=1 --preload-file ./resources -I '../../../framework' -I '../../sources' 
