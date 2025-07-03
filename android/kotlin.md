# Kotlin学习

## 基础概览

```kontlin
import kotlin.time.Duration
import kotlin.time.Duration.Companion.hours

fun main(args: Array<String>) {
    println("Hello World!")

    // Try adding program arguments via Run/Debug configuration.
    // Learn more about running applications: https://www.jetbrains.com/help/idea/running-applications.html.
    println("Program argumets: ${args.joinToString()}")
    val contact = Contact(1,"test@gmail.com")
    println(contact.email)
    contact.email = "new@gmail.com"
    println(contact.email)
    contact.printId()

    val user1 = User("alex",1)
    val user2 = User("alex",1)
    println(user1)
    println("user1 == user2 :${user1==user2}")
    println(user1.copy(name="new alex"))

    var nullableVal:String? = "可以为null的变量"
    nullableVal = null
    println(nullableVal)
    println(nullableVal?.length) // 加入问号可以在每次调用函数前自动判断是否为空
    println(null?:0)                // 如果是null值则返回默认值
    println(nullableVal?.length?:0)

    // 扩展函数
    fun String.bold():String = "<b>$this</b>"
    println("hello".bold())

    // 作用域函数

    // let函数 ?:可以在变量为null的时候返回默认值，?. 可以在变量不为null的时候执行函数
    val address:String? = "null"
    fun work1(address:String):String{
        println("address is $address")
        return "success work1"
    }
    val res = address?.let {
        work1(it)  // let函数里面可以多行，但是只返回最后一行的表达式值
        "let function val"
    }
    println(res)

    // apply函数
    val client = Client()
    client.token = "tok1"
    client.connect()
    client.authenticate()
    client.getData()
    // 用apply函数改写  返回值是对象本身
    val client2 = Client().apply {
        token="tok2"
        connect()
        authenticate()
    }
    client2.getData()
    // run函数 run：用于执行一段逻辑并返回结果  返回值是 Lambda 的最后一行表达式
    client2.run { token="tok3"
        connect()
        authenticate()
        getData()
    }
    // also函数 返回原来的对象，有点类似apply，但是可以附加一些动作
    // apply和also都是返回原对象，其他域函数都是返回最后一行的lambda值
    val medals = listOf("gold","silver","bronze")
//    val top2 = medals.map { s:String->s.uppercase() } 可以省略s:String 默认lambda值都是it
    val top2 = medals.map{it.uppercase()}
        .also { println(it) }
        .filter { it.length>4 }
        .also { println(it) }

    // with函数不是拓展函数，但是属于域函数
    val canva = Canvas()
    canva.circ()
    canva.text()
    canva.circ()
    // 这样写很麻烦 可以用with
    with(canva){
        circ()
        text()
        circ()
    }

    // DSL + Lambda 简化代码
    // 传统的Menu初始化和添加模式
    val menuTradition = Menu("menuTradition")
    menuTradition.item("item1")
    menuTradition.item("item2")
    menuTradition.items.forEach{println(it.name)}
    println("new way to init menu")
    // DSL模式
    fun menu(name:String,init:Menu.()->Unit):Menu{
        val menu = Menu(name)
        menu.init()
        return menu
    }
    val menuNew = menu("new menu",{
        item("item5")
        item("item6")
    })
    menuNew.items.forEach{println(it.name)}

    // Kotlin 中使用 open 关键字来允许一个类被继承，使用 : 来表示继承关系。
    val dog:Animal = Dog("dd","male",3,"white")
    dog.makeSound()
    // 抽象类
    val laptop = Electronic("laptop",1000.0,2)
    println(laptop.productInfo())
    // 接口
    val pay1 = CreditCardPayment("1234 5678 9012","user","12/25")
    println(pay1.initiatePayment(100.0))

    // 对象使用
    DoAuth.takeParams("name","123")

    // 伴生对象
    BigBen.getBongs(3)
    println()

    // 手动set和get
    val p = Person()
    p.name = "test name"
    println(p.name)

    // 类型检查 is 检查的是 一个值是否可以安全地转换为某个类型
    val myInt = 42
    println("myInt is Int ${myInt is Int}")
    // 类型转换 as 不安全，null就无法转为其他类型，失败时返回而不是引发错误时 用 as?
    val a:String?=null
    val b = a as? String
    println(b)

    val halfHour:Duration = 0.5.hours
    println(halfHour)

    // 辅助构造函数
    val device = SmartDevice("cc phone","phone",0)
    println("device status ${device.deviceStatus}")
}
class Contact(val id:Int,var email:String="example@gmail.com"){
    val category:String = ""
    fun printId(){
        println(id)
    }
}
// 自带toString，equals函数的类，专门用于存储数据
data class User(val name:String,val id:Int){

}

class Client(){
    var token:String?=null
    fun connect()=println("connected!")
    fun authenticate()=println("authenticated!")
    fun getData()=println("data!")
}

class Canvas(){
    fun text(){}
    fun rect(){}
    fun circ(){}
}

class MenuItem(val name:String)
class Menu(val name:String){
    val items = mutableListOf<MenuItem>()
    fun item(name:String){
        items.add(MenuItem(name))
    }
}

open class Animal(val name:String,val Sex:String,val age:Int){
    open fun makeSound(){
        println("animal sound")
    }
}
class Dog(name:String,sex:String,age:Int,val color:String):Animal(name,sex,age){
    override fun makeSound() {
        println("dog sound")
    }
}
// 抽象类的继承
abstract class Product(val name:String,var price:Double){
    abstract val category:String
    fun productInfo():String{
        return "Product:$name ,Category: $category ,Price:$price"
    }
}
class Electronic(name:String,price:Double,val warranty:Int):Product(name,price){
    override val category:String = "Electronic"
}

// 接口
interface PaymentMethod{
    fun initiatePayment(amount:Double):String
}
interface PaymentType{
    val paymentType:String
}
class CreditCardPayment(val cardNumber:String,val cardHolderName:String,val expiryDate:String):PaymentMethod,PaymentType{
    override fun initiatePayment(amount: Double): String {
        return "Payment of $$amount initiated using Credit Card ending in ${cardNumber.takeLast(4)}"
    }
    override val paymentType:String = "credit card"
}
// 接口的委托实现 接口定义了大量函数，子类还得都写一遍，有很多重复代码
interface  Drawable{
    fun draw()
    fun resize()
    val color:String?
}
open class Circle:Drawable{
    override fun draw() {
        TODO("Not yet implemented")
    }
    override fun resize() {
        TODO("Not yet implemented")
    }
    override val color: String?=null
}
//class RedCircle(val circle:Circle): Circle() {       这样写要将所有函数都写一遍
// 只重写了 color 属性，其它方法直接用 param 的实现 本质还是组合
class RedCircle(param:Circle): Drawable by param {
    override val color: String = "red"
}

// 对象 仅当首次调用函数时，才会创建该对象  实际感觉就是单例
interface Auth {
    fun takeParams(username: String, password: String)
}
object DoAuth:Auth{ // object还支持继承
    override fun takeParams(username:String,pwd:String){
        println("input Auth parameters = $username:$pwd")
    }
}

// 数据对象 1.9和之后才支持
//data object AppConfig{
//    var appName:String = "my app"
//    var version:String = "1.0.0"
//}

//伴生对象 本质就是一个static的成员对象
class BigBen{
    companion object Bonger{
        fun getBongs(nTime:Int){
            repeat(nTime){print("Bong")}
        }
    }
}

// 显式制定类属性的Getter和Setter
class Person{
    var name:String = ""
        set(value){
            field = value.uppercase()
        }
        get(){
            return field
        }
}

class SmartDevice(val name:String,val category:String){
    var deviceStatus:String? = null
    constructor(name:String,category:String,code:Int):this(name,category){
        deviceStatus = when(code){
            0-> "offline"
            1-> "online"
            else -> "unknown"
        }
    }
}
```

