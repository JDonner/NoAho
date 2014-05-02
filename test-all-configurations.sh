# run this as:
#   . test-all-configurations.bash
# because 'workon' is a bash function (from virtualenvwrapper)
# and won't propagate into a new shell.


# Your work directory
ROOT_DIR=~/projects/noaho-2014-04-26/noaho
NOAHO_DIR=${ROOT_DIR}/noaho
# noaho.cpp is the python wrapper that Cython generates
NOAHO_WRAPPER=${NOAHO_DIR}/noaho.cpp
LOGFILE=${ROOT_DIR}/test-results.log
VERSIONED_CYTHON="Cython-0.20.1"


log () {
    msg="$1"
    echo "$CONFIG: $msg"
    echo "$CONFIG: $msg" >> $LOGFILE 2>&1
}


install_cython () {
    py="$1"
    cd ${ROOT_DIR}
    rm -rf ${VERSIONED_CYTHON}
    # Expects a Cython tarball in $ROOT_DIR
    tar zxvf ${VERSIONED_CYTHON}.tar.gz
    cd ${VERSIONED_CYTHON}
    ${py} setup.py install
    cd -
    rm -rf $VERSIONED_CYTHON
    log "Haz cython"
}


cython_noaho () {
    log "Cythoning noaho"
    py="$1"
    clean_all
    cd $NOAHO_DIR
    if [[ -e $NOAHO_WRAPPER ]]
    then
        log "Oook; failed to get rid of $NOAHO_WRAPPER"
    fi
    log "expect $NOAHO_WRAPPER to be absent..."
    log $(ls $NOAHO_WRAPPER)
    ${py} ../cython-regenerate-noaho-setup.py build_ext --inplace
    if [[ ! -e $NOAHO_WRAPPER ]]
    then
        log "Failed to generate $NOAHO_WRAPPER"
    else
        log "expect $NOAHO_WRAPPER to be present..."
        log $(ls $NOAHO_WRAPPER)
    fi
    clean_build
    cd ${ROOT_DIR}
}


setup_install_noaho () {
    log "setup installing noaho"
    py="$1"
    cd ${NOAHO_DIR}
    clean_build
    ${py} ${ROOT_DIR}/setup.py install
}


#pip_install_noaho () {
#    log "pip installing noaho"
#    py="$1"
#    cd ${NOAHO_DIR}
#    clean_build
#    ${py} setup.py install
#    cd ${ROOT_DIR}
#}


test_noaho () {
    py="$1"
    log "testing..."
    ${py} ${ROOT_DIR}/test-noaho.py >> ${LOGFILE} 2>&1
    log "Done"
}

clean_build () {
    # not just 'noaho.so' - python3's version is eg 'noaho.cpython-33m.so'
    rm -rf ${NOAHO_DIR}/build ${NOAHO_DIR}/*.so
}

clean_all () {
    clean_build
    rm -f $NOAHO_WRAPPER
}

clean_up () {
    clean_all
    deactivate
}

# clean test log
rm -f ${LOGFILE}


# Python 2, distutils (setup.py)
py="python2"
cd ${ROOT_DIR}
CONFIG="noaho-p2-setup"
workon $CONFIG
install_cython ${py}
# We must use the 'target' python, because the process imports a Cython module
cython_noaho ${py}
setup_install_noaho ${py}
clean_all
test_noaho ${py}
#read -p "Done $CONFIG" yn
clean_up


## Python 2, pip
#py="python2"
#cd ${ROOT_DIR}
#CONFIG="noaho-p2-pip"
#workon $CONFIG
#install_cython ${py}
#cython_noaho ${py}
#pip_install_noaho ${py}
#clean_all
#test_noaho ${py}
##read -p "Done $CONFIG" yn
#clean_up


# Python 3, distutils (setup.py)
py="python3"
cd ${ROOT_DIR}
CONFIG="noaho-p3-setup"
workon $CONFIG
install_cython ${py}
cython_noaho ${py}
setup_install_noaho ${py}
clean_all
test_noaho ${py}
#read -p "Done $CONFIG" yn
clean_up


## Python 3, pip
#py="python3"
#cd ${ROOT_DIR}
#CONFIG="noaho-p3-pip"
#workon $CONFIG
#install_cython ${py}
#cython_noaho ${py}
#pip_install_noaho ${py}
#clean_all
#test_noaho ${py}
##read -p "Done $CONFIG" yn
#clean_up

cat ${ROOT_DIR}/test-results.log
cd ${ROOT_DIR}
