# Android

## 基本界面和代码模式

1. @Composable注解：这个注解代表界面组件，注解的函数不返回任何值只描述组件外观，内置了多种布局和组件

```kotlin
// 代码是DSL模式的，即让代码看起来像配置
@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Surface(color=Color.Cyan) {  
        Column {
            Text(
                text = "Hello my name is $name!",
                modifier = modifier.padding(24.dp)
            )
            Text(
                text = "TEST"
            )
        }

    }
}

// 一个模仿Jetpack的DSL模式样例
fun main(args: Array<String>) {
    println("DSL")

//    val col = Column()
//    col.Text(Text("1"))
//    col.Text(Text("2"))
//    col.print()

    // DSL 模式
    fun Column(init:Column.()->Unit):Column{
        val col = Column()
        col.init()
        return col
    }
    val newCol = Column{
        text(
            text="3"
        )
        text(
            text="4"
        )
    }
    newCol.print()
}

class Text(val text:String){}
class Column(){
    val textList = mutableListOf<Text>()
    fun text(text:String){
        textList.add(Text(text))
    }
    fun print(){
        textList.forEach{print(" ${it.text}")}
    }
}
```

2. @Preview注解，该注解用于预览组件效果，该注解接受参数，背景，名称等

3. @Composeable原理，写kotlin的安卓代码刚一接触非常不习惯，这是因为它是声明式的代码，比如以下代码，Greeting函数没有维护任何东西，只是执行了Text声明了一个对象，也没有返回值，非常让人迷惑。实际后期编译的时候会对所有@Composeable注解插桩，记录Text节点和整个函数和参数。最后使用applier来绘制所有UI节点。

```kotlin
@Composable
fun Greeting(name: String) {
    Text(text = "Hello $name!")
}
// 插桩代码
fun Greeting(name: String, $composer: Composer<*>, $changed: Int) {
    $composer.startElement("Greeting")
    Text("Hello $name", $composer, 0)
    $composer.endElement()
}
```

4. 字体：SP(可缩放像素)，DP（密度无关像素）

5. 布局：3个标准布局元素 Column,Row,Box，他们都存在最后一个形参content（函数），所以可以使用cotlin的尾随lambda。
```kotlin
//这段代码中的 content = { ... } 就是 DSL（Domain Specific Language）模式 的一种实现方式，它本质上是一个 lambda 表达式，Compose 会在 Row 内部调用其中的 Text 函数

Row(
    content = {
        Text("Some text")
        Text("Some more text")
        Text("Last text")
    }
)
// 尾随形式
Row {
    Text("Some text")
    Text("Some more text")
    Text("Last text")
}

// Row 的定义大概是这样的（简化版）

@Composable
fun Row(
    modifier: Modifier = Modifier,
    content: @Composable () -> Unit
) {
    // Compose 会在这里调用 content()
    content()
}
```

6. 设置元素居中，例子，Text组件内文本和布局元素的居中
```kotlin
@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Surface(color=Color.Cyan) {
        Column(verticalArrangement = Arrangement.Center) {
            Text(
                text = "Hello my name is $name!",
                modifier = modifier.padding(24.dp)
            )
            Text(
                text = "TEST", textAlign = TextAlign.Center
            )
        }

    }
}
```

7. 资源管理类R，R类是android自动生成的类用于管理项目的所有资源ID（文件名）

```kotlin
// 图片加载示例
@Composable
fun GreetingImage(message:String,from:String,modifier: Modifier= Modifier){
    val image = painterResource(R.drawable.logo)
    Image(painter = image, contentDescription = null)
}
```

8. 字符串资源，编辑时硬编码的字符串可以被抽取extract成资源，配置在res/values/strings.xml文件中，使用`R.string.名字`访问

9. Card卡片布局， 是一种在单个容器中显示内容和操作的 Surface布局现在已经取代了Card
```kotlin
Card(modifier=modifier) {
        Column {
            Image(
                painter= painterResource(affirmation.imageResourceId),
                contentDescription = stringResource(affirmation.stringResourceId),
                modifier=Modifier.fillMaxWidth().height(194.dp),
                contentScale = ContentScale.Crop
            )
            Text(
                text= stringResource(affirmation.stringResourceId),
                modifier= Modifier.padding(16.dp),
                style= MaterialTheme.typography.headlineSmall
            )
        }
    }
```

