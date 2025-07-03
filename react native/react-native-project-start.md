# react native 项目启动

## 安装配置

https://reactnative.cn/docs/environment-setup

1. 安装nodejs，查看官方：https://nodejs.org/en/download
```shell
curl -o- https://raw.githubusercontent.com/nvm-sh/nvm/v0.40.3/install.sh | bash
\. "$HOME/.nvm/nvm.sh"
nvm install 22
node -v # Should print "v22.17.0".
nvm current # Should print "v22.17.0".
corepack enable yarn
yarn -v
```
可以通过`nvm use 22`来快速切换版本

2. Android studio安装和配置，注意，如果是linux上安装，务必安装到/opt目录下，否则检测不到，推测是他的检测脚本不是全局扫盘的

3. 创建新项目：`npx @react-native-community/cli init AwesomeProject`

## 启动新项目

1. `cd AwesomeProject`,`yarn android`或者`yarn react-native run-android`

2. 第二种方式，这种可以看到详细的编译过程

终端1：`npx react-native start`

终端2：`npx react-native run-android`

## 打包APK

1. 生成和配置签名密钥，可选

2. `npx react-native run-android --variant=release` 或者 `cd android & ./gradlew assembleRelease`

## Metro意义

1. 模块打包（Module Bundling） 将所有模块打包成一个 JS 文件!

React Native 使用 CommonJS 或 ES Modules 的方式组织代码，而原生的 JavaScript 引擎（如 JavaScriptCore）不支持这些模块系统。
Metro 的作用 是将多个 JS 文件打包成一个或多个 bundle 文件，让 JS 引擎可以加载和运行。

2. 支持热重载（Hot Reloading）

Metro 支持热重载（Hot Reloading）和实时重载（Live Reloading），这是 React Native 开发体验的核心部分。
它能监听文件变化，重新打包并推送到正在运行的 App 中，无需重新启动。

3. 支持多平台打包

React Native 的目标是跨平台开发，Metro 可以为不同平台（如 Android、iOS）生成对应的 bundle 文件。

4. Transform（转换）JSX、ES6+、Flow 等语法

Metro 内部集成 Babel，可以将 JSX、ES6+、Flow 等语法转换为 JavaScript 引擎可以理解的代码。

5. 开发服务器（Dev Server）

Metro 提供了一个开发服务器（通常是 localhost:8081），在开发模式下，App 会从这个服务器动态加载 JS bundle，而不是打包进 APK/IPA。

6. `.bundle` 文件是一个打包后的 JS 文件，只包含 JS 模块代码，不包含图片等资源。图片等资源会被 Metro 拷贝到原生资源目录中，并通过资源 ID 被引用。
