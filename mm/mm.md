# 多模态

## 经典paper

1. https://ieeexplore.ieee.org/document/8373888   多模态学习的经典综述，涵盖模型、数据集、挑战等

2. https://arxiv.org/abs/2008.07995 深度多模态融合方法的综述

3. https://arxiv.org/abs/2106.02034 视觉 Transformer 的综述，多模态中广泛使用

## 模型

### CLIP

论文
openai官方的实现（模型本身的数据和训练脚本没有开源，模型和使用方法开源了）：github：https://github.com/openai/CLIP

社区的开源实现：https://github.com/mlfoundations/open_clip

### ALIGN

### BLIP / BLIP2	

https://github.com/salesforce/BLIP

### Flamingo

### LLaVA

https://github.com/haotian-liu/LLaVA

### Qwen-VL	

##  LoRA 微调 VLM 模型


## ViT

ViT 是一种将 Transformer 架构应用于图像任务（如分类、检测、分割）的编码器。

CNN	：使用卷积提取局部特征	（ResNet，VGG等）

ViT：将图像切分为 Patch，用 Transformer 学习全局特征	（Clip等）

ViT 的基本流程：

```
graph LR
  A[原始图像] --> B[切分为 Patch]
  B --> C[将 Patch 展平为向量]
  C --> D[加上位置编码]
  D --> E[输入 Transformer]
  E --> F[输出图像特征]
```

> Projection

Projection（投影）是将图像特征（来自 ViT）映射到语言模型（如 LLaMA）所能理解的向量空间中的过程。

> 为什么需要 Projection？

ViT 输出的特征维度通常和语言模型的词向量维度不一致

Projection 层可以是一个简单的线性层（Linear Layer），也可以是更复杂的结构（如 MLP），将图像特征转换为语言模型可以理解的形式的关键步骤

## 融合模型

将来自不同来源、不同类型或不同模态的数据进行结合，并通过一个统一的模型进行处理和理解的模型结构

多模态问答：LLaVA、BLIP、MobileVLM

### 融合模式

+ 早期融合（Early Fusion）	在输入层就将多模态数据拼接在一起，统一处理	输入A + 输入B → 融合 → 模型处理
+ 中期融合（Intermediate Fusion）	每个模态先经过各自的编码器，再在中间层融合	输入A/B →编码A/B → 融合A和B → 后续处理
+ 晚期融合（Late Fusion）	每个模态单独处理，最后融合结果	
    
    输入A→模型A → 输出A → 融合 → 最终输出
    
    输入B→模型B → 输出B → 融合

### 融合后操作

融合后的处理方式有哪些？

比如中期融合：两个模型各自处理输入进行 embedding（图像和文本），图像向量再经过 projection，两者融合后处理

|处理方式	|说明	|适用场景|
|---|---|---|
|MLP Head	|使用一个简单的全连接网络对融合后的向量进行处理	|分类、回归任务|
|Transformer Decoder 块	|使用一个或多个 Transformer 解码器层	|生成任务、问答任务|
|Cross-Attention Layer	|让图像与文本之间进行信息交互	|更精细的模态对齐|
|Pooling + Classifier	|对融合后的向量进行池化后分类	|简单任务、图像检索|