10. LazyColumn 自带懒加载和滚动条，适合渲染长列表的Column

```kotlin
@Composable
fun AffirmationList(affirmationList:List<Affirmation>,modifier: Modifier= Modifier){
    LazyColumn(modifier=modifier) {
        items(affirmationList){ affirmation->
            AffirmationsCard(affirmation=affirmation,
                modifier=Modifier.padding(8.dp))
        }
    }
}
```

## 组件和状态

1. TextField组件，onValueChange传入回调函数

```kotlin
@Composable
fun EditNumberField(modifier: Modifier=Modifier){
    var amountInput by remember {  mutableStateOf("") }
    TextField(
        value=amountInput,
        onValueChange = {amountInput = it},
        leadingIcon = {Icon(painter= painterResource(id=leadingIcon),null)}, // 文本框输入前的图标 可选
        modifier=modifier,
        // 输入框添加一个Text组件用于提示，用户点击后会自动到左上角
        label = {Text(stringResource(R.string.bill_amount))},
        singleLine = true  // 输入框变成单行
        // 设置用户输入的键盘为数字键盘 只能输入数字
        ,keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number)

    )
}
```

2. button组件,onClick传入回调函数

```kontlin
var result by remember {mutableStateOf(0)}
Button(onClick = { result = (1..6).random() }) {
            Text(text = stringResource(R.string.roll))
        }
```

3. by remember ，记录Compose中的状态，在1和2中定义的几个变量都是通过by remember定义的，这是因为compose监控变量变化时会执行重组，回去重新执行@Composable标记的函数，这时候变量会被重新初始化，只有by remember后变量才能不被重新执行初始化，remember是不会触发重组的！！！


4. 子组件通信问题，若存在2个子组件A，B，A组件会计算并维护状态，B组件展示状态，那么就需要AB进行通信，AB都维护状态的情况下变量只在各自内部可见。需要将AB改写为无状态的，变量由父组件进行维护，通过给AB两个字组件传入lambda函数更新父组件变量，例子：

```kotlin

// 计算小费函数
private fun calculateTip(amount: Double, tipPercent: Double = 15.0): String {
    val tip = tipPercent / 100 * amount
    return NumberFormat.getCurrencyInstance().format(tip)
}
@Composable
fun EditNumberField(value:String, onValueChange:(String)-> Unit, modifier: Modifier=Modifier){
    TextField(
        value=value,
        onValueChange = onValueChange,
        modifier=modifier,
        label = {Text(stringResource(R.string.bill_amount))},
        singleLine = true,
        keyboardOptions = KeyboardOptions(keyboardType = KeyboardType.Number)
    )
}

@Composable
fun TipTimeLayout() {
    var amountInput by remember {  mutableStateOf("") }
    val amount = amountInput.toDoubleOrNull()?:0.0
    val tip = calculateTip(amount)
    
    Column(
        modifier = Modifier
            .statusBarsPadding()
            .padding(horizontal = 40.dp)
            .verticalScroll(rememberScrollState())
            .safeDrawingPadding(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = stringResource(R.string.calculate_tip),
            modifier = Modifier
                .padding(bottom = 16.dp, top = 40.dp)
                .align(alignment = Alignment.Start)
        )
        EditNumberField(modifier = Modifier.padding(bottom = 32.dp).fillMaxWidth()
            , onValueChange = {amountInput=it}
            , value = amountInput)
        Text(
            text = stringResource(R.string.tip_amount, tip),
            style = MaterialTheme.typography.displaySmall
        )
        Spacer(modifier = Modifier.height(150.dp))
    }
}
```

5. Resource作为字符串形参传入函数，最好在函数定义的时候添加注解,用于表明要传递的整数是 values/strings.xml 文件中的字符串资源。这些注解对于处理代码的开发者以及 Android Studio 中的代码检查工具（如 lint）非常有用

```kontlin
@Composable
fun EditNumberField(
    @StringRes label: Int,   // 调用该函数的时候可以传入R.string.xxx
    value: String,
    onValueChanged: (String) -> Unit,
    modifier: Modifier = Modifier
) 
```

6. Switch组件

