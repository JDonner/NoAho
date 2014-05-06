# run this as:
#   . test-all-configurations.bash
# because 'workon' is a bash function (from virtualenvwrapper)
# and won't propagate into a new shell.

# set -e

# Your work directory
ROOT_DIR=~/projects/noaho-2014-04-26/noaho
SRC_DIR=${ROOT_DIR}/src
# noaho.cpp is the python wrapper that Cython generates
NOAHO_WRAPPER=${SRC_DIR}/noaho.cpp
LOGFILE=${ROOT_DIR}/test-results.log
VERSIONED_CYTHON="Cython-0.20.1"
VIRTUAL_ENV_NAME="noaho_setup"


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
    tar zxvf ${VERSIONED_CYTHON}.tar.gz || { echo "no Cython tarball" $LOGFILE 2>&1; return 1; }
    cd ${VERSIONED_CYTHON}
    ${py} setup.py install || { echo "Cython installation failed" $LOGFILE 2>&1; return 1; }
    cd ${ROOT_DIR}
    rm -rf $VERSIONED_CYTHON
    log "Haz cython"
}


cython_noaho () {
    log "Cythoning noaho"
    py="$1"
    cd ${ROOT_DIR}
    clean_all
    if [[ -e $NOAHO_WRAPPER ]]
    then
        log "Oook; failed to get rid of $NOAHO_WRAPPER"
    fi
    log "expect $NOAHO_WRAPPER to be absent..."
    log $(ls $NOAHO_WRAPPER)
    ${py} ${ROOT_DIR}/cython-regenerate-noaho-setup.py build_ext --inplace || { echo "regeneration failed" $LOGFILE 2>&1 ; return 1; }
    if [[ ! -e $NOAHO_WRAPPER ]]
    then
        log "Failed to generate $NOAHO_WRAPPER"
    else
        log "expect $NOAHO_WRAPPER to be present..."
        log $(ls $NOAHO_WRAPPER)
    fi
    clean_build
}


setup_install_noaho () {
    log "setup installing noaho"
    py="$1"
    clean_build
    cd ${ROOT_DIR}
    ${py} setup.py install || { echo "noaho installation failed" $LOGFILE 2>&1; return 1; }
}


test_noaho () {
    py="$1"
    log "testing..."
    ${py} ${ROOT_DIR}/test-noaho.py >> ${LOGFILE} 2>&1 || { echo "noaho test failed" $LOGFILE 2>&1 ; return 1; }
    log "Done"
}

clean_build () {
    # not just 'noaho.so' - python3's version is eg 'noaho.cpython-33m.so'
    rm -rf ${ROOT_DIR}/build ${SRC_DIR}/*.so
}

clean_all () {
    clean_build
    rm -f $NOAHO_WRAPPER
}


clean_all
# clean test log
rm -f ${LOGFILE}

# Don't give these full paths - in the beginning they refer to the system python,
# later to the virtualenv version
for py in "python2" "python3"
do
    log "testing $py"
    mkvirtualenv --python=${py} $VIRTUAL_ENV_NAME
    log "making venv $VIRTUAL_ENV_NAME"
    cd ${ROOT_DIR}
    CONFIG=$VIRTUAL_ENV_NAME
    workon $CONFIG
    install_cython ${py}
    # We must use the 'target' python, because the process imports a Cython module
    cython_noaho ${py}
    setup_install_noaho ${py}
    clean_all
    test_noaho ${py}
    #read -p "Done $CONFIG" yn
    clean_all
    deactivate
    rmvirtualenv $VIRTUAL_ENV_NAME
    log "removed venv $VIRTUAL_ENV_NAME"
done


on_exit () {
    log "SCRIPT FAILURE"
}

# trap errors
trap on_exit EXIT

cat ${ROOT_DIR}/test-results.log
cd ${ROOT_DIR}
