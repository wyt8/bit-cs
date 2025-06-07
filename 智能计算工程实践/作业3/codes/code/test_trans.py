import torch.backends.cudnn as cudnn
from utils import *
import json

from PIL import Image
import torch
import torchvision.models as models
import torchvision.transforms as transforms

# Parameters
cudnn.benchmark = True  # set to true only if inputs to model are fixed size; otherwise lot of computational overhead

device = torch.device("cuda:2" if torch.cuda.is_available() else "cpu")

# 加载预训练的 ResNet-101 模型
resnet = models.resnet101(pretrained=True)
resnet.eval()  # 设置模型为评估模式

# 移除全局平均池化层和全连接层
resnet = torch.nn.Sequential(*(list(resnet.children())[:-2])).to(device)

# 定义预处理步骤
transform = transforms.Compose(
    [
        transforms.Resize(224),  # 将输入图片大小调整为224×224，以便输出16×16
        transforms.ToTensor(),  # 将图片转换为Tensor
        transforms.Normalize(
            mean=[0.485, 0.456, 0.406], std=[0.229, 0.224, 0.225]
        ),  # 标准化
    ]
)

def get_key(d: dict, v):
    for key, value in d.items():
        if value == v:
            return key


def test(checkpoint_path, image_folder, sence_json_path, wordmap_json_path):
    # 加载模型
    checkpoint = torch.load(checkpoint_path, map_location=device)

    encoder = checkpoint["encoder"]
    encoder = encoder.to(device)
    encoder.eval()

    decoder = checkpoint["decoder"]
    decoder = decoder.to(device)
    decoder.eval()

    # 加载word_map文件
    with open(wordmap_json_path, "r") as f:
        word_map = json.load(f)

    # 加载sence文件
    with open(sence_json_path, "r") as f:
        sence = json.load(f)

    input = []  # 模型的输入
    img_path = os.path.join(image_folder, sence["image_filename"])
    img = Image.open(img_path)  # 读取图片
    # 检查图片的模式
    # 如果图片是RGBA模式（即带有透明度通道），则转换为RGB模式
    if img.mode == "RGBA":
        img = img.convert("RGB")
    # 应用预处理流程
    img = transform(img)
    # 添加一个维度
    img = img.unsqueeze(0).to(device)
    # 通过resnet提取特征
    with torch.no_grad():
        img = resnet(img)

    input.append(img)

    img_filename_pre = sence["image_filename"][:-4]
    img_filename_suf = ""
    for index, item in enumerate(sence["change_record"]):
        img_filename_suf += "_" + item
        img_filename = img_filename_pre + "_change" + str(index) + img_filename_suf

        img_path = os.path.join(image_folder, img_filename + ".png")
        img = Image.open(img_path)  # 读取图片
        # 检查图片的模式
        # 如果图片是RGBA模式（即带有透明度通道），则转换为RGB模式
        if img.mode == "RGBA":
            img = img.convert("RGB")
        # 应用预处理流程
        img = transform(img)
        # 添加一个维度
        img = img.unsqueeze(0).to(device)
        # 通过resnet提取特征
        with torch.no_grad():
            img = resnet(img)

        input.append(img)

    # 输入encoder
    memory = encoder(input)
    print(memory.shape)

    tgt = torch.zeros(200, 1).to(device).to(torch.int64)
    tgt_length = tgt.size(0)
    mask = (torch.triu(torch.ones(tgt_length, tgt_length)) == 1).transpose(0, 1)
    mask = (
        mask.float()
        .masked_fill(mask == 0, float("-inf"))
        .masked_fill(mask == 1, float(0.0))
    )
    mask = mask.to(device)
    tgt[0, 0] = word_map["<start>"]
    seq = []
    for i in range(tgt_length - 1):
        tgt_embedding = decoder.vocab_embedding(tgt)
        tgt_embedding = decoder.position_encoding(
            tgt_embedding
        )  # (length, batch, feature_dim)

        pred = decoder.transformer(
            tgt_embedding, memory, tgt_mask=mask
        )  # (length, batch, feature_dim)
        pred = decoder.wdc(pred)  # (length, batch, vocab_size)

        pred = pred[i, 0, :]
        predicted_id = torch.argmax(pred, axis=-1)

        ## if word_map['<end>'], end for current sentence
        if predicted_id == word_map["<end>"]:
            break

        seq.append(predicted_id)

        ## update mask, tgt
        tgt[i + 1, 0] = predicted_id
        mask[i + 1, 0] = 0.0

    tgt = tgt.cpu()
    tgt = tgt.reshape(-1)

    word_list = []
    for word in tgt:
        if word not in (word_map["<start>"], word_map["<end>"], word_map["<pad>"], word_map["<unk>"]):
            word_list.append(get_key(word_map, word))

    sentence = word_list[0]
    for word in word_list[1:]:
        if word in (".", ":", ","):
            sentence += word
        else:
            sentence += " " + word
    print(f"输出的句子如下：\n")
    print(sentence)


if __name__ == "__main__":
    checkpoint_path = "./results/checkpoint_epoch_9_3dcc_5_cap_per_img_0_min_word_freq.pth.tar"
    image_folder = "../test_output/images"
    sence_json_path = "../test_output/scenes/CLEVR_new_000009.json"
    wordmap_json_path = "../output/WORDMAP_3dcc_5_cap_per_img_0_min_word_freq.json"
    test(checkpoint_path, image_folder, sence_json_path, wordmap_json_path)
