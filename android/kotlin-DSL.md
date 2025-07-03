# Kotlin的DSL模式详解

例子

```kotlin
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