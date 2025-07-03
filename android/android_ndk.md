# NDK

## 准备

1. android studio中sdk tools 勾选NDK和CMAKE

2. 创建native c++项目，可以从头创建项目，也可以手动修改原有项目支持C++

## 创建

从头创建比较简单，官方会直接提供示例，如果需要手动添加C++支持，需要手动进行修改

1. android studio中左侧栏目展示改为project形式
2. app/src/main 目录右键，添加C++模块，会自动生成一个cpp，CMakeLists，includes文件，cpp文件内容是空白，可以编写一些c++规范代码，示例
```c
#include <jni.h>
#include <string>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_woof_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    return env->NewStringUTF(hello.c_str());
}
```

支持C++项目的数据结构中会新增一些东西

+ cpp目录，目录下有includes文件夹（不重要），CMakeLists.txt，cpp文件
+ build.gradle.kts内会新增一些cmake的配置

```
externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
```


## 使用

添加完支持后，cpp内可以编写各种方法，不过函数命名格式严格遵守规则如`Java_com_example_woof_MainActivity_stringFromJNI` 否则kotlin中无法找到目标函数

1. cpp编写函数

2. MainActivity.kt文件中要初始化和定义外部函数

```kotlin
    external fun stringFromJNI(): String
    companion object {
        init {
            System.loadLibrary("woof")
        }
    }
```

3. 函数使用，初始化完成后可以自由使用外部库函数，比如可以作为参数传递给其他组件

```kotlin
Surface(
    modifier = Modifier.fillMaxSize(),
) {
    GameScreen(GameViewModel(),::stringFromJNI)  // ::是将函数展为lambda格式
}

// 组件函数定义
@Composable
fun GameScreen(gameViewModel: GameViewModel, stringFromJNI: ()-> String) 
{.....}
```