```kotlin
@Composable
fun RoundTheTipRow(roundup: Boolean, onRoundChange:(Boolean)->Unit, modifier: Modifier= Modifier){
    Row(modifier=modifier.fillMaxWidth().size(48.dp)
    , verticalAlignment = Alignment.CenterVertically) {
        Text(text=stringResource(R.string.round_up_tip))
        Switch(
            modifier=modifier.fillMaxWidth().wrapContentWidth(Alignment.End),
            checked = roundup,
            onCheckedChange = onRoundChange
        )
    }
}
```

7. 添加滚动条，一般app支持横屏就要添加

```kotlin
modifier = Modifier
            .verticalScroll(rememberScrollState())
```

8. 弹窗

```kontlin
@Composable
private fun FinalScoreDialog(
   onPlayAgain: () -> Unit,
   modifier: Modifier = Modifier
) {
   val activity = (LocalContext.current as Activity)

   AlertDialog(
       onDismissRequest = {
           // Dismiss the dialog when the user clicks outside the dialog or on the back
           // button. If you want to disable that functionality, simply use an empty
           // onDismissRequest.
       },
       title = { Text(stringResource(R.string.congratulations)) },
       text = { Text(stringResource(R.string.you_scored, 0)) },
       modifier = modifier,
       dismissButton = {
           TextButton(
               onClick = {
                   activity.finish()
               }
           ) {
               Text(text = stringResource(R.string.exit))
           }
       },
       confirmButton = {
           TextButton(
               onClick = {
                   onPlayAgain()
               }
           ) {
               Text(text = stringResource(R.string.play_again))
           }
       }
   )
}
```

## Android底层架构

### Activity的生命周期

1. onCreate,onStart,onPause等事件什么时候触发
2. 用户返回主界面或者直接返回，或者部分遮蔽应用，会触发的事件

### Compose的ViewModel和状态

1. StateFlow：是 Kotlin 协程库中的一种 热流（hot flow），它持有当前状态（值）每次值更新时，会通知所有收集者（collectors）始终保持一个“当前值”，新收集者会立即收到当前值

2. ViewModel是MVVM的设计中，绑定UI和数据的存在,ViewModel 会存储并公开界面所使用的状态，ViewModel是不会被重组影响的，所以非常适合保存UI状态

> 例如以下是一个ViewModel，配置完毕后，需要传递给UI组件，UI组件展示ViewModel的内容
>
> 但是需要注意的是，传入到UI组件后，要在组件内转为State，让 Composable 可以观察它，即变量发生改动的时候，自动重组Composable的组件，如果是普通变量是不会触发重组的。只有使用了 State 或 remember 等 Compose 状态机制的变量，修改时才会触发界面重组
>
> 一旦UI组件状态修改了，用户输入了新的值，UI组件要赋值的时候，不能赋值给拿到的State变量，只能调用ViewModel函数更新变量。
>
> 还需要注意的是：_uiState修改后，uiState也会自动修改

```kontlin
class GameViewModel: ViewModel() {
    // UI显示的当前乱序单词
    // ViewModel内部可以修改
    private val _uiState = MutableStateFlow(GameUiState())
    // 后备属性 对外只暴露可读
    val uiState: StateFlow<GameUiState> = _uiState.asStateFlow()
}
```

## 并发

### kotlin协程

#### 概念

协程是运行在线程上的“轻量级线程”，它由 Kotlin 协程库在用户空间调度，而不是由操作系统调度。协程可以运行在一个或多个线程上，一个线程可以运行多个协程。

挂起时发生了什么？

当协程遇到挂起函数（如 delay()、await()）时它会暂停执行自己的逻辑释放当前线程，让线程可以去执行其他协程,等到挂起操作完成后，协程会在合适的线程上恢复执行

挂起函数是运行在协程内的

```kotlin
import kotlinx.coroutines.*
import kotlin.system.measureTimeMillis

fun main() {
    val time = measureTimeMillis {
        runBlocking {
            println("Weather forecast")
            printForecast()
            printTemperature()
        }
    }
    println("Execution time: ${time / 1000.0} seconds")

}
suspend fun printForecast() {  // suspend函数，挂起函数只能从协程或其他挂起函数中调用
    delay(1000)         // delay是挂起函数，调用它需要也是挂起函数
    println("Sunny")
}
suspend fun printTemperature(){
    delay(1000)
    println("30\u00b0C")
}
```
#### runBlocking

runBlocking 是 Kotlin 协程库提供的一个函数，用于启动一个协程并阻塞当前线程，直到协程执行完毕。可以在 runBlocking 里面启动多个协程（如 launch）


