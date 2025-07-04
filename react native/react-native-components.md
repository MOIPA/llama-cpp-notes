# react native 核心组件

Android中是用kotlin和jetpack compose编写ui，rn提供了一套自己的UI组件

## UI组件

### 概览

1. `<View>`     一个支持使用flexbox布局、样式、一些触摸处理和无障碍性控件的容器
2. `<Text>`     显示、样式和嵌套文本字符串，甚至处理触摸事件
3. `<Image>`	显示不同类型的图片
4. `<ScrollView>`	一个通用的滚动容器，可以包含多个组件和视图
5. `<TextInput>	`	使用户可以输入文本

### 文本输入

`<TextInput>`组件

```js
import React,{useState} from 'react'
import {Text,TextInput,View} from 'react-native'

const PizzaTranslator = () => {
  const [text, setText] = useState('');
  return (
    <View style={{padding: 10}}>
      <TextInput
        style={{height: 40}}
        placeholder="Type here to translate!"
        onChangeText={text => setText(text)}
        defaultValue={text}
      />
      <Text style={{padding: 10, fontSize: 42}}>
        {text.split(' ').map((word) => word && '🍕').join(' ')}
      </Text>
    </View>
  );
}

export default PizzaTranslator;
```

### 触摸事件

简单事件示例：

```js
<Button
  onPress={() => {
    Alert.alert('你点击了按钮！');
  }}
  title="点我！"
/>
```

一个完整demo，定义了一个类组件，React 会实例化这个类来渲染 UI
```js
import React, { Component } from 'react';
import { Alert, Button, StyleSheet, View } from 'react-native';


// 通过 extends Component，你让 ButtonBasics 成为一个真正的 React 组件,这样它就可以使用 React 的生命周期方法（如 componentDidMount）、状态管理、以及必须的 render() 方法
export default class ButtonBasics extends Component {

//     定义的一个函数；
// 用于处理按钮点击事件；
// 当用户点击按钮时，会触发这个函数
  _onPressButton() {   // 下划线表示 私有方法，不强制这么写
    Alert.alert('You tapped the button!')
  }
// render() 是 React 类组件的必须方法 React 会调用 render() 来渲染 UI
  render() {
    return (
      <View style={styles.container}>
        <View style={styles.buttonContainer}>
          <Button
            onPress={this._onPressButton}
            title="Press Me"
          />
        </View>
        <View style={styles.buttonContainer}>
          <Button
            onPress={this._onPressButton}
            title="Press Me"
            color="#841584"
          />
        </View>
        <View style={styles.alternativeLayoutButtonContainer}>
          <Button
            onPress={this._onPressButton}
            title="This looks great!"
          />
          <Button
            onPress={this._onPressButton}
            title="OK!"
            color="#841584"
          />
        </View>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
   flex: 1,
   justifyContent: 'center',
  },
  buttonContainer: {
    margin: 20
  },
  alternativeLayoutButtonContainer: {
    margin: 20,
    flexDirection: 'row',
    justifyContent: 'space-between'
  }
})
```

RN提供了很多样式的动效的按钮，还能捕获长按事件

```js
import React, { Component } from 'react';
import { Alert, Platform, StyleSheet, Text, TouchableHighlight, TouchableOpacity, TouchableNativeFeedback, TouchableWithoutFeedback, View } from 'react-native';

export default class Touchables extends Component {
  _onPressButton() {
    Alert.alert('You tapped the button!')
  }

  _onLongPressButton() {
    Alert.alert('You long-pressed the button!')
  }


  render() {
    return (
      <View style={styles.container}>
        <TouchableHighlight onPress={this._onPressButton} underlayColor="white">
          <View style={styles.button}>
            <Text style={styles.buttonText}>TouchableHighlight</Text>
          </View>
        </TouchableHighlight>
        <TouchableOpacity onPress={this._onPressButton}>
          <View style={styles.button}>
            <Text style={styles.buttonText}>TouchableOpacity</Text>
          </View>
        </TouchableOpacity>
        <TouchableNativeFeedback
            onPress={this._onPressButton}
            background={Platform.OS === 'android' ? TouchableNativeFeedback.SelectableBackground() : ''}>
          <View style={styles.button}>
            <Text style={styles.buttonText}>TouchableNativeFeedback</Text>
          </View>
        </TouchableNativeFeedback>
        <TouchableWithoutFeedback
            onPress={this._onPressButton}
            >
          <View style={styles.button}>
            <Text style={styles.buttonText}>TouchableWithoutFeedback</Text>
          </View>
        </TouchableWithoutFeedback>
        <TouchableHighlight onPress={this._onPressButton} onLongPress={this._onLongPressButton} underlayColor="white">
          <View style={styles.button}>
            <Text style={styles.buttonText}>Touchable with Long Press</Text>
          </View>
        </TouchableHighlight>
      </View>
    );
  }
}

const styles = StyleSheet.create({
  container: {
    paddingTop: 60,
    alignItems: 'center'
  },
  button: {
    marginBottom: 30,
    width: 260,
    alignItems: 'center',
    backgroundColor: '#2196F3'
  },
  buttonText: {
    textAlign: 'center',
    padding: 20,
    color: 'white'
  }
})
```

