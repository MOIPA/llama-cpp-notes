# ONNX

> Open Neural Network Exchange 

开放的神经网络模型格式标准,就像图像中的 .jpg 或 .png，但它是为 AI 模型设计的。它可以让 AI 模型在不同框架（如 PyTorch、TensorFlow、Scikit-learn）和不同设备（如 PC、手机、边缘设备）之间互相兼容和迁移

+ ONNX 模型（.onnx）:一个标准化的文件格式，描述神经网络结构和参数
+ ONNX Runtime	:微软开发的高性能推理引擎，支持 ONNX 模型
+ 转换器（如 torch.onnx.export）:将 PyTorch、TensorFlow 模型转换为 ONNX 格式
+ 兼容性:支持 PyTorch、TensorFlow、Keras、Scikit-learn、XGBoost 等

## 和VLLM区别

+ VLLM专注于LLM部署，而onnx是通用模型格式，不止支持LLM还支持CV等机器学习和深度学习模型
+ VLLM 支持 LoRA，onnx不支持
+ VLLM 常用于服务器部署，onnx更多是边缘设备

## 和llama.cpp区别

+ llama.cpp 只支持transformer结构，只为LLM提供部署服务和量化服务
+ llama.cpp 可以量化到q_2，onnx量化只到int8
+ llama.cpp 模型小，极致内存占用对边缘设备更友好
+ llama.cpp 只支持C/C++，需要其他语言支持可以看其他项目比如llama.rn对其进行的拓展，而onnx支持多种语言包括java等
+ llama.cpp 模型来源自hg，需要从hg的模型转换为gguf格式，而onnx支持直接从pytorch和tensorflow，TVM等导出模型

## 使用

ONNX支持多种生态，我的技术栈是pytorch+huggingface的Transformers库那一套，hg有专门的加速在硬件推理和训练的库：`Optimum`，支持转换为onnx模型并且运行在`onnx runtime`。

1. 安装依赖库：`python -m pip install optimum`

可以使用optimum库快速方便的加速训练和推理，并且将模型转为onnx格式，不过再这之前还需要指定支持onnx的运行环境，比较onnx只是种格式

2. onnx运行环境，有很多我选择微软的onnxruntime：`pip install --upgrade --upgrade-strategy eager optimum[onnxruntime]` 或者`pip install onnxruntime`

** 注意：zsh环境下中括号会被转义！ install的目标要加上双引号防止转义 **

3. 安装transformers和pytorch环境:`pip install transformers[torch]`

4. 安装进度条增加体验：`conda install ipywidgets`

### 快速体验

> 如果手头没有训练项目，可以参考官方demo，相对于原版的transformers使用，需要改动的地方很小

```python
- from transformers import AutoModelForSequenceClassification
+ from optimum.intel.openvino import OVModelForSequenceClassification
  from transformers import AutoTokenizer, pipeline

  # Download a tokenizer and model from the Hub and convert to OpenVINO format
  tokenizer = AutoTokenizer.from_pretrained(model_id)
  model_id = "distilbert-base-uncased-finetuned-sst-2-english"
- model = AutoModelForSequenceClassification.from_pretrained(model_id)
+ model = OVModelForSequenceClassification.from_pretrained(model_id, export=True)

  # Run inference!
  classifier = pipeline("text-classification", model=model, tokenizer=tokenizer)
  results = classifier("He's a dreadful magician.")
```
### 使用onnx量化

以下是一个量化的例子，并将量化后的模型保存在本地，模型大小只剩1/3了，说明量化非常省内存和空间

加载量化模型也很简单，tokenizer不变，只需要改变model，改为加载量化的模型即可。

```python
from optimum.onnxruntime.configuration import AutoQuantizationConfig
from optimum.onnxruntime import ORTQuantizer

qconfig = AutoQuantizationConfig.arm64(is_static=False, per_channel=False)
quantizer = ORTQuantizer.from_pretrained(model)
quantizer.quantize(save_dir="./quanted_model", quantization_config=qconfig)

# 加载一个量化模型
from optimum.onnxruntime import ORTModelForSequenceClassification
from transformers import pipeline, AutoTokenizer

model = ORTModelForSequenceClassification.from_pretrained("./quanted_model", file_name="model_quantized.onnx")
tokenizer = AutoTokenizer.from_pretrained("./quanted_model")
classifier = pipeline("text-classification", model=model, tokenizer=tokenizer)
results = classifier("I love burritos!")
results
```

### 使用onnx runtime训练

官方提供了自己的Trainer和TrainingArguments

```python
- from transformers import Trainer, TrainingArguments
+ from optimum.onnxruntime import ORTTrainer, ORTTrainingArguments

  # Download a pretrained model from the Hub
  model = AutoModelForSequenceClassification.from_pretrained("bert-base-uncased")

  # Define the training arguments
- training_args = TrainingArguments(
+ training_args = ORTTrainingArguments(
      output_dir="path/to/save/folder/",
      optim="adamw_ort_fused",
      ...
  )

  # Create a ONNX Runtime Trainer
- trainer = Trainer(
+ trainer = ORTTrainer(
      model=model,
      args=training_args,
      train_dataset=train_dataset,
+     feature="text-classification", # The model type to export to ONNX
      ...
  )

  # Use ONNX Runtime for training!
  trainer.train()
```

### 和PEFT的区别

optimum库虽然可以用来做模型训练加速，但是很少这么做，而且仅当需要全量调参的时候用，peft是专门做微调的。

optimum大多数时候都是用来做推理加速和peft是相辅相成的

```
[原始模型] 
    ↓ 微调（LoRA） ← PEFT
[微调后的 LoRA 模型]
    ↓ 合并权重（可选）
[适配下游任务的模型]
    ↓ 转换 ONNX / 量化 ← Optimum
[部署模型]
```

peft里的lora或者说qlora是对模型在内存中的存储进行量化，目的还是训练lora自己矩阵参数。训练结束后模型卸载，得到lora模型。下次使用还是正常加载。而onnx是可以将模型量化保存在本地的。