## 深入部分

### 泛型+enum类

```kontlin
fun main(args: Array<String>) {
    val question1 = Question<String>("Quoth the raven ___", "nevermore", Difficulty.medium)
    val question2 = Question<Boolean>("The sky is green. True or false", false, Difficulty.easy)
    val question3 = Question<Int>("How many days are there between full moons?", 28, Difficulty.hard)
}
class Question<T>(val question:String,val answer:T,val difficulty:Difficulty){   
}
enum class Difficulty{
    easy,medium,hard
}
```

### 数据类

数据类的属性会自动实现toString等方法

```kontlin
data class Question<T>(val question:String,val answer:T,val difficulty:Difficulty){
}
```

### 单例（对象）

```kontlin
object StudentProgress {
    var total: Int = 10
    var answered: Int = 3
}
```

### 伴生对象

```kontlin
fun main(args: Array<String>) {
    println(Quiz.answered)
}
class Quiz(){
    val question1 = Question<String>("Quoth the raven ___", "nevermore", Difficulty.medium)
    val question2 = Question<Boolean>("The sky is green. True or false", false, Difficulty.easy)
    val question3 = Question<Int>("How many days are there between full moons?", 28, Difficulty.hard)

    companion object StudentProgress{
        var total:Int = 0
        var answered:Int = 3
    }
}
```

### 扩展属性

比如对Quiz类添加一个progressText属性，扩展属性无法存储数据，因此它们必须是 get-only 的

```kotlin
class Quiz(){
    // ...
}
// 扩展一个属性 后面main可以直接 Quiz.progressText访问
val Quiz.StudentProgress.progressText:String
    get() = "${answered} of ${total} answered"
```

### 扩展函数

