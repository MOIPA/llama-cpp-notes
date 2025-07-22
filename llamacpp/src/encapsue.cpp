// 我自己对llama.cpp核心使用api的封装

#include <string>
#include <math.h>
#include "../include/llama.h"
#include "../include/common.h"

std::string cached_token_chars;

llama_model * loadModel(std::string fileName){
    llama_model_params model_params = llama_model_default_params();
    printf("loading model\n");
    llama_model * model = llama_model_load_from_file(fileName.c_str(),model_params);
    if(!model){
        printf("unable to load model\n");
        return nullptr;
    }
    return model;
}

void freeModel(llama_model* model){
    llama_model_free(model);
}

llama_context * newContext(llama_model* model,int context_size=2048){
    llama_context_params ctx_params  = llama_context_default_params();
    ctx_params.n_ctx = context_size;
    llama_context * context = llama_new_context_with_model(model,ctx_params);
    if(!context){
        printf("failed to load context\n");
    }
    return context;
}

void freeContext(llama_context* context){
    llama_free(context);
}

void loadBackendInit(){
    llama_backend_init();
}

/**
 * 本函数创建空的batch，用于装载数据
 * @param n_tokens  这个batch最多有多少个token
 * @param embd      输入是否是embedding，如果是传入embedding的长度，一般都是0 不采用embd
 * @param n_seq_max 最大序列，同时处理多个用户的问题时，一般都是1
 */
llama_batch* newBatch(int n_tokens,int embd=0,int n_seq_max=1){
    llama_batch *batch = new llama_batch{
        0,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
    };
    // 开辟存储token或者embedding的空间
    if(embd){
        batch->embd = (float *)malloc(sizeof(float)* n_tokens * embd);  // 存储embedding
    }else{
        batch->token = (llama_token *) malloc(sizeof(llama_token) * n_tokens);
    }
    // 存储每个token的位置信息
    batch->pos = (llama_pos *)malloc(sizeof(llama_pos)* n_tokens);
    // 存储每个token对应多少个序列  如果一个token在三个样本中都出现，就是3
    batch->n_seq_id = (int32_t *)malloc(sizeof(int32_t)* n_tokens);
    // 存储每个token对应的序列数组 如果一个token在三个样本中出现，且序列号是10,11,3
    batch->seq_id = (llama_seq_id **) malloc(sizeof(llama_seq_id *) * n_tokens);
    for (int i = 0; i < n_tokens; ++i) {
        batch->seq_id[i] = (llama_seq_id *) malloc(sizeof(llama_seq_id) * n_seq_max);
    }
    // 每个batch最终计算出来的logits，需要采样器采样成token id 再通过vocab转为实际字符
    // 这里的logits是标志位，表示token是否需要计算它对应的下一个字符logits
    // 这样的好处是，表示位为false的时候，只需要更新kv缓存不用计算logits
    batch->logits   = (int8_t *)        malloc(sizeof(int8_t) * n_tokens);

    return batch;
}

void freeBatch(llama_batch * batch){
    delete batch;
}

llama_sampler* newSampler(){
    llama_sampler_chain_params params = llama_sampler_chain_default_params();
    params.no_perf=true;
    llama_sampler * sampler = llama_sampler_chain_init(params);
    llama_sampler_chain_add(sampler,llama_sampler_init_greedy());
    return sampler;
}

void freeSampler(llama_sampler * sampler){
    llama_sampler_free(sampler);
}

/**
 * benchmark 模型
 * @param pp    prompt process，处理提示词的长度，主要是计算kv缓存和最后一个字符的下一字符logits
 * @param tg    text generation 生成内容长度，每个字符都要计算kv缓存+logits
 * @param pl    paral line  并行生成数量，同时处理三个用户的内容就是3，一般是1
 * @param nr    number run  测试的轮数，一般是1或者3，轮数越多测速结果越平均
 */