### 滚动视图

ScrollView 不仅可以垂直滚动，还能水平滚动（通过horizontal属性来设置）

ScrollView适合用来显示数量不多的滚动元素。放置在ScrollView中的所有组件都会被渲染，哪怕有些组件因为内容太长被挤出了屏幕外。如果需要显示较长的滚动列表，那么应该使用功能差不多但性能更好的FlatList组件


```js
import React from 'react';
import { Image, ScrollView, Text } from 'react-native';
// 加载logo图片
const logo = {
  uri: 'https://reactnative.dev/img/tiny_logo.png',
  width: 64,
  height: 64
};

export default App = () => (
  <ScrollView>
    <Text style={{ fontSize: 96 }}>Scroll me plz</Text>
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Text style={{ fontSize: 96 }}>If you like</Text>
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Text style={{ fontSize: 96 }}>Scrolling down</Text>
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Text style={{ fontSize: 96 }}>What's the best</Text>
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Text style={{ fontSize: 96 }}>Framework around?</Text>
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Image source={logo} />
    <Text style={{ fontSize: 80 }}>React Native</Text>
  </ScrollView>
);
```

### FlatList组件

和ScrollView不同的是，FlatList并不立即渲染所有元素，而是优先渲染屏幕上可见的元素。

必须的两个属性是data和renderItem。

+ data是列表的数据源
+ renderItem则从数据源中逐个解析数据，然后返回一个设定好格式的组件来渲染

```js
import React from 'react';
import { FlatList, StyleSheet, Text, View } from 'react-native';

const styles = StyleSheet.create({
  container: {
   flex: 1,
   paddingTop: 22
  },
  item: {
    padding: 10,
    fontSize: 18,
    height: 44,
  },
});

const FlatListBasics = () => {
  return (
    <View style={styles.container}>
      <FlatList
        data={[
          {key: 'Devin'},
          {key: 'Dan'},
          {key: 'Dominic'},
          {key: 'Jackson'},
          {key: 'James'},
          {key: 'Joel'},
          {key: 'John'},
          {key: 'Jillian'},
          {key: 'Jimmy'},
          {key: 'Julie'},
        ]}
        renderItem={
          ({item}) => <Text style={styles.item}>{item.key}</Text>
          }
      />
    </View>
  );
}

export default FlatListBasics;
```

如果数据分组且携带标签，可以用seciton list
```js
const SectionListBasics = () => {
    return (
      <View style={styles.container}>
        <SectionList
          sections={[
            {title: 'D', data: ['Devin', 'Dan', 'Dominic']},
            {title: 'J', data: ['Jackson', 'James', 'Jillian', 'Jimmy', 'Joel', 'John', 'Julie']},
          ]}
          renderItem={({item}) => <Text style={styles.item}>{item}</Text>}
          renderSectionHeader={({section}) => <Text style={styles.sectionHeader}>{section.title}</Text>}
          keyExtractor={(item, index) => index}
        />
      </View>
    );
}
```

## 交互组件

### 触摸事件

查看上一节内容

### 导航器跳转

 React Navigation 提供了简单易用的跨平台导航方案

内部有若干示例

https://react-navigation-example.netlify.app/


## 网络请求

RN提供的函数叫fetch

```js
fetch('https://mywebsite.com/mydata.json'); // 可以这么写

// 也可以手动指定http参数
fetch('https://mywebsite.com/endpoint/', {
  method: 'POST',
  headers: {
    Accept: 'application/json',
    'Content-Type': 'application/json',
  },
  body: JSON.stringify({
    firstParam: 'yourValue',
    secondParam: 'yourOtherValue',
  }),
});
```

