
# pocketpal-ai

一个移动端的llamacpp项目，基于llamacpp和llamcrn

## 架构概览

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



## 代码阅读

### 数据库组件

本项目使用的是WatermelonDB 模型，WatermelonDB 是一个为 React 和 React Native 应用设计的、快速、响应式、离线的本地数据库

#### ChatSession

提供会话的历史记录存储

+ 表名：chat_sessions
+ 表间关联关系：一个 ChatSession 可以拥有多个 Message
+ 在 Message 模型对应的表中，用来关联回 ChatSession 的外键列的名称是 session_id
+ completion_settings表同上

#### CompletionSetting

管理对应会话的模型推理设置，比如topk，温度，token限制等等，在该数据库组件中还提供了设置字段的版本控制

+ 表名：completion_settings
+ 表内有settings字段，JSON方式存储，存取时定义了辅助函数自动解析JSON
+ 还需要注意的settings字段内json存在Version字段用于判断设置的版本是否需要更新，随着这个项目更新，会有更多settings的相关字段添加删除或修改（utils中提供了比较版本后自动迁移到新版本设置的功能）

#### GlobalSetting

全局设置表：global_settings，value字段存储JSON格式设置并和CompletionSetting一样提供配置自动更新最新版本

#### Message

+ 表名：messages
+ 存储session_id,作者，内容，元数据，位置（第一条，第二条...），类型（text：文本可能带图片，其他类型）
+ 元数据字段meta_data也是个JSON

#### DAO层

ChatSessionRepository，控制以上的所有数据模型完成数据存取业务

注意：这里的代码有个迁移业务，这是因为这个项目以前是本地JSON文件存储数据，现在是WatermelonDB，要迁移数据到数据库中

+ 获取所有session函数
+ 通过sessionId获取session函数
+ 创建、删除session
+ 通过sessionID更新对应session的Message/settings
+ Message/全局设置的CURD

### 业务组件

#### 0.基础类型定义

不同包下有不同的types.ts，本质都是在定义基础类型

类似java的class，基础类型class定义，js用type/interface而不是class

1. 定义多种模型配置数据对象，BaseFormData：包含地址，用户角色，提示词等信息，AssistantFormData，RoleplayFormData，VideoPalFormData都是基于Base添加一些属性

2. 消息类型MessageType，内部包含多种type（图片，文本，文件，自定义等），各种type主要都是包含用户，创建时间，id，状态等字段

3. 定义用户User，包含名字，id，角色，图片等

4. 定义ChatMessage，角色+内容

5. 定义模型Model包含id，作者，url，下载速度等信息

6. 定义hg模型，gguf格式，benchmark类型，设备类型

#### 1.utils包

##### chat组件

0. 定义assistant类和user类，包含id名字等基础信息

1. convertToChatMessages，任意消息类型转为ChatMessage类型函数，且消息如果是多模态还要进一步转换，整理和统一格式的函数

2. applyChatTemplate函数，根据模型添加提示词来格式化消息，返回llama.rn消息格式JinjaFormattedChatResult


#### 2.PalStore

维护和管理所有Pal

1. 定义了Pal类型，需要id且满足AssistantFormData | RoleplayFormData | VideoPalFormData 三者之一的属性。

2. Pal类型基础上定义了三个类型Pal，在Pal基础上增加了PayType字段，类型是个Enum

3. 定义PalStore类：维护一个pal数组+对其CRUD函数，并增加可观察和可持久化，最后exportPalStore对象

4. export初始化函数initializeLookiePal，寻找默认模型，若不存在去defaultModels（维护Model数组）里找。默认配置一个VideoPalFormData类型的Pal添加到PalStore中

#### 3.ChatSessionStore

管理会话

1. 定义SessionMetaData（id，标题，日期，消息Message），SessionGroup（key：SessionMetaData数组）

2. 默认的group名称，今天，昨天，上周等定义

3. ChatSessionStore类，会话数组，当前激活session的id，定义一些加载session函数，加载全局配置函数，删除重置更新session函数等

4. 定义了若干对Session和Message的操作，重置当前激活session，添加Message到某个session，CRUD，修改title等

### UI组件

#### 1.SidebarContent侧边栏

这个组件负责显示抽屉菜单的内容，包括会话列表、可能的上下文菜单操作（如重命名、删除会话），并且能够响应主题和语言的变化

+ 提供的视图组件可以路由到chat，models，pals，benchmark组件
+ 读取所有按日期分组的session，遍历打印日期label，日期内遍历所有session，对当前激活的session id进行背景灰色特效
+ 所有session提供长按和点击事件，普通点击可以跳转到chat页面

#### 2.Bubble包裹器

Bubble组件是会话包裹文字的容器，并在文字下方添加了复制按钮，token生成速度等信息

一条消息的到来，会传递给Bubble消息本体和child子组件，Bubble在内部解析需要的信息后再渲染child子组件

总结 Bubble 组件的功能：

1. 消息包裹器: 作为聊天消息（文本、图片等）的通用外层容器。
2. 动态样式: 根据消息是否由当前用户发送、主题等因素应用不同的样式。
3. 动画支持: 通过 Animated.View 和 scale prop，可以实现消息气泡的缩放动画。
4. 元数据显示:•如果消息元数据中包含 timings 信息，则显示格式化和本地化的时间相关字符串。•如果消息元数据中指示消息 copyable 并且消息类型为文本，则显示一个复制按钮。
5. 复制功能: 允许用户点击复制按钮将文本消息内容复制到剪贴板，并提供触觉反馈。
6. 可定制性: 通过 props 如 child 和 scale，允许父组件控制其内容和动画。
7. 上下文感知: 使用 useContext 来获取主题、当前用户信息和本地化资源。

#### 3.ChatInput 输入组件

这个组件是会话界面的底部输入框的元素组合。包括上传图片，发送按钮，编辑模式和非编辑模式的动画，选择pal等。

需要传入的函数和参数比较多

1. 模型生成中的中断按钮触发的中断函数onStopPress
2. 使用pal进行视频会话onStartCamera
3. 发送文字到模型，onSendPress

#### 4.Menu组件

重写了react的组件

+ 增强 react-native-paper 的 Menu: 通过添加自定义主题、样式、子菜单状态管理和对状态栏的更好处理。
+ 提供更一致的 API: 通过复合组件模式，为菜单项和分隔符提供统一的访问方式。
+ 实现更高级的交互: 例如，根据是否有子菜单打开来改变主菜单的样式。
+ 提升开发者体验: 封装了一些通用逻辑，使得在应用中使用菜单更加方便和一致。

#### 5.ChatPalModelPickerSheet

选择模型和pal的组件，渲染models和Pals两个tab，每个tab都有自己的list，选择模型的一系列事件和警告等

#### 6.ChatHeader

Chat页面的左上角菜单栏，点击可以呼出抽屉导航器

#### 7.ChatEmptyPlaceholder

没有选择任何模型时候的默认页面

#### 8.VideoPalEmptyPlaceholder

同上没有选择pal时候的页面

#### 9.ChatView

本组件是整个chat页面的所有内容，会用到1-8的所有组件


## 仿写

1. 初始化项目

2. 初始化数据库模块

```
yarn add @nozbe/watermelondb
yarn add @nozbe/with-observables @nozbe/graphql-adapter
```