#!/bin/bash

HOME=$1
TARGET=$2
USER_PATH="${HOME}/user"
TARGET_PATH="${USER_PATH}/$2"

createDirectory(){
    if [ -d ${TARGET_PATH} ]
    then
        echo "had create dir ${TARGET_PATH}"
        exit 1
    fi

    mkdir ${TARGET_PATH}
    echo -e "add_wast_executable(TARGET ${TARGET} \n INCLUDE_FOLDERS "\${STANDARD_INCLUDE_FOLDERS}" \n LIBRARIES  libplaton libc++ libc \n DESTINATION_FOLDER \${CMAKE_CURRENT_BINARY_DIR})" >> ${TARGET_PATH}/CMakeLists.txt
    echo "//auto create contract" > "${TARGET_PATH}/${TARGET}.cpp"
}

appendCMakeDir(){
    cmd="add_subdirectory(${TARGET})"
    add=`cat ${USER_PATH}/CMakeLists.txt | grep "${cmd}" | wc -l`

    if [ "-$add" -eq "-0" ]
    then
        echo "${cmd}" >> "${USER_PATH}/CMakeLists.txt"
    fi
}



main(){

    if [ "${HOME}" == "help" ]
    then
        echo "autoproject [home] [target]"
        exit 1
    fi

    if [ ! -d "${HOME}" ]
    then
        echo "doesn't have ${HOME}"
        exit 1
    fi

    if [ "-${TARGET}" == "-" ]
    then
        echo "doesn't have target" 
        exit 1
    fi

    createDirectory   

    num=$?
    if [ -n $num ]
    then
       appendCMakeDir
    fi
}

main
