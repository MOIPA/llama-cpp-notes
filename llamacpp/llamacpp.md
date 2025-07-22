# llama.cpp

对llamacpp项目的学习和理解

## linux下编译安装

使用了源码安装，系统newstartos，gcc和g++是8.4.1

1. 安装ccache，命令cp到usr/bin就行

2. git clone https://github.com/ggml-org/llama.cpp 然后 cd llama.cpp

3. echo 'target_link_libraries(ggml PRIVATE stdc++fs)' >> CMakeLists.txt  ： gcc9.0之后才默认并入stdc++fs，在这之前的版本都需要手动引入库
4. cmake -S . -B build -DCMAKE_EXE_LINKER_FLAGS="-lstdc++fs"
5. cmake --build build --config Release

6. 编译完成后命令都在build/bin下，默认不会make install，可以手动cp到usr/local/bin

7. 可选安装：sudo cp build/bin /usr/local/bin

## android ndk编译和使用



### 编译

命令(都是从android studio下载的)：

```
export ANDROID_NDK=/home/0668001490/Android/Sdk/ndk/29.0.13599879

# -DCMAKE_TOOLCHAIN_FILE  这个是使用ndk平台编译的意思，不写就用linux平台g++和依赖库编译了
# 写入配置文件
/home/0668001490/Android/Sdk/cmake/4.0.2/bin/cmake -DCMAKE_TOOLCHAIN_FILE=$ANDROID_NDK/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28 -DCMAKE_C_FLAGS="-march=armv8.7a"  -DCMAKE_CXX_FLAGS="-march=armv8.7a" -DGGML_OPENMP=OFF -DGGML_LLAMAFILE=OFF -DLLAMA_CURL=OFF -B build-android

# 开始编译  4线程执行
/home/0668001490/Android/Sdk/cmake/4.0.2/bin/cmake --build build-android --config Release -j 4

# 安装编译后内容
# 它的作用是将项目构建过程中生成的特定文件（例如可执行文件、库文件、头文件、文档等）从构建目录 (build-android) 复制到一个用户指定的安装目录 ({install-dir})
cmake --install build-android --prefix {install-dir} --config Release
```

### android JNI使用

编译成功后，现在拥有了可以直接在 Android JNI 项目中使用 .so 共享库文件，而不需要再像之前那样将 llama.cpp 的全部源码拷贝到你的项目中并尝试在 Android Studio 的构建系统（如 CMake 或 ndk-build）中重新编译


## 命令行/C代码 基本使用

1. 去hg下载几个gguf格式的量化模型

2. 加载：`llama-cli -m qwen2.5-1.5b-instruct-q2_k.gguf `

`llama-cli` 这个指令我查阅了源码，位置在tools/main/main.cpp，作者是将这个main文件编译成可执行文件放入build/bin下，基本所有命令都是这么来的，可以详细看看怎么编写的，算是一个展示怎么使用各种函数加载和模型推理的非常良好的示例。

### 基于C调用

llamacpp编译完成后会在build/bin目录下生成若干指令和lib的so动态链接库，完全可以直接调用动态链接库进行编程

### 项目结构

> include目录的头文件是直接从llama.cpp的include和src里面找到并拷贝下来的，lib内文件是本地编译的

```
project/
├── src/
│   └── main.cpp           // 使用llama来加载模型推理的代码
├── include/
│   ├── llama.h            // llama.cpp 的头文件,本来只要一个llama.h但是层层依赖就这么多头文件了
│   ├── common.h            
│   ├── ggml-backend.h            
│   ├── ggml-cpu.h            
│   ├── ggml-opt.h            
│   ├── ggml.h            
│   ├── ggml-alloc.h                      
│   └── llama-cpp.h            
├── lib/
│   ├── libllama.so        // llama.cpp 的动态库（Linux）都是从llama.cpp官方文档指导下在本地编译的so文件
│   ├── libllama.so        
│   ├── libllama.so        
│   ├── libllama.so        
    └── libggml-base.so
└── model/
    └── qwen.gguf          // 我下载的一个qwen2.5 1.5b q8_0的gguf本地模型
```

### 代码内容

> 官方也提供了示例，在llama.cpp内的example下