处理服务器回复的数据，网络请求天然是一种异步操作。Fetch 方法会返回一个Promise

```js
function getMoviesFromApiAsync() {
  return fetch(
    'https://facebook.github.io/react-native/movies.json',
  )
    .then(response => response.json())
    .then(responseJson => {
      return responseJson.movies;
    })
    .catch(error => {
      console.error(error);
    });
}
```
函数式组件例子

```js
import React, { useEffect, useState } from 'react';
import { ActivityIndicator, FlatList, Text, View } from 'react-native';

export default App = () => {
  const [isLoading, setLoading] = useState(true);
  const [data, setData] = useState([]);

  useEffect(() => {
    fetch('https://reactnative.dev/movies.json')
      .then((response) => response.json())
      .then((json) => setData(json.movies))
      .catch((error) => console.error(error))
      .finally(() => setLoading(false));
  }, []);

  return (
    <View style={{ flex: 1, padding: 24 }}>
      {isLoading ? <ActivityIndicator/> : (
        <FlatList
          data={data}
          keyExtractor={({ id }, index) => id}
          renderItem={({ item }) => (
            <Text>{item.title}, {item.releaseYear}</Text>
          )}
        />
      )}
    </View>
  );
};
```

### websocket

```js
const ws = new WebSocket('ws://host.com/path');

ws.onopen = () => {
  // connection opened
  ws.send('something'); // send a message
};

ws.onmessage = e => {
  // a message was received
  console.log(e.data);
};

ws.onerror = e => {
  // an error occurred
  console.log(e.message);
};

ws.onclose = e => {
  // connection closed
  console.log(e.code, e.reason);
};
```

## 遇到的代码问题解析

1. 代码模版的内容PropsWithChildren和Section({children, title}: SectionProps)的含义

```js
type SectionProps = PropsWithChildren<{   // 会自动添加一个ReactNode类型的children变量
  title: string;
}>;

function Section({children, title}: SectionProps): JSX.Element {  // 取出SectionProps对象的children和title两个变量赋值
  const isDarkMode = useColorScheme() === 'dark';
  return (
    <View style={styles.sectionContainer}>
    // 其他代码内容...
  )
```

2. 布局问题，不显示按钮或者组件

组件定义了style，其中flex:1导致的，flex: 1 的意思是：“请给我尽可能多的空间”，
但这个“尽可能多”是有前提的：父容器必须有高度，它才知道怎么分配空间。

我的问题是，父 View 没有设定高度，所以 ButtonBasics 的 flex: 1 就 没有空间可用，结果就是：按钮不可见

3. {}结构问题

编写代码的时候，如下代码让我疑惑,
为什么item要加{}?
```js
const data=[
    {key: 'Devin'},
    {key: 'Julie'},
  ]
 <FlatList data={data} renderItem={
            ({item})=>{
              return <Text>{item.key}</Text>
            }
          }/>
```

实际上，完整写法是：

```js
renderItem={(info) => {
  console.log(info.item);
  console.log(info.index);
  return <Text>{info.item.key}</Text>;
}}
```

JavaScript 提供了一种叫 解构（Destructuring） 的语法，可以从对象中直接提取属性


## react native 导航

> 开源工具：React Navigation

1. 安装stack navigator 库 `npm install @react-navigation/native-stack`

2. 安装elements库，`npm install @react-navigation/elements`

可能会出现错误："RNCSafeAreaProvider" was not found in the UIManager，属于兼容性问题，升级RN版本可解决

官方示例

```js
// In App.js in a new project

import * as React from 'react';
import { View, Text } from 'react-native';
import { createStaticNavigation } from '@react-navigation/native';
import { createNativeStackNavigator } from '@react-navigation/native-stack';

function HomeScreen() {
  return (
    <View style={{ flex: 1, alignItems: 'center', justifyContent: 'center' }}>
      <Text>Home Screen</Text>
    </View>
  );
}

const RootStack = createNativeStackNavigator({
  screens: {
    Home: HomeScreen,
  },
});

const Navigation = createStaticNavigation(RootStack);

export default function App() {
  return <Navigation />;
}
```

细化配置

```js
const RootStack = createNativeStackNavigator({
  initialRouteName: 'Home',    // 初始渲染的页面
  screens: {
    Home: {                   // 可以用大括号表示对象的方式添加额外参数
      HomeScreen
    },
    Details: DetailsScreen,   // 添加的第二个页面
  },
});
``

