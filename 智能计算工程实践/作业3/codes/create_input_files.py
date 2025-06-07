import os
import h5py
import json
from PIL import Image
import torch
import torchvision.models as models
import torchvision.transforms as transforms

device = torch.device("cuda:1" if torch.cuda.is_available() else "cpu") 

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


def from_str_to_words(s: str) -> list:
    """
    从字符串转换为单词列表
    """
    res = []
    for w in s.split(" "):
        if w[-1] in (",", ":", "."):
            res.append(w[:-1])
            res.append(str(w[-1]))
        else:
            res.append(w)
    return res


def create_input_files(
    caption_json_path,
    image_folder,
    wordmap_json_path,
    output_folder,
    max_len=200,
    data_name="3dcc_5_cap_per_img_0_min_word_freq",
):
    """
    Creates input files for training, validation, and test data.

    :param dataset: name of dataset, one of 'coco', 'flickr8k', 'flickr30k'
    :param karpathy_json_path: path of Karpathy JSON file with splits and captions
    :param image_folder: folder with downloaded images
    :param captions_per_image: number of captions to sample per image
    :param min_word_freq: words occuring less frequently than this threshold are binned as <unk>s
    :param output_folder: folder to save files
    :param max_len: don't sample captions longer than this length
    """

    # 读取change_caption.json文件
    with open(caption_json_path, "r") as j:
        caption_data = json.load(j)

    # 读取WORDMAP_3dcc_5_cap_per_img_0_min_word_freq.json文件
    with open(wordmap_json_path, "r") as j:
        wordmap_data = json.load(j)

    split = "TRAIN"
    with h5py.File(
        os.path.join(output_folder, split + "_IMAGE_FEATURES_1_" + data_name + ".h5"),
        "w",
    ) as h1, h5py.File(
        os.path.join(output_folder, split + "_IMAGE_FEATURES_2_" + data_name + ".h5"),
        "w",
    ) as h2:
        # Create dataset inside HDF5 file to store images
        images_features1 = h1.create_dataset(
            "images_features", (len(caption_data), 2048, 7, 7), dtype="float32"
        )
        images_features2 = h2.create_dataset(
            "images_features", (len(caption_data), 2048, 7, 7), dtype="float32"
        )
        enc_captions = []
        caplens = []
        seqs = []
        for index, item in enumerate(caption_data):
            print(f"正在生成第{index+1}个")
            img0_path = os.path.join(image_folder, item["image_0"])
            img1_path = os.path.join(image_folder, item["image_1"])
            # 读取图片
            img0 = Image.open(img0_path)
            # 检查图片的模式  
            # 如果图片是RGBA模式（即带有透明度通道），则转换为RGB模式  
            if img0.mode == 'RGBA':  
                img0 = img0.convert('RGB')  
            # 应用预处理流程
            img0 = transform(img0)
            # 添加一个维度
            img0 = img0.unsqueeze(0).to(device)
            # 通过resnet提取特征
            with torch.no_grad():
                img0 = resnet(img0)
            # 压缩一个维度
            img0 = img0.squeeze(0).cpu()
            # 存入h5文件中
            images_features1[index] = img0
            # 读取图片
            img1 = Image.open(img1_path)
            # 检查图片的模式  
            # 如果图片是RGBA模式（即带有透明度通道），则转换为RGB模式  
            if img1.mode == 'RGBA':  
                img1 = img1.convert('RGB')  
            # 应用预处理流程
            img1 = transform(img1)
            # 添加一个维度
            img1 = img1.unsqueeze(0).to(device)
            # 通过resnet提取特征
            with torch.no_grad():
                img1 = resnet(img1)
            # 压缩一个维度
            img1 = img1.squeeze(0).cpu()
            # 存入h5文件中
            images_features2[index] = img1

            # # 添加batch维度（如果需要的话）
            # input_batch = input_tensor.unsqueeze(0)  # 这会将tensor的形状从[C, H, W]变为[B, C, H,

            seqs.append(item["seq"])

            enc_c = []
            enc_c.append(wordmap_data["<start>"])
            for caption in item["change_captions"]:
                # 编码描述
                for word in from_str_to_words(caption):
                    enc_c.append(wordmap_data.get(word, wordmap_data["<unk>"]))
            enc_c.append(wordmap_data["<end>"])
            c_len = len(enc_c)

            for _ in range(max_len - c_len):
                enc_c.append(wordmap_data["<pad>"])

            enc_captions.append(enc_c)
            caplens.append(c_len)
            
            # if (index+1) % 10 == 0:
            #     break

        # 保存captions、caplens、seqs文件
        with open(
            os.path.join(output_folder, split + "_CAPTIONS_" + data_name + ".json"),
            "w",
        ) as j:
            json.dump(enc_captions, j)
        with open(
            os.path.join(output_folder, split + "_CAPLENS_" + data_name + ".json"),
            "w",
        ) as j:
            json.dump(caplens, j)
        with open(
            os.path.join(output_folder, split + "_SEQS_" + data_name + ".json"),
            "w",
        ) as j:
            json.dump(seqs, j)


if __name__ == "__main__":
    caption_json_path = "./output/change_caption.json"
    image_folder = "./output/images"
    wordmap_json_path = "./output/WORDMAP_3dcc_5_cap_per_img_0_min_word_freq.json"
    output_folder = "./model_input"
    create_input_files(
        caption_json_path, image_folder, wordmap_json_path, output_folder
    )
