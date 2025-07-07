
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

2. 安装依赖：
```
npm install llama.rn 
npm install react-native-fs  # 文件系统支持
npm install react-native-safe-area-context # 可选
npm install @react-navigation/native-stack   # 导航栏可选
npm install @react-navigation/elements      # 可选
```

3. 模型位置：`/home/0668001490/work/AwesomeProject/android/app/src/main/assets/model/qwen2.5-1.5b-instruct-q8_0.gguf`

  需要注意的是，模型比较大可能会提示heapsize空间不足的报错：在android目录下的gradle.properties中调整大小`org.gradle.jvmargs=-Xmx4G -XX:MaxMetaspaceSize=3G`

4. 添加多模态支持，增加上传图片组件：`npx expo install expo-image-picker`

#### 代码

```js
import React, { useState, useEffect } from 'react'; // 导入 useState 和 useEffect
import type {PropsWithChildren} from 'react';
import {
  SafeAreaView,
  ScrollView,
  StatusBar,
  StyleSheet,
  Text,
  TextInput,
  TouchableOpacity,
  useColorScheme,
  View,
  Image,
} from 'react-native';

import {
  Colors,
  DebugInstructions,
  Header,
  LearnMoreLinks,
  ReloadInstructions,
} from 'react-native/Libraries/NewAppScreen';

// 我的修改
import { loadLlamaModelInfo, initLlama, type LlamaContext } from 'llama.rn'; // 假设 LlamaContext 是 initLlama 返回的类型
import RNFS from 'react-native-fs';
import * as ImagePicker from 'expo-image-picker';


type SectionProps = PropsWithChildren<{ title: string }>; // 修正 SectionProps

function Section({children, title}: SectionProps): JSX.Element {
  const isDarkMode = useColorScheme() === 'dark';
  return (
    <View style={styles.sectionContainer}>
      <Text
        style={[
          styles.sectionTitle,
          {
            color: isDarkMode ? Colors.white : Colors.black,
          },
        ]}>
        {title}
      </Text>
      {/* 直接渲染 children，不再用 Text 组件包裹 */}
      {children} 
    </View>
  );
}

function App(): JSX.Element {
  const androidAssetSourcePath = 'model/qwen2.5-1.5b-instruct-q8_0.gguf'
  const destPath = `${RNFS.DocumentDirectoryPath}/qwen.gguf`;

  const stopWords = ['</s>', '<|end|>', '<|eot_id|>', '<|end_of_text|>', '<|im_end|>', '<|EOT|>', '<|END_OF_TURN_TOKEN|>', '<|end_of_turn|>', '<|endoftext|>']

  // 使用 useState 来存储 context，初始值为 null 或 undefined
  const [llamaContext, setLlamaContext] = useState<LlamaContext | null>(null); // 明确 context 的类型
  const [isLoading, setIsLoading] = useState(true); // 添加加载状态
  const [error, setError] = useState<string | null>(null); // 添加错误状态

  const [userInput, setUserInput] = useState(''); // 用户输入的内容
  const [chatOutput, setChatOutput] = useState(''); // 模型输出的内容 (可以是一个字符串或对象数组)
  const [isCompleting, setIsCompleting] = useState(false); // 模型是否正在生成回答
  const [selectedImage, setSelectedImage] = useState<string | null>(null); // 存储图片路径

  useEffect(() => {
    const initializeLlama = async () => {
      try {
        setIsLoading(true);
        setError(null);
        const exists = await RNFS.exists(destPath);
        if (exists) {
          console.log('模型文件已存在于目标路径:', destPath);
        } else {
          console.log('模型文件在目标路径不存在，尝试从 Android assets 复制:', destPath);
          // Android: 使用 copyFileAssets
          await RNFS.copyFileAssets(androidAssetSourcePath, destPath);
          console.log(`模型已从 Android assets ('${androidAssetSourcePath}') 复制到: ${destPath}`);
        }

        console.log('尝试加载模型信息 from:', destPath);
        const modelInfo = await loadLlamaModelInfo(destPath);
        console.log('Model Info:', modelInfo);

        if (!modelInfo) { // 或者根据你的库的具体行为判断
            setError('模型信息加载失败，无法初始化 Llama。');
            setIsLoading(false);
            return;
        }

        console.log('尝试初始化 Llama context...');
        const context = await initLlama({
          model: destPath, // 使用复制后的目标路径
          use_mlock: false, // **建议先设为 false 进行测试**
          n_ctx: 2048,
          n_gpu_layers: 0, // **在 Android 上通常设为 0**
        });
        console.log('Llama context 初始化成功:', context);
        setLlamaContext(context);
      } catch (e: any) {
        console.error('初始化 Llama 失败 - 错误消息:', e.message);
        if (e.stack) {
            console.error('初始化 Llama 失败 - 错误堆栈:', e.stack);
        }
        // 如果错误对象本身包含更多有用信息，可以考虑打印整个对象
        // console.error('初始化 Llama 失败 - 完整错误对象:', e);
        setError('初始化 Llama 失败: ' + e.message);
      } finally {
        setIsLoading(false);
      }
    };

    initializeLlama();

    // 清理函数（可选，如果 initLlama 返回了需要清理的资源）
    return () => {
      // if (llamaContext) {
      //   // llamaContext.release(); // 假设有释放资源的方法
      // }
    };
  }, [androidAssetSourcePath]); // modelPath 作为依赖项，如果它可能改变则需要重组


  const handleSendMessage = async () => {
    if (!llamaContext || !userInput.trim()) {
      // 如果 context 未初始化或用户输入为空，则不执行
      return;
    }

    setIsCompleting(true);
    setChatOutput(''); // 清空之前的输出或准备接收新输出
    const currentInput = userInput; // 保存当前输入，因为 userInput state 可能会在异步操作中改变
    setUserInput(''); // 清空输入框

    try {
      // 你可以选择使用 "chat" 模式或 "text completion" 模式
      // 这里以 "text completion" 模式为例，因为它更简单直接
      // 对于聊天，你可能需要维护一个消息列表传递给 'messages' 参数

      const promptText = `User: ${currentInput}\nAssistant:`; // 构建一个简单的提示

      console.log('Sending prompt to Llama:', promptText);

      const result = await llamaContext.completion(
        {
          prompt: promptText,
          n_predict: 100, // 预测的 token 数量
          stop: [...stopWords, 'User:', 'Assistant:'], // 添加更多停止词
          // temperature: 0.7, // 可以调整的其他参数
        },
        (data) => {
          // 流式回调：每当模型生成一个新的 token 时调用
          const { token } = data;
          console.log('Received token:', token);
          setChatOutput((prevOutput) => prevOutput + token); // 将新的 token 追加到输出
        }
      );

      console.log('Completion finished. Full result object:', result);
      // result.text 包含了完整的补全文本 (在流式回调结束后)
      // 如果你已经在流式回调中构建了完整的 chatOutput，这里可能不需要再用 result.text 更新
      // 但如果流式回调有问题，或者你想确保最终结果，可以使用它
      // setChatOutput(result.text); // 或者，如果你想确保最终输出是完整的

      console.log('Final Text:', result.text);
      console.log('Timings:', result.timings);

    } catch (e: any) {
      console.error('Llama completion error:', e.message, e);
      setChatOutput((prev) => prev + `\n\nError: ${e.message}`); // 在输出中显示错误
    } finally {
      setIsCompleting(false);
    }
  };

  // 处理图片
  const handlePickImage = async () => {
    // 请求权限
    const { status } = await ImagePicker.requestMediaLibraryPermissionsAsync();
    if (status !== 'granted') {
      console.log('需要访问你的相册来上传图片');
      return;
    }
  
    // 打开图片选择器
    const result = await ImagePicker.launchImageLibraryAsync({
      mediaTypes: ImagePicker.MediaTypeOptions.Images,
      allowsEditing: true,
      aspect:[4,3],
      quality: 1,
    });
  
    if (!result.canceled) {
      const uri = result.assets[0].uri;
      console.log('Selected image URI:', uri);
      setSelectedImage(uri); // 保存图片路径
    }
  };

  const isDarkMode = useColorScheme() === 'dark';
  const backgroundStyle = {
    backgroundColor: isDarkMode ? Colors.darker : Colors.lighter,
  };

  if (isLoading) {
    return (
      <SafeAreaView style={backgroundStyle}>
        <View style={{flex: 1, justifyContent: 'center', alignItems: 'center'}}>
          <Text style={{color: isDarkMode ? Colors.white : Colors.black}}>
            正在加载模型和初始化 Llama...
          </Text>
        </View>
      </SafeAreaView>
    );
  }

  if (error) {
    return (
      <SafeAreaView style={backgroundStyle}>
        <View style={{flex: 1, justifyContent: 'center', alignItems: 'center'}}>
          <Text style={{color: 'red'}}>错误: {error}</Text>
        </View>
      </SafeAreaView>
    );
  }

  // 只有当 llamaContext 成功初始化后才渲染主要内容
  if (!llamaContext) {
    return (
      <SafeAreaView style={backgroundStyle}>
        <View style={{flex: 1, justifyContent: 'center', alignItems: 'center'}}>
          <Text style={{color: isDarkMode ? Colors.white : Colors.black}}>
            Llama Context 未能初始化。
          </Text>
        </View>
      </SafeAreaView>
    );
  }

  return (
    <SafeAreaView style={backgroundStyle}>
      <StatusBar
        barStyle={isDarkMode ? 'light-content' : 'dark-content'}
        backgroundColor={backgroundStyle.backgroundColor}
      />
      <ScrollView
        contentInsetAdjustmentBehavior="automatic"
        style={backgroundStyle}>
        <Header />
        <View
          style={{
            backgroundColor: isDarkMode ? Colors.black : Colors.white,
            padding:16,
          }}>
          <Section title="Llama Interaction">
            {/* 用户文本输入 */}
            <TextInput
             style={[
              styles.input,
              { color: isDarkMode ? Colors.white : Colors.black, borderColor: isDarkMode ? Colors.light : Colors.dark }
            ]}
            placeholder="Ask Llama..."
            placeholderTextColor={isDarkMode ? Colors.light : Colors.dark}
            value={userInput}
            onChangeText={setUserInput}
            editable={!isCompleting}
            />

          <TouchableOpacity
            style={styles.button}
            onPress={handlePickImage}
            disabled={isCompleting}
          >{/* 图片处理 */}
            <Text style={styles.buttonText}>Upload Image</Text>
          </TouchableOpacity>

            <TouchableOpacity
              style={[styles.button, isCompleting ? styles.buttonDisabled : {}]}            
              onPress={handleSendMessage}
              disabled={isCompleting || !llamaContext}
              > 
              {/* 点击效果 */}
              <Text style={styles.buttonText}>
                {isCompleting?'Generating...':'Send'}
              </Text>
            </TouchableOpacity>

             {/* 显示模型输出 */}

             {chatOutput?(
                <View style={styles.outputContainer}>
                  <Text style={[styles.outputTitle, { color: isDarkMode ? Colors.white : Colors.black }]}>
                  Assistant:
                  </Text>
                  <Text style={[styles.outputText, { color: isDarkMode ? Colors.white : Colors.black }]}>
                    {chatOutput}
                  </Text>
                </View>
             ):null}

            {selectedImage && (
              <View style={styles.imageContainer}>
                <Text style={{ color: isDarkMode ? Colors.white : Colors.black }}>
                  Selected Image:
                </Text>
                <Image source={{ uri: selectedImage }} style={styles.imagePreview} />
              </View>
            )}
          </Section>
        </View>
      </ScrollView>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  // ... (你的样式保持不变)
  imageContainer: {
    marginTop: 20,
  },
  imagePreview: {
    width: 200,
    height: 200,
    marginTop: 10,
    borderRadius: 10,
  },
  sectionContainer: {
    marginTop: 32,
    paddingHorizontal: 24,
  },
  sectionTitle: {
    fontSize: 24,
    fontWeight: '600',
  },
  sectionDescription: {
    marginTop: 8,
    fontSize: 18,
    fontWeight: '400',
  },
  highlight: {
    fontWeight: '700',
  },
  input: {
    borderWidth: 1,
    padding: 10,
    marginBottom: 10,
    borderRadius: 5,
    fontSize: 16,
  },
  button: {
    backgroundColor: '#007AFF', // 一个示例颜色
    padding: 12,
    borderRadius: 5,
    alignItems: 'center',
    marginBottom: 20,
  },
  buttonDisabled: {
    backgroundColor: '#cccccc',
  },
  buttonText: {
    color: 'white',
    fontSize: 16,
    fontWeight: 'bold',
  },
  outputContainer: {
    marginTop: 20,
    padding: 10,
    backgroundColor: '#f0f0f0', // 示例背景色
    borderRadius: 5,
  },
  outputTitle: {
    fontSize: 18,
    fontWeight: 'bold',
    marginBottom: 5,
  },
  outputText: {
    fontSize: 16,
  },
});

export default App;
```

