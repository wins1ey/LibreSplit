#!/bin/bash

set -ex

if [ "$GITHUB_EVENT_NAME" == "pull_request" ]; then 
    git fetch origin $GITHUB_BASE_REF:refs/remotes/origin/$GITHUB_BASE_REF
    for f in $(git diff --diff-filter=AM --name-only origin/$GITHUB_BASE_REF 'src/**.c' 'src/**.cpp' 'src/**.h' ); do
        if [ "$(diff -u <(cat $f) <(clang-format $f))" != "" ]
        then
            echo "run format"
            exit 1
        fi
    done
fi
