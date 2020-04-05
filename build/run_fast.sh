make

if [ $? -eq 0 ]; then
    ./pong

    echo "Exit with code " $?
else
    exit 1
fi

