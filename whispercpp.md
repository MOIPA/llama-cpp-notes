# whisper.cpp 

llama.cpp 的姊妹项目，用于部署whisper模型做asr

git clone 项目后

如果本机的gcc工具链版本太低，需要改配置：`./ggml/src/CMakeLists.txt`

```c
 if (CMAKE_SYSTEM_NAME MATCHES "Linux")
     target_link_libraries(ggml PRIVATE dl)
 endif()
```

改为

```c
 if (CMAKE_SYSTEM_NAME MATCHES "Linux")
     target_link_libraries(ggml PRIVATE dl stdc++fs)
 endif()
```

## NDK交叉编译

