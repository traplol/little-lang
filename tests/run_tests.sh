#!/bin/bash

HAS_VALGRIND=0
command -v valgrind >/dev/null 2>&1 && HAS_VALGRIND=1

if ! make; then
    exit 1
fi


run_with_valgrind() {
    if [ "$HAS_VALGRIND" -eq 0 ]; then
        echo "Valgrind isn't installed; this requires valgrind to run!"
        return
    fi
    valgrind --leak-check=full --error-exitcode=1 ./$1 2> /dev/null
    if [ "$?" -eq 1 ]; then
        echo "valgrind found errors with '$1'"
    else
        echo "valgrind found no errors with '$1'"
    fi
}

run() {
    if [[ "$run_with_valgrind_flag" ]]; then
        run_with_valgrind $1
    else
        ./$1
    fi
}

run_all() {
    for f in bin/*; do
        echo "Running '$f'"
        run $f
    done
}

while [ $# -gt 0 ]
do
    case "$1" in
        --all ) run_all_flag=true; shift ;;
        --with-valgrind ) run_with_valgrind_flag=true; shift ;;
        -- ) shift; break ;;
        * )
            run $1
            shift ;;
    esac
done

if [[ "$run_all_flag" ]] ; then
    run_all
fi