#### launch关键字

协程启动器，不返回结果启动一个并发任务，以下代码为例就是同时两个协程执行

```kotlin
runBlocking {
            println("Weather forecast")
            launch {  printForecast()}
            launch {printTemperature()}
        }
```

#### async关键字

有返回值的异步任务，async() 函数会返回一个类型为 Deferred 的对象，就好像在承诺结果准备就绪后就会出现在其中。使用 await() 访问 Deferred 对象上的结果

```kontlin
val time = measureTimeMillis {
        runBlocking {
            println("Weather forecast")
            val forecast:Deferred<String> = async { printForecast() }
            val tmperature:Deferred<String> = async {printTemperature()}
            println(forecast.await())
            println(tmperature.await())
        }
    }
```

#### coroutineScope

coroutineScope 是一个挂起函数**，它会创建一个新的协程作用域（scope），并等待该作用域内所有协程执行完毕。它常用于在一个挂起函数中启动多个子协程，并保证它们都执行完成后再继续执行后续代码。

和 runBlocking 的区别：不阻塞，继承外部协程的上下文，可以被取消（随父协程取消）

```kotlin
suspend fun getWeatherReport()= coroutineScope {
    // coroutineScope{} 用于创建局部作用域。在此作用域内启动的协程会归入此作用域内，这对取消和异常都会产生影响
    val forecast:Deferred<String> = async { printForecast() }
    val temperature:Deferred<String> = async {printTemperature()}
    "${forecast.await()} ${temperature.await()}"
}
```

#### 异常和取消

#### 异常

主动抛出一个异常

```kotlin
suspend fun printTemperature():String{
    delay(1000)
    throw AssertionError("Temperature is invalid")
    return "30\u00b0C"
}
```

子协程因发生异常而失败时，异常会向上传播。系统会取消父协程，继而取消所有其他子协程。最后，错误会向上传播，程序也会崩溃并出现 AssertionError

> 解决方案：使用try-catch捕获可能发生的异常，若async块内有返回值，可以在catch中返回发生错误时应该返回的值

针对上面抛出的异常

```kotlin
try{println(getWeatherReport())}catch (e:AssertionError){
                println("Caught exception :$e")
            }
```

#### 取消

某个父作业被取消，那么其子作业也会被取消

手动取消代码

```kotlin
val forecast:Deferred<String> = async { printForecast() }
// 这里的async替换成launch也可以
forecast.cancel()
```

CoroutineScope被取消，那么其作业也会被取消，而且取消会传播到其子作业。如果作用域内的某个子作业失败并引发异常，那么其他子作业会被取消，父作业也会被取消，而且会向调用方重新抛出异常。

#### 协程调度

#### CoroutineContext

CoroutineContext 提供将在其中运行协程的上下文的相关信息。CoroutineContext 本质上是一个用于存储元素的映射，其中的每个元素都有一个唯一的键。这些并非必填字段，不过下面列举了一些上下文中可能包含的字段：

1. name - 协程的名称，用于唯一标识协程，协程名称的默认值为“coroutine”
2. job - 控制协程的生命周期，父作业无默认值
3. dispatcher - 将工作分派到适当的线程，协程调度程序的默认值为 Dispatchers.Default
4. exception handler - 处理协程中执行的代码所抛出的异常，异常处理程序无默认值

如果在协程中启动一个新协程，子协程将从父协程继承 CoroutineContext，如果想对启动的子协程修改CoroutineContext的某个字段，只需要在launch或者async的时候传入要修改的参数即可

#### 调度程序

调度程序来确定用于执行协程的线程，Kotlin 提供了一些内置调度程序

+ Dispatchers.Main:：使用此调度程序可在 Android 主线程上运行协程，只有 1 个主线程（UI 线程）UI 线程负责渲染界面，如果你在这里做网络或磁盘操作，界面会“卡住”

+ Dispatchers.IO:：此调度程序经过了专门优化，适合在主线程之外执行磁盘或网络 I/O。

+ Dispatchers.Default:：在调用 launch() 和 async() 时，如果其上下文中未指定调度程序，就会使用此默认调度程序，用于 CPU 密集型任务

这三个调度器到底有什么区别？它们不是都开启一个线程来执行协程吗？