```kotlin
class Quiz(){
    // ...
}
fun Quiz.StudentProgress.printProgressBar(){
    repeat(Quiz.answered){print("▓")}
}
```

### 接口

```kotlin
fun main(args: Array<String>) {
    Quiz().printProgressBar()
}
class Quiz():ProgressPrintable{
    val question1 = Question<String>("Quoth the raven ___", "nevermore", Difficulty.medium)
    val question2 = Question<Boolean>("The sky is green. True or false", false, Difficulty.easy)
    val question3 = Question<Int>("How many days are there between full moons?", 28, Difficulty.hard)

    companion object StudentProgress{
        var total:Int = 10
        var answered:Int = 3
    }

    override val progressText: String
        get() = "${answered} of ${total} answered"

    override fun printProgressBar() {
        repeat(Quiz.answered) { print("▓") }
        repeat(Quiz.total - Quiz.answered) { print("▒") }
        println()
        println(progressText)
    }
}
interface ProgressPrintable {
    val progressText:String
    fun printProgressBar()
}
```

### Let

let会返回lambda最后一行的结果，如果没有就返回Unit

```kotlin
fun main(args: Array<String>) {
    Quiz().printProgressBar()
    val quiz = Quiz()
    quiz.let {
        it.question1.let {
            println(it.question)
            println(it.answer)
            println(it.difficulty)
        }
        it.question2.let {
            println(it.question)
            println(it.answer)
            println(it.difficulty)
        }
    }
}
```

### Apply

```kotlin
// 原始方式
Quiz().printProgressBar()
val quiz = Quiz()
// apply方式
Quiz().apply {
    printProgressBar()
}
```

## 集合

### 数组

比较简单，例子：`val rockPlanets = arrayOf<String>("1","2")` `<String>`可以省略，会自动推断，访问还是`rockPlanets[0]`

### List

1. 不可变 `val mylist = listOf("1","2")`，可变`var mylist = mutableListOf("1","2")`
2. 访问还是可以通过`[]`或者`get()`
3. 支持`indexOf`，`add(index),add(index,item)`，`remove(item)，removeAt(index)`，`contains(item)`
4. 遍历：`for(x in mylist){}`

### Set

1. 也分可变和不可变`setOf("1","2")和mutableSetOf()`
2. 支持`add`,`contains`,`remove`

### Map

1. 也分可变和不可变
2. 支持`remove`,支持`put(k,v)`
3. 可以通过下标访问和修改，也可以通过`get`访问，不论是下标语法还是调用 get，key不存在都返回null
4. 遍历，可以直接遍历key：`for(key in solarSystem.keys){}`，也支持遍历valus:`for(v in solarSystem.values)`
5. lambda遍历`res.forEach{(k,v)-> println("name:$k ,price:${v}")}`

```kontlin
val solarSystem = mutableMapOf(
    "Mercury" to 0,
    "Venus" to 0,
    "Earth" to 1,
    "Mars" to 2,
    "Jupiter" to 79,
    "Saturn" to 82,
    "Uranus" to 27,
    "Neptune" to 14
)
solarSystem["Pluto"] = 5

// 遍历也支持这种形式
for(item in solarSystem){
        println(item.key+" "+item.value)
    }

```

## 常用Lambda操作

`val cookies = listOf(Cookie(...),Cookie(...),......)`

### forEach

```kotlin
    cookies.forEach{
        println(it.name)
    }
```

### map
```kontlin
    val res = cookies.map {
        it.name.uppercase()
    }
    res.forEach{println(it)}
```

### filter
```kontlin
    val res = cookies.filter {
        it.price>1.5
    }
    res.forEach{println(it.name)}
```

### groupBy
```kontlin
    val res = cookies.groupBy { it.price }
    res.forEach{println(it)}

// 输出结果是个Map<it.price的类型,Cookie类型>
1.69=[Cookie@2c7b84de]
1.49=[Cookie@3fee733d, Cookie@5acf9800]
1.59=[Cookie@4617c264]
1.39=[Cookie@36baf30c, Cookie@7a81197d]
1.79=[Cookie@5ca881b5]
```

### fold 

和reduce很相似，区别是要传入初始值，空集合返回初始值不抛出异常

用于从集合中生成单个值。这最常用于计算总价，或汇总列表中的所有元素以求平均值

```kontlin
//    val result = collection.fold(initialValue) { acc, item ->
//        // 返回新的 acc 值
//    }
    val totalPrice = cookies.fold(0.0){ total,cookie ->
        total+cookie.price
    }
    println(totalPrice)
```
### sortedBy和sortedByDescending
```kontlin
// 递增排序
    val res = cookies.sortedBy{
        it.price
    }
    res.forEach{println("name:${it.name} ,price:${it.price}")}
// 递减排序
    val res = cookies.sortedByDescending{
        it.price
    }
    res.forEach{println("name:${it.name} ,price:${it.price}")}
```


## kotlin协程

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

