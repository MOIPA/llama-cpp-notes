# react native 核心UI组件

Android中是用kotlin和jetpack compose编写ui，rn提供了一套自己的UI组件

## 概览

1. `<View>`     一个支持使用flexbox布局、样式、一些触摸处理和无障碍性控件的容器
2. `<Text>`     显示、样式和嵌套文本字符串，甚至处理触摸事件
3. `<Image>`	显示不同类型的图片
4. `<ScrollView>`	一个通用的滚动容器，可以包含多个组件和视图
5. `<TextInput>	`	使用户可以输入文本

## 文本输入

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

## 触摸事件

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