main.c
```c
#include "../include/llama.h"  // llama.cpp 的主头文件
#include <cstdio>             // C标准输入输出（printf等）
#include <cstring>            // C字符串操作（strcpy等）
#include <string>             // C++字符串
#include <vector>             // C++动态数组

int main(int argc, char ** argv) {
    // 模型路径（gguf格式模型）
    std::string model_path = "../model/qwen.gguf";

    // 输入的提示词（prompt）
    std::string prompt = "what is llama";

    // GPU 加速的层数（设置为99表示尽可能多的层都放到GPU）
    int ngl = 99;

    // 要生成的 token 数量
    int n_predict = 320;


    // 加载所有支持的后端（CPU、CUDA、Vulkan、Metal 等）
    ggml_backend_load_all();


    // ================== 第一步：加载模型 ==================

    // 初始化模型参数（默认参数）
    llama_model_params model_params = llama_model_default_params();
    model_params.n_gpu_layers = ngl;  // 设置 GPU 加速层数

    // 从文件加载模型
    llama_model * model = llama_model_load_from_file(model_path.c_str(), model_params);

    // 检查模型是否加载成功
    if (model == NULL) {
        fprintf(stderr , "%s: error: 无法加载模型\n" , __func__);
        return 1;
    }

    // 获取模型的词汇表（用于 token 和字符串之间的转换）
    const llama_vocab * vocab = llama_model_get_vocab(model);


    // ================== 第二步：Tokenize 提示词 ==================

    // 先计算 prompt 会被 tokenize 成多少个 token
    // llama_tokenize 返回负数表示需要的 buffer 大小
    const int n_prompt = -llama_tokenize(vocab, prompt.c_str(), prompt.size(), NULL, 0, true, true);

    // 分配空间并真正 tokenize 提示词
    std::vector<llama_token> prompt_tokens(n_prompt);
    if (llama_tokenize(vocab, prompt.c_str(), prompt.size(), prompt_tokens.data(), prompt_tokens.size(), true, true) < 0) {
        fprintf(stderr, "%s: error: tokenize 提示词失败\n", __func__);
        return 1;
    }

    // 打印原始 prompt 的每个 token（调试用）
    for (auto id : prompt_tokens) {
        char buf[128]; 
        int n = llama_token_to_piece(vocab, id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
            return 1;
        }
        std::string s(buf, n);
        printf("%s", s.c_str());
    }
    printf("\n");


    // ================== 第三步：初始化上下文 ==================

    // 初始化上下文参数
    llama_context_params ctx_params = llama_context_default_params();
    ctx_params.n_ctx = n_prompt + n_predict - 1;  // 设置上下文最大长度
    ctx_params.n_batch = n_prompt;                // 一个 batch 最多处理多少 token
    ctx_params.no_perf = false;                   // 开启性能统计

    // 使用模型初始化上下文
    llama_context * ctx = llama_init_from_model(model, ctx_params);

    // 检查上下文是否创建成功
    if (ctx == NULL) {
        fprintf(stderr , "%s: 创建llama_context失败 \n" , __func__);
        return 1;
    }


    // ================== 第四步：初始化采样器 ==================

    // 初始化采样器链（支持多个采样策略）
    auto sparams = llama_sampler_chain_default_params();
    sparams.no_perf = false;

    // 创建采样器链
    llama_sampler * smpl = llama_sampler_chain_init(sparams);

    // 添加一个贪婪采样器（总是选概率最大的 token）
    llama_sampler_chain_add(smpl, llama_sampler_init_greedy());


    // ================== 第五步：主推理循环 ==================

    // 记录开始时间，用于计算生成速度
    const auto t_main_start = ggml_time_us();

    // 实际生成的 token 数量
    int n_decode = 0;

    // 新生成的 token ID
    llama_token new_token_id;
    llama_batch batch = llama_batch_get_one(prompt_tokens.data(), prompt_tokens.size());

    // 主循环，直到生成完 n_predict 个 token 或遇到结束符
    for (int n_pos = 0; n_pos + batch.n_tokens < n_prompt + n_predict; ) {

        // 将这个 batch 输入模型进行推理（前向计算）
        if (llama_decode(ctx, batch)) {
            fprintf(stderr, "%s : failed to eval, return code %d\n", __func__, 1);
            return 1;
        }

        // 当前 batch 的 token 数量加到已处理位置
        n_pos += batch.n_tokens;


        // ================== 第六步：采样下一个 token ==================

        // 使用采样器从模型输出中采样下一个 token
        new_token_id = llama_sampler_sample(smpl, ctx, -1);

        // 检查是否生成结束（遇到 <EOS> 等结束符）
        if (llama_vocab_is_eog(vocab, new_token_id)) {
            break;
        }

        // 将新生成的 token 转换为字符串输出
        char buf [128];
        int n = llama_token_to_piece(vocab, new_token_id, buf, sizeof(buf), 0, true);
        if (n < 0) {
            fprintf(stderr, "%s: error: failed to convert token to piece\n", __func__);
            return 1;
        }
        std::string s(buf, n);
        printf("%s", s.c_str());
        fflush(stdout);  // 强制刷新输出缓冲区（让输出立刻显示）

        // 将新生成的 token 放入下一轮的 batch
        batch = llama_batch_get_one(&new_token_id, 1);
        n_decode += 1;
    }

    printf("\n");


    // ================== 第七步：性能统计 ==================

    const auto t_main_end = ggml_time_us();

    // 输出生成速度（token/s）
    fprintf(stderr, "%s: decoded %d tokens in %.2f s, speed: %.2f t/s\n",
            __func__, n_decode, (t_main_end - t_main_start) / 1000000.0f, n_decode / ((t_main_end - t_main_start) / 1000000.0f));

    // 输出采样器和上下文的性能统计信息
    fprintf(stderr, "\n");
    llama_perf_sampler_print(smpl);
    llama_perf_context_print(ctx);
    fprintf(stderr, "\n");


    // ================== 第八步：释放资源 ==================

    llama_sampler_free(smpl);       // 释放采样器
    llama_free(ctx);                // 释放上下文
    llama_model_free(model);        // 释放模型

    return 0;
}
```