其实不是开启新线程，而是分配到某个线程，它们都是用于指定协程在哪个线程（或线程池）上运行，但它们的用途不同、线程资源管理不同、性能优化也不同，所以它们不是简单的“都开一个线程”的区别，而是有明确分工的

手动指定协程调度器
```kotlin
runBlocking {
            launch {
                withContext(Dispatchers.Default){
                    delay(1000)
                    println("Weather forecast")
                }
            }
            println("have a good day")
        }
```

一些库可能已经定义好了协程的调度器，不用手动指定



### android 协程

#### LaunchedEffect

UI层启动协程的非阻塞方式

android提供的非阻塞的函数，类似launch不会阻塞UI线程，用于在 UI 中启动协程绑定到 Compose 的生命周期当 key 变化时，协程会取消并重新启动

当key变化时，协程就会取消旧的、启动新的！

```kotlin
LaunchedEffect(playerOne, playerTwo) {
            coroutineScope{
                launch{playerOne.run()}
                launch{playerTwo.run()}
            }
            raceInProgress=false
        }
```
#### ViewModelScope

ViewModel层启动协程的非阻塞方式

假设有`class MarsViewModel:ViewModel(){...}`，想在这个类中异步访问网络

```kotlin
fun getMarsPhotos() {
        viewModelScope.launch {
            val listResult = MarsApi.retrofitService.getPhotos()
        }
    }
```

## 网络通信

### 添加网络权限

`manifests/AndroidManifest.xml`中添加到application标签前面`<uses-permission android:name="android.permission.INTERNET" />`

### Retrofit 库

build.gradle.kts添加配置
```
// Retrofit
    implementation("com.squareup.retrofit2:retrofit:2.9.0")
// Retrofit with Scalar Converter
    implementation("com.squareup.retrofit2:converter-scalars:2.9.0")
```

添加network包，并添加网络连接服务类
```kotlin
package com.example.marsphotos.network

import retrofit2.Retrofit

private const val BASE_URL="https://android-kotlin-fun-mars-server.appspot.com"
private val retrofit = Retrofit.Builder()

class MarsApiService {
}
```

配置retrofit，配置api
```kotlin
package com.example.marsphotos.network

import retrofit2.Retrofit
import retrofit2.converter.scalars.ScalarsConverterFactory
import retrofit2.create
import retrofit2.http.GET

private const val BASE_URL="https://android-kotlin-fun-mars-server.appspot.com"

private val retrofit = Retrofit.Builder()
    .addConverterFactory(ScalarsConverterFactory.create())
    // 转换器会告知 Retrofit 如何处理它从 Web 服务获取的数据
    // ScalarsConverter，支持字符串和其他基元类型
    .baseUrl(BASE_URL) // 访问的基础网址
    .build()        // 创建Retrofit对象

interface MarsApiService{
    @GET("photos")     // GET请求，路径是 baseurl/photos
    fun getPhotos():String
}
// 单例对象 将api和retrofit创建器结合
object MarsApi{
    val retrofitService: MarsApiService by lazy{    // 延迟初始化，第一次使用才初始化
        retrofit.create(MarsApiService::class.java)
    }
}
```

在ViewModel中使用
```kotlin
fun getMarsPhotos() {
        viewModelScope.launch {
            val listResult = MarsApi.retrofitService.getPhotos()
        }
    }
```

### 状态表示

上面的例子没办法显示网络成功，失败，一般自己定义这些

```kotlin
sealed interface MarsUiState{
    // 这种写法很新奇，将对接口的子类实现写在接口内部
    data class Success(val photos: String): MarsUiState
    object Error : MarsUiState
    // 是MarsUiState的一个实例，相当于创建class实现MarsUiState接口并生成一个实例
    object Loading : MarsUiState
}
```

整个Demo
```kotlin
sealed interface MarsUiState{
    // 这种写法很新奇，将对接口的子类实现写在接口内部
    data class Success(val photos: String): MarsUiState
    object Error : MarsUiState
    // 是MarsUiState的一个实例，相当于创建class实现MarsUiState接口并生成一个实例
    object Loading : MarsUiState
}
class MarsViewModel : ViewModel() {
    var marsUiState: MarsUiState by mutableStateOf(MarsUiState.Loading)
        private set
    init {
        getMarsPhotos()
    }
    fun getMarsPhotos() {
        viewModelScope.launch {
            try {
                val listResult = MarsApi.retrofitService.getPhotos()
                marsUiState = MarsUiState.Success(listResult)
            }catch (e:IOException){
                marsUiState = MarsUiState.Error
            }
        }
    }
}
```

