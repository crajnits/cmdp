src_dir=.
build_dir=./build

#rm -rf ${build_dir} # Clean build

# Generate Build system
/usr/bin/cmake \
    -DCMAKE_EXPORT_COMPILE_COMMANDS:BOOL=TRUE \
    -DCMAKE_BUILD_TYPE:STRING=Debug \
    -DCMAKE_C_COMPILER:FILEPATH=/bin/gcc-9 \
    -DCMAKE_CXX_COMPILER:FILEPATH=/bin/g++-9 \
    -H${src_dir} \
    -B${build_dir} \
    -G "Unix Makefiles" \

# Build lib and test app
/usr/bin/cmake --build ${build_dir}

# Run test app
${build_dir}/cmdp_test --filename test.jpg --ratio 15.6