### 编译并链接so库


#### 手动编译

1. 我在目录下的src目录进行的编译：`g++ -I../include -L../lib -Wl,-rpath=../lib -o llama_demo main.cpp -pthread -lm ../lib/*.so`

> 参数意义
+ g++	GNU C++ 编译器，用于编译和链接 C++ 程序
+ -I../include	添加头文件搜索路径，告诉编译器去 ../include 目录下找 .h 或 .hpp 文件
+ -L../lib	添加库文件搜索路径，告诉链接器去 ../lib 目录下找 .so（Linux）或 .a/.dll（Windows）库文件
+ -Wl,-rpath=../lib	传递链接器选项：告诉生成的可执行文件在运行时去 ../lib 路径下查找 .so 动态库文件，避免每次运行前手动设置 LD_LIBRARY_PATH
+ -o llama_demo	指定输出的可执行文件名为 llama_demo
+ main.cpp	要编译的源文件
+ -pthread	启用 POSIX 线程支持（多线程）
+ -lm	链接数学库（libm.so），提供 sin, cos, sqrt 等数学函数
+ ../lib/*.so	匹配并链接 ../lib 目录下所有 .so 文件（如 libggml.so, libllama.so）

2. 编译后执行就行：`./llama_demo`


#### make自动编译

编写CMakeLists.txt文件放在项目根目录

```shell
cmake_minimum_required(VERSION 3.14)
project(llama_demo)
# 设置 C++ 标准
# set(CMAKE_CXX_STANDARD 11)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 设置构建类型（Debug/Release）
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
# 添加头文件目录
include_directories(${PROJECT_SOURCE_DIR}/include)
# 设置 lib 目录路径
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib)
# 查找 lib 目录下的所有 .so 文件（Linux）
file(GLOB LIBS "${LIB_DIR}/*.so")
if(LIBS)
    message(STATUS "Found shared libraries: ${LIBS}")
else()
    message(FATAL_ERROR "No .so files found in ${LIB_DIR}, please make sure they exist.")
endif()
# 添加可执行文件
add_executable(${PROJECT_NAME} src/main.cpp)
# 链接所有 .so 文件
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        ${LIBS}
        pthread
        m
)
# 设置运行时库路径（RPATH），让程序运行时能找到 .so 文件
set(CMAKE_INSTALL_RPATH "${CMAKE_SOURCE_DIR}/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
```

1. mkdir build
2. cd build
3. cmake ..         // 生成makefile等配置文件
4. cmake --build .  // 生成可执行文件



## 部署单片机

llama.cpp 强大之处在于自己用C++完全实现了模型的整个架构和推理代码，非常高效，而且支持量化使得垃圾cpu都能跑推理，比如单片机这样的性能羸弱的平台

也可以部署在移动端APP上，通过APP的方式使用离线模型，这种方式就得语言切换了，毕竟不能直接用C++开发APP。比如安卓平台就得通过JNI调用C/C++的方式使用llamacpp。也有现成的框架，llama.rn。通过react native开发。

如果部署单片机就简单多了，单片机的大模型和人交互，可以采用多种方式，蓝牙，或者跑一个简单的http服务器。

采用http服务器最方便，llama.cpp/examples内有chat等Demo代码，稍微改一改增加一个http功能，运行在单片机上即可。

带来的好处一目了然：

+ 减少语言切换成本（不需要用 Python、Node.js）
+ 降低系统资源占用（适合嵌入式、单片机、小型设备）
+ 保持部署简单（不需要运行时、虚拟机、容器）

这里选择cpp-httplib，非常简单和轻量，访问官网进行下载配置，也可以使用llama-cpp-python，基于python实现服务器

项目架构类似上个Demo

```c
#include "httplib.h"
#include "llama.h"  // 集成 llama.cpp 的头文件
// 已经加载了模型
struct llama_model* model;
struct llama_context* ctx;

// 生成文本的函数（简化）
std::string generate_response(const std::string& prompt) {
    // 设置 prompt
    llama_token bos = llama_token_bos(model);
    llama_token eos = llama_token_eos(model);
    // 编码 prompt
    std::vector<llama_token> tokens = encode_prompt(prompt);
    // 清除上下文
    llama_reset_context(ctx, nullptr);
    // 生成文本
    std::string output;
    for (int i = 0; i < 100; ++i) {  // 最多生成 100 个 token
        llama_token token = llama_sample_top_p(ctx, tokens.data(), tokens.size(), 0.95);
        tokens.push_back(token);
        output += llama_token_to_str(model, token);
        if (token == eos) break;
    }
    return output;
}
int main() {
    httplib::Server svr;
    // POST /generate
    svr.Post("/generate", [](const httplib::Request& req, httplib::Response& res) {
        std::string prompt = req.get_param_value("prompt");
        std::string response = generate_response(prompt);
        res.set_content(response, "text/plain");
    });
    // GET /health
    svr.Get("/health", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("OK", "text/plain");
    });
    printf("Server started at http://0.0.0.0:8080\n");
    svr.listen("0.0.0.0", 8080);
    return 0;
}
```

使用，连接单片机热点 或者单片机cmd内`curl "http://localhost:8080/generate?prompt=Hello"`

更好的交互方式：语音+蓝牙交互，以后研究

## 开启多模态支持

### 模型支持

llama.cpp项目初期就支持了llava，(lava.cpp,cli.cpp)，编译的llava-cli就用来跑llava

随后增加了mobileVlm支持（轻量llava）

随后又支持了若干模型，模型越多，会话的chat模版越复杂，添加了若干 xxx-cli导致异常复杂

最后引入了libmtmd替换lava.cpp，提供统一的命令行接口处理多模态输入，mtmd-cli就可以加载若干不同模型

### mmproj

> 全称 Multi-model project 

llamacpp支持的多模态实现，首先需要一个embedding层对图片进行编码输出向量。这个embedding层是独立于llamacpp项目的，现在的许多视觉模型（比如clip）都是基于ViT的，他们的预处理（归一，卷积等）和投影（图像特征->模型语义空间）差别很大，将它们直接集成到 libllama 中目前还比较困难，所以独立了embedding模型

因此llamacpp的多模态需要两个模型（中期融合）

+ llm
+ 处理图像的模型（encoding和projection两个工作）

> 获取和使用

这块还是看官网，提供了多种多样的模型和地址

如果选择Gemma 3 vision就执行`llama-mtmd-cli -hf ggml-org/gemma-3-4b-it-GGUF`,注意1b版本不支持视觉，支持音频。4b及以上才支持视觉。

cli下载的默认地址是`~/.cache/llama.cpp/ggml-org_gemma-3-4b-it-GGUF_gemma-3-4b-it-Q4_K_M.gguf`

### 多模态使用

正常指定基础模型和ViT结构的mmproj模型，手动加载本地模型使用：`llama-mtmd-cli -m {llm基础模型比如qwen2.5-1.5b-q4_0}.gguf --mmproj {mmproj模型}.gguf --image 图片.jpg`

Qwen2.5-VL 在ggml官方的库里下载最好，官方已经做好了转换：`https://github.com/ggml-org/llama.cpp/blob/master/docs/multimodal.md`

注意：最好不要手动下载模型，而是 `llama-server -hf ggml/Qwen2.5... ` 会自动下载模型和mmproj，手动下载不要只下载模型，还要点开`files and versions`找到里面对应的mmproj模型下载。

只要有模型+mmproj就可以进行多模态任务，模型可以是原版的，也可是ggml库的，但如果模型本身不支持多模态，强行加载会出错


## llama.cpp 的底层实现原理

llama.cpp 不依赖 PyTorch 或 TensorFlow，而是自己实现了：
+ LLaMA 模型的结构（如 attention、feed-forward、RMSNorm 等）
+ 权重加载与解析（从 HuggingFace 或 Meta 的原始格式）
+ 自回归生成（逐 token 生成）

### 权重加载与内存管理

+ llama.cpp 将模型权重加载为 二进制格式（.bin）
+ 每个权重张量都被映射为一个结构体（ggml_tensor）
+ 使用自定义内存管理器（ggml）来处理 tensor 的分配和释放

### 自定义张量计算库：ggml
ggml 是 llama.cpp 的底层计算库（类似于 PyTorch 的 autograd）
特点：
+ 支持 CPU 指令集加速（如 AVX、AVX2、FMA、NEON）
+ 支持 GPU 加速（Metal、CUDA、Vulkan）
+ 支持自动微分（但主要用于推理）
+ 支持 量化计算（Q4_0、Q4_1、Q5_0、Q8_0 等）

###  量化

llama.cpp 最大的亮点之一是 支持模型量化，将浮点数（FP32）转换为低精度整数（如 4-bit、5-bit、8-bit）

量化后模型体积大幅减小，推理速度提升，内存占用降低。 例如：原始 LLaMA-7B 模型：约 13GB，量化为 Q4_0 后：约 3.5GB，甚至更低

### 推理引擎
llama.cpp 实现了一个轻量的推理引擎：

+ 支持自回归生成（逐 token 生成）
+ 支持上下文缓存（KV Cache）优化
+ 支持多种采样策略（如 greedy、top-k、top-p、temperature）
+ 所有推理逻辑都用 纯 C/C++ 实现，不依赖 Python


### 源码阅读例子

关键函数，计算矩阵相乘，可以看看llamacpp底层是怎么实现的，源码位置（ggml/src/ggml.c 2904行）：
```c
/**
 * ggml_mul_mat - 构造一个矩阵乘法操作的张量节点
 *
 * 该函数不会立即执行矩阵乘法运算，而是创建一个张量节点，
 * 表示两个输入张量 a 和 b 的矩阵乘法操作。
 * 真正的计算会在 ggml_graph_compute 阶段执行。
 *
 * 参数：
 *   ctx - ggml 上下文，用于内存管理
 *   a   - 第一个输入张量，形状应为 [K, M]
 *   b   - 第二个输入张量，形状应为 [K, N]
 *
 * 返回：
 *   一个新的张量，表示 a × b 的结果，形状为 [M, N]
 */