### Retrofit JSON反序列化

添加依赖
```xml
# 插件配置
    id("org.jetbrains.kotlin.plugin.serialization") version "1.8.10"

#依赖
implementation("org.jetbrains.kotlinx:kotlinx-serialization-json:1.5.1")
implementation("androidx.lifecycle:lifecycle-viewmodel-compose:2.6.2")
// Retrofit
implementation("com.squareup.retrofit2:retrofit:2.9.0")
// Retrofit with Scalar Converter
implementation("com.jakewharton.retrofit:retrofit2-kotlinx-serialization-converter:1.0.0")
implementation("com.squareup.okhttp3:okhttp:4.11.0")
```

添加数据类,MarsPhoto 类中的每个变量都对应于 JSON 对象中的一个键名,kotlinx serialization 解析 JSON 时，它会按名称匹配键，并用适当的值填充数据对象。

如果Json的key和变量名不同，需要使用@SerialName(value = "img_src") 在变量上

```kotlin
@Serializable
data class MarsPhoto(
    val id:String,val img_src:String
)
```
apiservice文件内容
```kotlin
private const val BASE_URL="https://android-kotlin-fun-mars-server.appspot.com"

private val retrofit = Retrofit.Builder()
    .addConverterFactory(Json.asConverterFactory("application/json".toMediaType()))
    // 转换器会告知 Retrofit 如何处理它从 Web 服务获取的数据
    .baseUrl(BASE_URL) // 访问的基础网址
    .build()        // 创建Retrofit对象

interface MarsApiService{
    @GET("photos")     // GET请求，路径是 baseurl/photos
    fun getPhotos():List<MarsPhoto>
}
// 单例对象 将api和retrofit创建器结合
object MarsApi{
    val retrofitService: MarsApiService by lazy{    // 延迟初始化，第一次使用才初始化
        retrofit.create(MarsApiService::class.java)
    }
}
```

## 分层架构

### 仓库类

仓库类的作用包括：

1. 向应用的其余部分公开数据。
2. 集中管理数据更改。
3. 解决多个数据源之间的冲突。
4. 对应用其余部分的数据源进行抽象化处理。
5. 存放业务逻辑。

### 分层实例

在MarsPhotos例子中
1. 首先有个network层，定义了接受的Json内的每个对象数据类，定义网络api接口获取数据
2. 定义data层，针对不同数据来源编写不同的Repository，定义Repository接口和对应的具体实现，接收网络Api接口的具体实现对象，通过对象获取数据
3. data层中，还有container接口，维护repository接口，并实现具体container类，实现具体的网络api接口，注入到network层使用
4. ViewModel层，这层编写获取数据的方法需要viewModelScope.launch异步获取，调用data层的repository对应获取数据方法
5. UI层，使用ViewModel层的数据，当ViewModel层数据变动时自动重组

## 图片下载和解析

### Coil 

下载、缓冲、解码以及缓存图片的库

依赖：`implementation("io.coil-kt:coil-compose:2.4.0")`

### AsyncImage

用于异步执行图片请求并呈现结果的可组合项

```kotlin
@Composable
fun MarsPhotoCard(photo: MarsPhoto,modifier: Modifier= Modifier){
    AsyncImage(
        model = ImageRequest.Builder(context = LocalContext.current)
            .data("图片网址")
            .crossfade(true)
            .build(),
        contentDescription = stringResource(R.string.mars_photo),
        contentScale = ContentScale.Crop,
        modifier = Modifier.fillMaxWidth(),
        error = painterResource(R.drawable.ic_broken_image), // 如果图片加载失败就显示这个图标
        placeholder = painterResource(R.drawable.loading_img), // 图片加载中图标
    )
}
```

### 网格布局

最适合图片展示的布局

```kotlin
@Composable
fun PhotosGridScreen(
    photos: List<MarsPhoto>,
    modifier: Modifier = Modifier,
    contentPadding: PaddingValues = PaddingValues(0.dp),
) {
    LazyVerticalGrid(
        columns = GridCells.Adaptive(150.dp), // 指定列的宽度，会自动根据屏幕大小算有多少列
        modifier = modifier.padding(horizontal = 4.dp),
        contentPadding = contentPadding,
    ) {
        items(items=photos,key={photo -> photo.id}){
            photo->MarsPhotoCard(photo)
        }
    }
}
```

