# Install
Create build directory

```
mkdir build
cd build
```

If you have a virtual environment, do this, otherwise default values will be used.

```
cmake -D PYTHON_INCLUDE_DIRS=~/.pyenv/versions/3.5.0/include/python3.5m \
    -D PYTHON_INCLUDE_DIR=~/.pyenv/versions/3.5.0/include/python3.5m \
    -D PYTHON_LIBRARY=/usr/lib/libpython3.so \
    -D PYTHON_EXECUTABLE=~/.pyenv/versions/3.5.0/bin/python ..
```
