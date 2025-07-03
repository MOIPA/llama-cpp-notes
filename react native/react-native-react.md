# react基础

react-native基于react，需要了解基础，简单记录下核心的一些东西

### React 的核心概念

+ components 组件
+ JSX
+ props 属性
+ state 状态

#### 导出

React 是默认导出；
{ useState } 是具名导出；

```js
// math.js
export const add = (a, b) => a + b;
export const subtract = (a, b) => a - b;
export default multiply = (a, b) => a * b;

// 导入文件只能
import multiply, { add, subtract } from './math';

```

#### Demo

```js
// 第一个DEMO 使用import语句来引入React以及React Native的Text组件
import React from 'react';
import { Text } from 'react-native';

const Cat = () => {  // 一个简单的函数就可以作为一个组件
  return (
    <Text>Hello, I am your cat!</Text>   // 返回值就会被渲染为一个 React 元素。这里Cat会渲染一个<Text>元素
  );
}

export default Cat; // export default语句来导出这个组件，以使其可以在其他地方引入使用
```

### JSX

第一个Demo里的return写法就是JSX，`<Text>Hello, I am your cat!</Text>`是一种简化 React 元素的写法，这种语法名字叫做 JSX

使用变量
```js
import React from 'react';
import { Text } from 'react-native';

const Cat = () => {
  const name = "Maru";
  return (
    <Text>Hello, I am {name}!</Text>
    // 括号中可以使用任意 JavaScript 表达式，包括调用函数，例如{getFullName("Rum", Tum", "Tugger")}：
    <Text>
      Hello, I am {getFullName("Rum", "Tum", "Tugger")}!
    </Text>
  );
}

export default Cat;
```
### 组件

 自定义组件使用，自定义了组件Cat，这是JSX写法

```js
import React from 'react';
import { Text, TextInput, View } from 'react-native';

const Cat = () => {
  return (
    <View>
      <Text>I am also a cat!</Text>
    </View>
  );
}
// Cafe是一个父组件，而每个Cat则是子组件
const Cafe = () => {
  return (
    <View>
      <Text>Welcome!</Text>
      <Cat />
      <Cat />
      <Cat />
    </View>
  );
}

export default Cafe;
```

### Props 属性

父组件需要给子组件传递信息

```js
import React from 'react';
import { Text, View } from 'react-native';

const Cat = (props) => {
  return (
    <View>
      <Text>Hello, I am {props.name}!</Text>
    </View>
  );
}

const Cafe = ()=>{
    return(
        <View>
            <Cat name='Maru' />
            <Cat name='Spot'/>
        </View>
    )
}

export default Cafe;
```

React Native 的绝大多数核心组件都提供了可定制的 props。例如，在使用Image组件时，你可以给它传递一个source属性，用来指定它显示的内容

```js
import React from 'react';
import { Text, View, Image } from 'react-native';

const CatApp = () => {
  return (
    <View>
    // source和style都是子组件image的props
      <Image
      // 这里是两个大括号，外面的大括号是JS值引用，里面的{}是一个对象k:v格式
        source={{uri: "https://reactnative.dev/docs/assets/p_cat1.png"}}
        style={{width: 200, height: 200}}
      />
      <Text>Hello, I am your cat!</Text>
    </View>
  );
}

export default CatApp;
```

### State状态

有点类似kotlin的jetpackcompose的ViewModel，或者是`var xx by mutableStateOf(0)`,是不随组件重组而重新初始化的。

props 用来配置组件的第一次渲染（初始状态）。state 则用来记录组件中任意可能随时间变化的数据

调用setIsHungry这样的设置状态的函数时，其所在的组件会重新渲染。一整个Cat函数都会从头重新执行一遍。重新执行的时候，useState会返回给我们新设置的值

```js
import React,{useState} from "react";
import {Button,Text,View} from "react-native";

const Cat = (props)=>{

    // 定义变量和对应的set函数，默认值是true
    const [isHungry,setIsHungry] = useState(true);
    return(
        <View>
        I am {props.name},and I am {isHungry?"hungry":"full"}
        </View>
        <Button>
        onPress={()=>{
            setIsHungry(false);
        }}
        disabled={!isHungry}
        title={isHungry ? "Pour me some milk, please!" : "Thank you!"}
        </Button>
    )
}

const Cafe = ()=>{
    return(
        <>
            <Cat name="MARU"/>
            <Cat name="SPOT"/>
        </>
    )
}

```
