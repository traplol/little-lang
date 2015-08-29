#!/bin/bash

if ! make; then
    exit 1
fi

for f in bin/*; do
    echo "Running '$f'"
    ./$f
done
