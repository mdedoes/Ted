set -u -e -x

make clean
make -j

rm */*.o

for makefile in */makefile
do
    sed -i -e '/-g -O2/d' ${makefile}
done

make CC=cppcheck CFLAGS=''
