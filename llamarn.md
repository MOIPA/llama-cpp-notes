
# llama.rn

### 源码原理

基于llamacpp的react native项目，查看官方库的代码可以看到是基于这个步骤完成的

1. 将 llama.cpp 编译为 Android 可用的 .so 库，Android NDK（Native Development Kit）将 llama.cpp 的 C/C++ 代码交叉编译为 Android 支持的架构（如 armeabi-v7a, arm64-v8a, x86_64 等）。生成 .so（共享库）文件，供 Android 平台调用

2. 将 .so 库打包进 React Native 模块

3. 通过 JNI 暴露接口给 Java / React Native

4. CMakeLists我看了项目的配置，jni.cpp自己写的暴露给java的c++代码是和llamacpp的代码一起编译的

5. 编译完成后Java内编写适配reactNative的封装调用c++代码的部分。这部分我看了只是重新用java定义了一下函数，比如以下的java代码，具体实现都在C++中，用native关键字申明，不用提供实现。

```java
public class LlamaContext {
    //  .... 其他代码
    // WritableMap 是 React Native 的数据结构，用于在 Java 和 JS 之间传递数据（类似 Map）
    protected static native WritableMap modelInfo(
    String model,
    String[] skip
  
    //  .... 其他代码
  );
}
```

6. JS 层是怎么调用到 Java 层的这段 native 代码的？原理是什么？

```js
import { NativeModules } from 'react-native';
const { LlamaModule } = NativeModules;

const info = await LlamaModule.modelInfo('ggml-model.bin', ['some-key']);
```
LlamaModule 是在 Java 中定义的一个 Native Module，它是一个 Java 类，继承自 ReactContextBaseJavaModule，并实现了 getName() 和导出方法

> React Native Bridge 的工作原理

React Native 通过一个叫 Bridge（桥接） 的机制，在 JS 和 Java 之间通信,JS 层和 Java 层运行在不同的线程（JS 运行在 JS 线程，Java 运行在主线程或原生模块线程）

所有 JS 调用 Native Module 的方法，都会通过 Bridge 序列化成消息，发给 Java
Java 层处理完之后，再通过 Bridge 把结果返回给 JS

JS 代码运行在 JS 引擎中（比如 Hermes、JavaScriptCore），Java 代码运行在 JVM 中（Android 上的 ART 虚拟机），React Native 通过 Bridge 机制让 JS 引擎线程向 JVM 发送序列化消息，从而调用 Java 函数。

Bridge 本质上是两个线程之间的 消息队列 + 序列化/反序列化机制。

### llama.rn应用

1. 初始化一个react native项目，比如官方示例的 AwesomeProject



