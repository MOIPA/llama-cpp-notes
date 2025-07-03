
## pocketpal-ai

一个移动端的llamacpp项目，基于llamacpp和llamcrn

### 架构

这个项目的架构主要是基于 React Native 的。

意味着：

+ 核心应用逻辑和UI在JavaScript中：大部分应用界面 (UI) 和业务逻辑使用 JavaScript 和 React 来编写。

+ 原生Android“外壳”：MainActivity.kt 文件是原生 Android 应用的一部分，它作为容器来承载和启动 React Native 应用。

+ 原生模块和组件（可能存在）：

+ 原生模块 (Native Modules)：如果应用需要访问更底层的设备功能（比如特定的传感器、蓝牙功能等JavaScript直接无法访问的API），可能会有 Java 或 Kotlin 编写的原生模块来桥接这些功能给 JavaScript 使用。

+ 原生UI组件 (Native UI Components)：对于一些性能要求极高或者 React Native 没有提供的特殊UI视图，也可能会用原生代码（Java/Kotlin）实现，并嵌入到 React Native 的布局中。具体来看 MainActivity.kt 这个文件：

+ 它继承自 ReactActivity，这是 React Native 为 Android 平台提供的 Activity 基类。

+ getMainComponentName(): 这个方法告诉 React Native 应用启动时应该加载哪个 JavaScript 组件作为根组件。在这个项目中，根组件的名称是 "PocketPal"。

+ createReactActivityDelegate(): 这个方法用于创建和配置 ReactActivityDelegate，它负责管理 React Native 实例的生命周期等，这里使用了 DefaultReactActivityDelegate，并且启用了对新架构 (Fabric) 的支持。

+ onCreate(): 这是 Android Activity 的一个标准生命周期方法。

+ enableEdgeToEdge(): 这是调用 AndroidX 库中的一个函数，用于让应用的内容可以绘制到系统状态栏和导航栏的后面，实现沉浸式的边到边界面效果。

+ super.onCreate(null): 这是一个关键点。正如注释中所解释的，这里传递 null 是为了解决 react-native-screens 库在恢复 Fragment 时可能导致应用崩溃的问题。这是一个已知的解决方案。

#### 架构的关键点总结：

1. JavaScript 为主导：UI 和大部分应用逻辑将存在于 JavaScript 文件中（通常是 .js 或 .jsx/.tsx 文件）。您会使用 React 组件来构建用户界面。
2. 原生桥接 (Native Bridge)：React Native 通过一个“桥”来实现 JavaScript 代码和原生 Android 平台之间的通信。这使得 JavaScript 可以调用原生 API，反之亦然。新的架构（如 Fabric 和 TurboModules）致力于提升这种通信的效率。
3. 原生Android项目结构依然存在：在 React Native 项目中，通常会有一个 android 文件夹，这就是一个完整的原生 Android 项目。您会在这里管理原生依赖（比如您依赖的 androidx.activity:activity:1.9.0）、配置原生设置（如 AndroidManifest.xml 中的权限声明）等。4.入口点：在 Android 平台上，MainActivity.kt（以及在 iOS 平台上的 AppDelegate.m 或 .swift）是原生的启动点，它负责加载和运行您的 React Native JavaScript 包。为了更深入地了解这个项目，您应该关注以下部分：

+ JavaScript/TypeScript 文件：通常在项目的根目录或者一个名为 src 的文件夹下。寻找注册了 "PocketPal" 组件的文件（例如 App.js, index.js）。

+ package.json 文件：这个文件列出了项目所有的 JavaScript 依赖库（包括 React, React Native 以及其他第三方库）。
+ android/ 文件夹：包含了原生的 Android 项目。
+ android/app/build.gradle：Android 特定的构建配置和原生依赖。
+ android/app/src/main/AndroidManifest.xml：应用的权限声明、Activity 注册等。
+ android/app/src/main/java/...：您的原生 Java/Kotlin 代码存放位置，比如 MainActivity.kt。
+ ios/ 文件夹（如果这是一个跨平台应用）：包含原生的 iOS 项目。总的来说，这个 MainActivity.kt 是一个标准的 React Native 应用的 Android 入口配置，其中包含了一个针对 react-native-screens 库的特定修复，以及使用了现代的 enableEdgeToEdge() API 来改善UI显示效果。