std::string benchModel(llama_context * context,llama_model* model,
    llama_batch * batch,int pp,int tg,int pl,int nr){
    auto pp_avg = 0.0;
    auto tg_avg = 0.0;
    auto pp_std = 0.0;
    auto tg_std = 0.0;
    // 上下文最大长度
    int n_ctx = llama_n_ctx(context);
    printf("n_ctx = %d\n", n_ctx);
    int i, j;
    int nri;
    for (nri = 0; nri < nr; nri++) {
        // 每一轮计算提示词处理和生成结果处理的速度
        // 提示词处理速度
        printf("Benchmark prompt processing (pp)\n");
        common_batch_clear(*batch); // 清空batch空间先
        const int n_tokens = pp;
        for (i = 0; i < n_tokens; i++) {
            common_batch_add(*batch, 0, i, { 0 }, false);
        }
        batch->logits[batch->n_tokens - 1] = true;
        llama_memory_clear(llama_get_memory(context), false); // 计算前清理kv缓存
        const auto t_pp_start = ggml_time_us();
        if (llama_decode(context, *batch) != 0) {           // 计算kv缓存和logits
            printf("llama_decode() failed during prompt processing\n");
        }
        const auto t_pp_end = ggml_time_us();

        // 生成处理速度
        printf("Benchmark text generation (tg)\n");
        llama_memory_clear(llama_get_memory(context), false);
        const auto t_tg_start = ggml_time_us();
        for (i = 0; i < tg; i++) {
            common_batch_clear(*batch);
            for (j = 0; j < pl; j++) {
                common_batch_add(*batch, 0, i, { j }, true);
            }
            printf("llama_decode() text generation: %d\n", i);
            if (llama_decode(context, *batch) != 0) {
                printf("llama_decode() failed during text generation\n");
            }
        }
        const auto t_tg_end = ggml_time_us();
        llama_memory_clear(llama_get_memory(context), false);
        // 计算速度
        const auto t_pp = double(t_pp_end - t_pp_start) / 1000000.0;
        const auto t_tg = double(t_tg_end - t_tg_start) / 1000000.0;

        const auto speed_pp = double(pp) / t_pp;
        const auto speed_tg = double(pl * tg) / t_tg;

        pp_avg += speed_pp;
        tg_avg += speed_tg;

        pp_std += speed_pp * speed_pp;
        tg_std += speed_tg * speed_tg;

        printf("pp %f t/s, tg %f t/s\n", speed_pp, speed_tg);
    }

    pp_avg /= double(nr);
    tg_avg /= double(nr);
    if (nr > 1) {
        pp_std = sqrt(pp_std / double(nr - 1) - pp_avg * pp_avg * double(nr) / double(nr - 1));
        tg_std = sqrt(tg_std / double(nr - 1) - tg_avg * tg_avg * double(nr) / double(nr - 1));
    } else {
        pp_std = 0;
        tg_std = 0;
    }
    char model_desc[128];
    llama_model_desc(model, model_desc, sizeof(model_desc));

    const auto model_size     = double(llama_model_size(model)) / 1024.0 / 1024.0 / 1024.0;
    const auto model_n_params = double(llama_model_n_params(model)) / 1e9;

    const auto backend    = "(Android)"; // TODO: What should this be?

    std::stringstream result;
    // result << std::setprecision(2);
    result << "| model | size | params | backend | test | t/s |\n";
    result << "| --- | --- | --- | --- | --- | --- |\n";
    result << "| " << model_desc << " | " << model_size << "GiB | " << model_n_params << "B | " << backend << " | pp " << pp << " | " << pp_avg << " ± " << pp_std << " |\n";
    result << "| " << model_desc << " | " << model_size << "GiB | " << model_n_params << "B | " << backend << " | tg " << tg << " | " << tg_avg << " ± " << tg_std << " |\n";
    return result.str();
}

/**
 * 初始化对话，主要工作是处理提示词，生成kv缓存计算最后一个字符对应的下一个字符的logits
 * @param formatChat    是否处理提示词里的特殊字符，如果false，<EOS>这种特殊字符普通处理了
 * @param nLen          生成的长度
 * @param text          输入的提示词
 * @return 返回提示词最后一词的位置，用于后面loop添加batch的时候提供初始位置
 */
int completionInit(llama_context * context,llama_batch* batch, std::string text,bool formatChat,int n_len){
    cached_token_chars.clear(); // 清理全局缓存
    const auto tokens_list = common_tokenize(context,text,true,formatChat); // 分词
    auto n_ctx = llama_n_ctx(context);  // 上下文长度
    auto n_kv_req = tokens_list.size() + n_len; // kv缓存需要的上下文长度
    printf("n_len = %d, n_ctx = %d, n_kv_req = %d\n", n_len, n_ctx, n_kv_req);
    // kv缓存空间不足
    if (n_kv_req > n_ctx) {
        printf("error: n_kv_req > n_ctx, the required KV cache size is not big enough\n");
    }
    // 每个token转回字符打印，看存不存在分词问题，调试用的
    for (auto id : tokens_list) {
        printf("token: `%s`-> %d \n", common_token_to_piece(context, id).c_str(), id);
    }
    common_batch_clear(*batch); // 清空batch
    for (auto i = 0; i < tokens_list.size(); i++) { // 添加提示词每个token到batch，只有一个序列
        common_batch_add(*batch, tokens_list[i], i, { 0 }, false);
    }
    batch->logits[batch->n_tokens - 1] = true; // 最后一个token需要计算logits
    if (llama_decode(context, *batch) != 0) {
        printf("llama_decode() failed\n");
    }
    return batch->n_tokens;  // 返回提示词的全部tokens数量
}

std::string completionLoop(){

}