struct ggml_tensor * ggml_mul_mat(
        struct ggml_context * ctx,
        struct ggml_tensor  * a,
        struct ggml_tensor  * b) {

    // 确保两个张量可以进行矩阵乘法：
    // a 的列数必须等于 b 的列数（即 a->ne == K，b->ne == K）
    GGML_ASSERT(ggml_can_mul_mat(a, b));

    // ggml 不支持对 a 进行转置，所以必须确保 a 没有被转置
    GGML_ASSERT(!ggml_is_transposed(a));

    // 构造输出张量的维度：
    // a->ne 是 a 的列数 M
    // b->ne 是 b 的列数 N
    // 输出张量的维度为 [M, N, 1, 1]，即形状为 M x N
    const int64_t ne = { a->ne b->ne b->ne b->ne };

    // 创建一个新的张量，类型为 float32（F32），维度为 4（虽然只用前两个）
    struct ggml_tensor * result = ggml_new_tensor(ctx, GGML_TYPE_F32, 4, ne);

    // 设置该张量的操作类型为矩阵乘法（GGML_OP_MUL_MAT）
    result->op = GGML_OP_MUL_MAT;

    // 设置该张量的两个输入源张量
    result->src = a; // 第一个输入是 a
    result->src = b; // 第二个输入是 b

    // 返回构造好的张量节点
    return result;
}
```