## 数据问题

### kotlin flow

就是一个自动轮询数据的工具，会根据生命周期自动停止和启动

android中，flow就是不断获取数据，UI层收集数据的意思就是用flow获取的数据更新UI重组，一旦UI层从flow中读取数据的行为中断了，flow就停止

state flow 一个带缓存功能的 Flow，即使没有收集者，它也会持续保存最新的值。是因为屏幕选择或者配置修改等会导致activity销毁重建，导致flow也重新创建重新开始获取数据，所以需要state flow缓存部分值提供一个更平滑的体验。 Flow 是“冷流”，它会从头开始执行。而state flow是热流，存放在Viewmodel中的，永远不会停止，会在后台一直跑。

比如你正在看一个列表，屏幕旋转后：

1. 如果用 Flow，会重新加载数据，用户看到“空白 → 加载 → 显示”。
2. 如果用 StateFlow，UI 重建后立刻显示上次的数据，体验更好。

`WhileSubscribed(5000)`是作用于冷流Flow的，意思是5秒内没人收集数据才停止，有的话就继续，这样冷流在旋转屏幕就不会重头开始，且能在退出到首页后停止在后台执行。

### Room数据库

Room封装了Sqlite以便于使用，个人感觉就是实现了Spring的通过注解将类和表映射起来，还能映射整个数据库，通过类自动创建表和DB。还实现了spring data jpa的功能，通过注解和接口访问数据库，注解里甚至也可以编写sql

在android中使用，通常还在获取数据@Query注解对应的接口，将返回值包装成Flow类型来自动获取数据。

一般开发流程：

1. Dao层 注解+接口+Flow 定义对表的CRUD操作
2. 数据层：注解+接口 映射表
3. 数据层：注解+接口 映射数据库DB
4. 数据层：DB内返回Dao层接口
5. 数据层：DB内定义伴生对象，也就是单例，确保同一时刻只有一个数据库实例被初始化

### Room实践

依赖

```kotlin
插件配置
id("com.google.devtools.ksp") version "1.9.0-1.0.13" // 根据你的 Kotlin 版本选择


// 依赖项
//Room
implementation("androidx.room:room-runtime:${rootProject.extra["room_version"]}")
ksp("androidx.room:room-compiler:${rootProject.extra["room_version"]}")
implementation("androidx.room:room-ktx:${rootProject.extra["room_version"]}")
```

实体类
```kotlin
@Entity(tableName="items")
data class Item(
    @PrimaryKey(autoGenerate = true)
    val id: Int = 0,
    val name: String,
    val price: Double,
    val quantity: Int
)
```

Dao层
```kotlin
@Dao
interface ItemDao {
    @Insert(onConflict=OnConfictStrategy.IGNORE)
    suspend fun insert(item: Item)
    @Update
    suspend fun update(item: Item)
    @Delete
    suspend fun delete(item: Item)
    // 不需要手动指定suspend，因为Flow会自动将查询放入后台线程执行
    @Query("select * from items where id=:id")
    fun getItem(id: Int): Flow<Item> // 只要数据库中的数据发生更改，就会收到通知
    @Query("select * from items order by name asc")
    fun getAllItems():Flow<List<Item>>
}
```

Database实例
```kotlin
@Database(entities = [Item::class], version = 1, exportSchema = false)
abstract class InventoryDatabase:RoomDatabase() {
    abstract fun itemDao():ItemDao
    companion object {
        @Volatile  // 不会被缓存，一个线程修改其他线程都可见
        private var Instance: InventoryDatabase?=null

        // 初始化实例
        fun getDatabase(context: Context): InventoryDatabase{
            return Instance?:synchronized(this) {
                Room.databaseBuilder(context, InventoryDatabase::class.java,"item_database")
                    .fallbackToDestructiveMigration()
                    .build()
                    .also { Instance = it }
            }
        }
    }
}
```

需要注意的是在android中UI层只能使用StateFlow，所以需要将Flow做转换

```kotlin
// stateIn 转换关键字
val homeUiState: StateFlow<HomeUiState> =
    itemsRepository.getAllItemsStream().map { HomeUiState(it) }
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(TIMEOUT_MILLIS),
            initialValue = HomeUiState()
        )
```



